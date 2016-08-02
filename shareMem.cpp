#include "stdafx.h"
#include "shareMem.h"


/*create＿file用来说明是用磁盘文件映射还是页文件映射，如果用磁盘文件映射，共享内存不会出现存储器release后出现违规访问的问题，但是会在磁盘上建立一个文件，文件的名称由参数shm＿name给定．如果用页文件映射，则不会在磁盘上建立一个文件*/
shareMemory::shareMemory(const string& shm_name, bool create_file)
{   
	m_is_create_file = create_file;
	const char * _c_shm_name = shm_name.c_str();
	int _size =(int)shm_name.length()+1;
	m_shm_name_u=(LPWSTR)malloc(_size*2);
	MultiByteToWideChar(CP_ACP,0,_c_shm_name,_size,m_shm_name_u,_size);
	m_semaphore =  CreateSemaphore(NULL,1,1,NULL);
	m_sh_base = NULL;
	m_file_mapping = INVALID_HANDLE_VALUE;
}

void * shareMemory::shm_open(void* addr,const int length, DWORD protect)
{   m_addr_len = length; 
	HANDLE _file_handle = INVALID_HANDLE_VALUE;
	if(m_is_create_file)
	{
		_file_handle=
			CreateFile(m_shm_name_u,GENERIC_READ|GENERIC_WRITE,0,
			NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	#ifdef _DEBUG
		if(_file_handle==INVALID_HANDLE_VALUE)
			cout<<"can not create file! we use page file instead!";
	#endif  //end _DEBUG
	}

	/*打开指定名称的文件映射，如果不存在则创建一个*/
	m_file_mapping = OpenFileMapping(PAGE_READWRITE,false,m_shm_name_u );
	if(m_file_mapping!=NULL)
		goto file_mapping_exist;
	m_file_mapping = 
		CreateFileMapping(_file_handle,NULL,PAGE_READWRITE,
		0,length,m_shm_name_u);
	#ifdef _DEBUG
	assert(m_file_mapping);
	#endif 
	if(m_file_mapping==NULL)
		return NULL;
	file_mapping_exist:
	m_sh_base = MapViewOfFileEx(m_file_mapping,protect,0,0,length,addr);
	CloseHandle(_file_handle);
	return m_sh_base;
}

/*往共享内存中写数据，返回写出数据的字节个数*/
int  shareMemory::shm_write(void *dest, void * src, int size)
{   
	if(!check_adress(dest))
		return -1;
	int _write_count = (int)m_sh_base+m_addr_len - (int)dest;
	if(_write_count>size)
		_write_count = size;
	/*利用semaphore进行保护映射的区域（同一进程的不同线程调用时候才进行保护）*/
	WaitForSingleObject(m_semaphore,INFINITE);
	memcpy(dest,src,_write_count); 
	ReleaseSemaphore(m_semaphore,1,NULL);
	FlushViewOfFile(m_sh_base,_write_count);

	cout << "[write success!], writed " << _write_count << " Bytes!" << endl;
	cout << "[write Info] :" << endl;

	int *piDest = (int*)dest;
	for (int i = 0; i < _write_count/sizeof(int); ++i)
	{
		cout << piDest[i] << "\t";
	}
	cout << endl;

	return _write_count;
}

/*从共享内存中读数据，返回读出的数据字节个数＊*/
int  shareMemory::shm_read(void* src, void * dest, int size)
{
	if(!check_adress(src))
		return -1;
	int _read_count = (int)m_sh_base+m_addr_len -(int) src;
	if(_read_count>size)
		_read_count = size;

	memcpy(dest,src,_read_count);


	cout << "[Read count is: " << _read_count << "Bytes" << endl;
	cout << "[Read Info is: " << endl;
	int* pIDest = (int*)dest;
	for (int i = 0; i < _read_count/sizeof(int); ++i)
	{
		cout << pIDest[i] << "\t";
	}
	cout << endl;

	return _read_count;
}

shareMemory::~shareMemory()
{ 
	UnmapViewOfFile(m_sh_base);
	free(m_shm_name_u);
	CloseHandle(m_semaphore);
	CloseHandle(m_file_mapping);
}


/*进行地址检测*/
bool shareMemory::check_adress(void* addr)
{
	if( ( (int)addr <(int)m_sh_base) || ((int)addr > (int)m_sh_base+m_addr_len) )
	{
		SetLastError(ERROR_ACCESS_DENIED);
#ifdef _DEBUG
		printf("access denied,the destination address out of the map view!");
#endif //_DEBUG
		return  false;
	}
	return true;
}