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

class Chunks
{

    typedef struct chunk_
    {
        uint block_size;
        uint l_ofs; // The continuous buffer left offset in data ***(l_ofs)--------------(r_ofs)*****
        uint r_ofs; // The continuous buffer right offset in data
        char data[0];
    } Chunk;

    typedef std::list<Chunk*> CKList;
    typedef CKList::iterator CkIter;

private:

    CkIter iter;
    CKList ready_list;  //ready list,start
    CKList free_list;  //free_list start

    const uint c_resident_size;
    const uint c_max_size;    // if  ck_alloc_size > c_max_size buffer is full
    uint threshold;

    uint ck_alloc_size;
    uint ck_free_list_size;

private:

    uint ck_ceil_to_k(uint i);
    /**
     *
     * @param data
     * @param length
     * @return rest size of data
     */
    int copyDataIntoReadyList(const void* data, uint length);

    int copyDataIntoFreeList(const void*data, uint length);

    int copyDataIntoNewChunk(const void* data, uint length);

    void reduceFreeList();

    void checkWaterLevel();

    bool useExistChunk(uint length);

public:

    uint getAllocSize() const;

    Chunks(uint max_size, uint resident_size);

    virtual ~Chunks();
    /**
     * 0 means true
     * length failed
     * */
    int copyDataIntoChunks(const void*data, uint length);
    
    int drainOutWithPipe(std::function<int(const char*, uint)> in_pipe_cb);

    void resetChunks();

};

#endif