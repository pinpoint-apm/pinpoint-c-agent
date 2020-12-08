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
/*
 * pinpoint_helper.cpp
 *
 *  Created on: Jul 3, 2019
 *      Author: eeliu
 */
#include "Chunk.h"
namespace Cache{
    
int Chunks::copyDataIntoReadyCK(const void* data, uint32_t length)
{
    if (ready_cks.empty())
    {
        return length;
    }

    Chunk* ck = ready_cks.back();

    char* p_ck_r = &ck->data[ck->r_ofs];
    uint32_t capacity = ck->block_size - ck->r_ofs;
    if (capacity >= length)
    {
        memcpy(p_ck_r, data, length);
        ck->r_ofs += length;
        length = 0;
    }
    else if (capacity > 0) // capacity < length
    { // current ck is full
        memcpy(p_ck_r, data, capacity);
        length -= capacity;
        ck->r_ofs += capacity;
    }

    return length;
}

int Chunks::copyDataIntoFreeCK(const void*data, uint32_t length)
{
    if (this->free_cks.empty())
    {
        return length;
    }


    iter = this->free_cks.begin();

    while (iter != this->free_cks.end() && length >0 )
    {
        Chunk* ck = *iter;
        iter++;
        char* f_buf_start = &ck->data[ck->r_ofs];
        uint32_t ck_capacity = ck->block_size - ck->r_ofs;
        if (ck_capacity >= length)
        {
            // current data task is done
            memcpy(f_buf_start, data, length);
            ck->r_ofs += length;
            length = 0;

        }
        else if (ck_capacity > 0) //ck_capacity < length
        {
            memcpy(f_buf_start, data, ck_capacity);
            length -= ck_capacity;
            data = (const char*) data + ck_capacity;
            ck->r_ofs += ck_capacity;
        }

        this->ck_free_ck_capacity -= ck->block_size;
        // remove free in flist
        this->free_cks.pop_front();
        // append free into rlist_end
        this->ready_cks.push_back(ck);

    }

    return length;
}

uint32_t Chunks::ck_ceil_to_k(uint32_t i)
{
    if (i < threshold)
    {
        return threshold;
    }

    uint32_t k = 0x80000000;
    while (k && !(k & i))
    {
        k >>= 1;
    }
    return k << 1;
}

int Chunks::copyDataIntoNewChunk(const void* data, uint32_t length)
{
    int mem_size = ck_ceil_to_k(sizeof(Chunk) + length);

    // new a chunk
    Chunk* ck = (Chunk*) malloc(mem_size);
    if (ck == NULL)
    {
        return -1;
    }
    ck->block_size = mem_size - sizeof(Chunk);
    memcpy(&ck->data[0], data, length);
    ck->r_ofs = length;
    ck->l_ofs = 0;
    /// insert into ready list
    this->ready_cks.push_back(ck);
    this->ck_alloc_size +=  mem_size;
    return 0;
}

void Chunks::reduceFreeCK()
{
    if (!this->free_cks.empty())
    {
        Chunk* c = *this->free_cks.begin();
        this->free_cks.pop_front();
        this->ck_alloc_size -= (c->block_size + sizeof(Chunk) );
        this->ck_free_ck_capacity -= c->block_size;
        free(c);
    }
}

void Chunks::checkWaterLevel()
{
    while (this->c_resident_size < this->ck_alloc_size
            && !this->free_cks.empty())
    {
        this->reduceFreeCK();
    }
}


uint32_t Chunks::getAllocSize() const
{
   return  this->ck_alloc_size;
}

Chunks::Chunks(uint32_t max_size, uint32_t resident_size) :
         c_resident_size(resident_size),c_max_size(max_size),threshold(1024)
{
    this->ck_alloc_size = 0;
    this->ck_free_ck_capacity = 0;
    if(max_size < resident_size)
        throw std::invalid_argument("chunks: max_size must bigger then resident_size");
}

Chunks::~Chunks()
{
    while (!this->ready_cks.empty())
    {
        Chunk* c = this->ready_cks.front();
        free(c);
        ready_cks.pop_front();
    }

    while (!this->free_cks.empty())
    {
        Chunk* c = *this->free_cks.begin();
        free(c);
        free_cks.pop_front();
    }
    this->ck_free_ck_capacity = 0;
}

bool Chunks::useExistingChunk(uint32_t length) const
{
    Chunk * c = nullptr;
    if(!this->ready_cks.empty())
    {
        c = this->ready_cks.back();
        uint32_t availiable = c->block_size - c->r_ofs ;
        if (availiable < length){
            return false;
        }else{
            length -= availiable;
        }
    }

    if(this->ck_free_ck_capacity >=length)
    {
        return true;
    }

    return false;
}


int Chunks::copyDataIntoChunks(const void*data, uint32_t length)
{
    const char* p_in = (const char*) data;
    int ret = 0;

    // fill order 
    // 1. ready ck
    // 2. free ck
    // 3. create a new ck
    if ((ret = copyDataIntoReadyCK(p_in, length)) == 0)
    {
       goto DONE;
    }

    p_in += (length - ret);
    length = ret;

    if ((ret = copyDataIntoFreeCK(p_in, length)) == 0)
    {
        goto DONE;
    }

    p_in += (length - ret);
    length = ret;

    if ((ret = copyDataIntoNewChunk(p_in, length)) == -1)
    {
        // exhausting memory
        return -1;
    }

DONE:
    return 0;
}

int Chunks::drainOutWithPipe(std::function<int(const char*, uint32_t)> in_pipe_cb)
{
    if (this->ready_cks.empty())
    {
        return 0;
    }
    this->iter = this->ready_cks.begin();

    while (this->iter != this->ready_cks.end())
    {
        Chunk* cur = *iter;
        char* cur_buf = &cur->data[cur->l_ofs];
        uint32_t cur_size = cur->r_ofs - cur->l_ofs;
        // call in_pipe_cb flush the data out
        int ret = in_pipe_cb(cur_buf, cur_size);
        if (ret <= 0)
        {
            return ret;
        }
        else if (ret == (int)cur_size) 
        { // current chunk is done, drop current chunk
            // reset read offset
            cur->l_ofs = 0;
            //reset write offset
            cur->r_ofs = 0;
            // insert into free cks
            this->ck_free_ck_capacity +=cur->block_size;
            this->free_cks.push_front(cur);
            // remove from read cks
            iter++;
            this->ready_cks.pop_front();
        }
        else if (ret < (int)cur_size)
        {   // some are failed, resend at next time
            cur->l_ofs += (uint32_t)ret;
        }
    }
    this->checkWaterLevel();
    return 0;
}

void Chunks::resetChunks()
{
    while (!this->ready_cks.empty())
    {
        Chunk* c = *this->ready_cks.begin();
        c->l_ofs = 0;
        c->r_ofs = 0;
        this->ready_cks.pop_front();
        this->ck_free_ck_capacity +=c->block_size;
        this->free_cks.push_back(c);
    }
    this->checkWaterLevel();
}

}