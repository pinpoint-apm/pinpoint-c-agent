#include <gtest/gtest.h>
#include "Cache/Chunk.h"
#include "json/json.h"
using namespace testing;
using Cache::Chunks;

static int checkData(const void* buf,uint length,void* dst)
{
    int ret = 0;
    if(length < 25)
    {
        ret =  length;
    }else{
        ret = length /2;
    }
    std::string * str = (std::string*)dst;
    str->append((const char*)buf,ret);

    return ret;
}


TEST(chunk, all_in_one)
{
    Chunks chunks(1024*4,128);
    std::string out,ret;
//    int size = 0;
    // user random data
    char buf1[1024];
    char buf2[1024];
    char buf3[1024];

    chunks.copyDataIntoChunks(buf1,1024);
    chunks.copyDataIntoChunks(buf2,1024);
    chunks.copyDataIntoChunks(buf3,1024);
    out.append(buf1,1024);
    out.append(buf2,1024);
    out.append(buf3,1024);

    // in and out
    chunks.drainOutWithPipe(std::bind(&checkData,std::placeholders::_1,std::placeholders::_2,&ret));

    EXPECT_TRUE(ret == out);

    // copy until full
    chunks.copyDataIntoChunks(buf1,1024);
    chunks.copyDataIntoChunks(buf2,1024);
    chunks.copyDataIntoChunks(buf3,1024);
    chunks.copyDataIntoChunks(buf3,1024);
    EXPECT_TRUE(chunks.getAllocSize() >= 1024*4);
    chunks.copyDataIntoChunks(buf3,1024);
    EXPECT_TRUE(chunks.copyDataIntoChunks(buf3,1024) == 1024 );

    out.clear();
    out.append(buf1,1024);
    out.append(buf2,1024);
    out.append(buf3,1024);
    out.append(buf3,1024);

    ret.clear();
    chunks.drainOutWithPipe(std::bind(&checkData,std::placeholders::_1,std::placeholders::_2,&ret));
    EXPECT_TRUE(chunks.getAllocSize() == 0);
    EXPECT_TRUE(ret == out);
}
