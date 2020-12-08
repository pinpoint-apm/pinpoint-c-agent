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
 * pinpoint_chunks.cpp
 *
 *  Created on: Jul 2, 2019
 *      Author: eeliu
 */
#ifndef COMMON_SRC_CHUNK_H
#define COMMON_SRC_CHUNK_H
#include <stdlib.h>
#include <list>
#include <string.h>
#include <json/json.h>
#include <assert.h>
#include <functional>
#include <iostream>

namespace Cache{
    
/// Readme:
/// 1. not thread safe
/// 2. check the capacity before copy data  
class Chunks
{

    typedef struct chunk_
    {
        uint32_t block_size;
        /**
         * @brief 
         * The continuous buffer left offset in data ***(l_ofs)--------------(r_ofs)*****
         *   for read data from ck
         */
        uint32_t l_ofs; 

        /**
         * @brief  The continuous buffer right offset in data
         * for write data into ck
         */
        uint32_t r_ofs; 
        char data[0];
    } Chunk;

    typedef std::list<Chunk*> CKList;
    typedef CKList::iterator CkIter;

private:

    CkIter iter;
    CKList ready_cks;  //Wait to send
    CKList free_cks;  //already send, can be reuse

    const uint32_t c_resident_size;
    const uint32_t c_max_size;    // if  ck_alloc_size > c_max_size buffer is full
    uint32_t threshold;

    uint32_t ck_alloc_size;
    uint32_t ck_free_ck_capacity;

private:

    uint32_t ck_ceil_to_k(uint32_t i);
    /**
     *
     * @param data
     * @param length
     * @return rest size of data
     */
    int copyDataIntoReadyCK(const void* data, uint32_t length);

    int copyDataIntoFreeCK(const void*data, uint32_t length);

    int copyDataIntoNewChunk(const void* data, uint32_t length);

    void reduceFreeCK();

    void checkWaterLevel();
    // try to use the ready and free list
    bool useExistingChunk(uint32_t length) const;

public:

    uint32_t getAllocSize() const;

    Chunks(uint32_t max_size, uint32_t resident_size);

    virtual ~Chunks();

    int copyDataIntoChunks(const void*data, uint32_t length);

    /**
     * If you want use the chunk safety, check capacity before use it.
     * @param length
     * @return true: chunk can hold length data
     *              false: length is too large according to max_size
     */
    inline bool checkCapacity(uint32_t length) const 
    {
        // exist chunk is full and the thredhold is reached
        if( this->useExistingChunk(length) == false && this->ck_alloc_size+ length > this->c_max_size  ){
           return false;
        }    
        return true;
    }

    int drainOutWithPipe(std::function<int(const char*, uint32_t)> in_pipe_cb);

    void resetChunks();

};
}
#endif
