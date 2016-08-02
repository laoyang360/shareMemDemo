#include <windows.h>
#include <string>
#include <iostream>
#include <assert.h>
using std::string;
using std::cout;
using std::endl;
using namespace std;

#pragma warning(disable: 4311)

class shareMemory
{
private:
	LPWSTR m_shm_name_u;
	bool  m_is_create_file;
	void * m_sh_base;
	HANDLE m_semaphore;
	HANDLE m_file_mapping;
	int  m_addr_len;
public:
	shareMemory(const string& shm_name, bool create_file=false);
	~shareMemory();

	void* shm_open(void* addr,const int length, DWORD protect);
	int  shm_write(void *dest, void * src, int size);
	int  shm_read(void* src, void * dest, int size);
	

private :
	/*½øÐÐµØÖ·¼ì²â*/
	bool check_adress(void* addr);
};