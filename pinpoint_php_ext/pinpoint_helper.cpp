////////////////////////////////////////////////////////////////////////////////
// Copyright 2019 NAVER Corp
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////
/*
 * pinpoint_helper.cpp
 *
 *  Created on: Jul 3, 2019
 *      Author: eeliu
 */
#include "pinpoint_helper.h"

int Chunks::copyDataIntoReadyList(const void* data, uint length)
{
    if (ready_list.empty())
    {
        return length;
    }
    Chunk* e = ready_list.back();

    char* e_buf_start = &e->data[e->r_ofs];
    uint in_max_len = e->block_size - e->r_ofs;
    if (e)
    {
        if (in_max_len >= length)
        {
            memcpy(e_buf_start, data, length);
            e->r_ofs += length;
            length = 0;
        }
        else if (in_max_len > 0)
        {
            memcpy(e_buf_start, data, in_max_len);
            length -= in_max_len;
            e->r_ofs += in_max_len;
        }
    }
    return length;
}

int Chunks::copyDataIntoFreeList(const void*data, uint length)
{
    if (this->free_list.empty())
    {
        return length;
    }


    iter = this->free_list.begin();

    while (iter != this->free_list.end() && length > 0)
    {
        Chunk* free = *iter;
        iter++;
        char* f_buf_start = &free->data[free->r_ofs];
        uint in_max_len = free->block_size - free->r_ofs;
        if (in_max_len >= length)
        {
            memcpy(f_buf_start, data, length);
            free->r_ofs += length;
            length = 0;
        }
        else if (in_max_len > 0)
        {
            memcpy(f_buf_start, data, in_max_len);
            length -= in_max_len;
            data = (const char*) data + in_max_len;
            free->r_ofs += in_max_len;
        }
        this->ck_free_list_size -=free->block_size;
        // remove free in flist
        this->free_list.pop_front();
        // append free into rlist_end
        this->ready_list.push_back(free);

    }

    return length;
}

uint Chunks::ck_ceil_to_k(uint i)
{
    if (i < threshold)
    {
        return threshold;
    }

    uint k = 0x80000000;
    while (k && !(k & i))
    {
        k >>= 1;
    }
    return k << 1;
}

int Chunks::copyDataIntoNewChunk(const void* data, uint length)
{
    int block_size = ck_ceil_to_k(sizeof(Chunk) + length);

    // new a chunk
    Chunk* nc = (Chunk*) malloc(block_size);
    if (nc == NULL)
    {
        return -1;
    }
    nc->block_size = block_size - sizeof(Chunk);
    memcpy(&nc->data[0], data, length);
    nc->r_ofs = length;
    nc->l_ofs = 0;
    /// insert into busy_chunk
    this->ready_list.push_back(nc);
    this->ck_alloc_size +=  block_size;
    return 0;
}

void Chunks::reduceFreeList()
{
    if (!this->free_list.empty())
    {
        Chunk* c = *this->free_list.begin();
        this->free_list.pop_front();
        this->ck_alloc_size -= c->block_size + sizeof(Chunk);
        this->ck_free_list_size -= c->block_size;
        free(c);
    }
}

void Chunks::checkWaterLevel()
{
    while (this->c_resident_size < this->ck_alloc_size
            && !this->free_list.empty())
    {
        this->reduceFreeList();
    }
}


uint Chunks::getAllocSize() const
{
   return  ck_alloc_size;
}

Chunks::Chunks(uint max_size, uint resident_size) :
         c_resident_size(resident_size),c_max_size(max_size),threshold(1024)
{
    this->ck_alloc_size = 0;
    this->ck_free_list_size = 0;

}

Chunks::~Chunks()
{
    while (!this->ready_list.empty())
    {
        Chunk* c = *this->ready_list.begin();
        free(c);
        ready_list.pop_front();
    }

    while (!this->free_list.empty())
    {
        Chunk* c = *this->free_list.begin();
        free(c);
        free_list.pop_front();
    }
    this->ck_free_list_size = 0;
}

bool Chunks::useExistChunk(uint length)
{
    Chunk * c = NULL;
    if(!this->ready_list.empty())
    {
        c = this->ready_list.back();
        uint avilable = c->block_size - c->r_ofs ;
        if (avilable <= length){
            return true;
        }else{
            length -= avilable;
        }
    }

    if(this->ck_free_list_size >length)
    {
        return true;
    }

    return false;
}


int Chunks::copyDataIntoChunks(const void*data, uint length)
{
    if(this->ck_alloc_size > this->c_max_size && this->useExistChunk(length)){
        return length;
    }


    const char* p_in = (const char*) data;
    int ret = 0;

    if ((ret = copyDataIntoReadyList(p_in, length)) == 0)
    {
       goto DONE;
    }

    p_in += (length - ret);
    length = ret;

    if ((ret = copyDataIntoFreeList(p_in, length)) == 0)
    {
        goto DONE;
    }

    p_in += (length - ret);
    length = ret;

    if ((ret = copyDataIntoNewChunk(p_in, length)) == 0)
    {
        goto DONE;
    }
    return length;
DONE:
    return 0;

}

int Chunks::drainOutWithPipe(int (*IN_PIPE_CB)(const void*, uint, void* args),
        void* args)
{
    if (this->ready_list.empty())
    {
        return 0;
    }
    this->iter = this->ready_list.begin();

    while (this->iter != this->ready_list.end())
    {
        Chunk* cur = *iter;
        char* cur_buf = &cur->data[cur->l_ofs];
        uint cur_size = cur->r_ofs - cur->l_ofs;
        //  ret is used size
        int ret = IN_PIPE_CB(cur_buf, cur_size, args);
        if (ret <= 0)
        {
            return ret;
        }
        else if (ret == (int)cur_size)
        {
            cur->l_ofs = 0;
            cur->r_ofs = 0;
            this->ck_free_list_size +=cur->block_size;
            this->free_list.push_front(cur);
            iter++;
            this->ready_list.pop_front();
        }
        else if (ret < (int)cur_size)
        {
            cur->l_ofs += (uint)ret;
        }
    }
    this->checkWaterLevel();
    return 0;
}

void Chunks::resetChunks()
{
    while (!this->ready_list.empty())
    {
        Chunk* c = *this->ready_list.begin();
        c->l_ofs = 0;
        c->r_ofs = 0;
        this->ready_list.pop_front();
        this->ck_free_list_size +=c->block_size;
        this->free_list.push_back(c);
    }
    this->checkWaterLevel();
}

