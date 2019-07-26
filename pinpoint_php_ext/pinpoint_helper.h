/*
 * pinpoint_chunks.cpp
 *
 *  Created on: Jul 2, 2019
 *      Author: eeliu
 */

#include <stdlib.h>
#include <list>
#include <string.h>
#include <json/json.h>
#include <assert.h>

class TraceNode
{
public:
    TraceNode(Json::Value& node):
        node(node)
    {
        ancestor_start_time = start_time = 0;
    }
    Json::Value &node;
    uint64_t ancestor_start_time;
    uint64_t start_time;
};


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

    int copyDataIntoChunks(const void*data, uint length);

    int drainOutWithPipe(int (*IN_PIPE_CB)(const void*, uint, void* args),
            void* args);

    void resetChunks();

};

