/*******************************************************************************
 * Copyright 2019 NAVER Corp
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
//
// Created by eeliu on 12/8/2019.
//

#include <string.h>
#include <assert.h>

#include "SharedObj.h"

#if defined(__linux__) || defined(_UNIX) ||defined(__APPLE__)
#define SHARE_OBJ_HEADER "pinpoint.shm"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>



#pragma pack(push, 1)
typedef struct shared_file_header_s{
    char header[sizeof(SHARE_OBJ_HEADER)];
    time_t create_time;
}SharedFileHeaderT;
#pragma pack(pop)

static inline int padding_pagesize(int length)
{
    int64_t pagesize = sysconf(_SC_PAGESIZE);
    return (pagesize >= length) ? (pagesize):((length + pagesize)&(~pagesize));
}

static bool set_shm_file_header(int fd,int length)
{
    // extend the new file
    struct stat _stat;
    fstat(fd,&_stat);
    assert(_stat.st_size == 0);

    if(ftruncate(fd,length) == -1)
    {
        pp_trace("trancate %d failed:%s",fd,strerror(errno));
        return false;
    }
    // due to phpt, remove this line
    // pp_trace("ftruncate the size to:%ld",length);

    void* addr = mmap(NULL,length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(addr == MAP_FAILED)
    {
        pp_trace("mmap %d length:%d %s ",fd ,length,strerror(errno));
        return false;
    }
    SharedFileHeaderT* header = (SharedFileHeaderT*)addr;
    strcpy(header->header,SHARE_OBJ_HEADER);
    header->create_time = time(NULL);
    munmap(addr,length);

    return true;
}

static inline bool get_shm_body_region(int fd,int fileSize,SharedObject_T* obj)
{
    void* addr = mmap(NULL,fileSize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(addr == MAP_FAILED)   
    {
        pp_trace("mmap %d length:%d %s ",fd ,fileSize,strerror(errno));
        return false;
    }
    obj->region = (char*)addr + sizeof(SharedFileHeaderT);
    obj->length = fileSize - sizeof(SharedFileHeaderT);
    return true;
} 

static int attach_file(const char* filepath,int length)
{
    int flag = O_RDWR;
    mode_t mode = S_IRUSR |S_IWUSR|S_IRGRP|S_IWGRP;
    // 2021-05-20 10:30:52 +0800
    // if address exist, just return true
    int fd = shm_open(filepath,flag,mode);
    if( fd > 0)
    {
        return fd;
    }

    // not exist 
    flag |=  O_CREAT | O_EXCL;
    fd = shm_open(filepath,flag,mode);
    if(fd == -1){
        if(errno == EEXIST ){
            return attach_file(filepath,length);
        }
        pp_trace("%s: shm_open:%s error:%s \n",filepath,__FUNCTION__,strerror(errno));
        return -1;
    }

    return set_shm_file_header(fd,length) ? (fd):(-1);
}

static inline void rename_address(char *address,int max_size,const char* prefix,int length){
    snprintf(address,max_size,"%s-%u-%d.shm",prefix,getuid(),length);
}

bool attach_shared_memory(SharedObject_T* shm_obj)
{
    assert(shm_obj);
    char shm_name[NAME_MAX]={0};
    int realLen = padding_pagesize(shm_obj->length);
    rename_address(shm_name,NAME_MAX,shm_obj->fileprefix,shm_obj->length);

    int fd = attach_file(shm_name,realLen);
    if(fd == -1){
        return false;
    }

    return get_shm_body_region(fd,realLen,shm_obj);
}


bool detach_shared_memory(SharedObject_T* shm_obj)
{
    void* body = shm_obj->region;
    SharedFileHeaderT* header = (SharedFileHeaderT* )((char*)body - sizeof(SharedFileHeaderT));
    if(strcmp(header->header,SHARE_OBJ_HEADER) == 0){
        munmap(header,shm_obj->length+sizeof(SharedFileHeaderT));
        return true;
    }
    pp_trace("not found header mark on shm_obj:%p",shm_obj);
    return false;
}

time_t get_shared_memory_create_time(SharedObject_T* shm_obj)
{
    assert(shm_obj);
    void* body = shm_obj->region;
    SharedFileHeaderT* header = (SharedFileHeaderT* )((char*)body - sizeof(SharedFileHeaderT));
    return header->create_time;
}


#elif _WIN32

#error "No support on win32"

#include <windows.h>

typedef struct shared_object_s{
    HANDLE hMap;
    LPVOID region;
    int length;
    const char *address;
}SharedObject_T;

static SharedObject_T object;

#define SHARE_OBJ_NAME "pinpoint-php.shm"


bool pre_init_shared_object()
{
    memset(&object,0, sizeof(SharedObject_T));
    object.address = SHARE_OBJ_NAME;
    return true;
}

bool init_shared_obj()
{
    assert(object.address);

    LPVOID pBuffer = NULL;
    HANDLE hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, object.address);
    if(hMap == NULL)
    {
        hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                   NULL,PAGE_READWRITE, 0,4096,object.address);
        if(hMap == NULL)
        {
            goto FAILED;
        }
        pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if(pBuffer == NULL){
            goto FAILED;
        }
        object.region = pBuffer;
    }else{
        object.hMap =hMap;
        pBuffer = ::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if(pBuffer == NULL){
            goto FAILED;
        }
        object.region = pBuffer;
        object.length = 4096;
    };

    return true;
FAILED:
    if(hMap){
        ::CloseHandle(hMap);
    }

    //::UnmapViewOfFile(pBuffer);

    pp_trace("init_shared_obj failed with [%ld]",GetLastError());
    return false;
}


void detach_shared_obj()
{

    ::UnmapViewOfFile(object.region);
    ::CloseHandle(object.hMap);
}


#else

#error "not support platform"
#endif


