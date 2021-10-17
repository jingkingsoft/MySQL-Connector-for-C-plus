#ifndef _SYSTEM_DEFINE
#define _SYSTEM_DEFINE

#define BOOL int
#define DWORD unsigned int
#define UINT32 unsigned int
#define UINT64 unsigned long long
#define UINT16 unsigned short
#define UINT8 unsigned char
#define BYTE unsigned char
#define LPVOID void*
#define SOCKET int
#define TRUE 1
#define FALSE 0
#define ATOMIC_BOOL atomic_int

#define SAFEDELETE(p) {if(p){delete(p);(p)=NULL;}}
#define SAFEDELETEARRAY(p) {if(p){delete[](p);(p)=NULL;}}
#define SAFECLOSEHANDLE(p) {if((p)!=NULL) {CloseHandle((p)); (p)=NULL;}}
#define SafeDeleteVector(p) {for(int i = 0; i < (p).size(); i ++){if((p)[i]){delete((p)[i]);}}(p).clear();}

#endif