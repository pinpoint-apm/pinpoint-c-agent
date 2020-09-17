#include <unistd.h>
#include <errno.h>
#include<functional>
#include<iostream>
#include <string>
#include <gtest/gtest.h>

#include "ConnectionPool/SpanConnectionPool.h"
#include "json/json.h"

using namespace testing;
using ConnectionPool::TransLayer;
using ConnectionPool::SpanConnectionPool;
using ConnectionPool::TransConnection;

#define unix_socket  "./pinpoint_test.sock"
bool run = true;

void sig_exit(int signm)
{
    run = false;
}

int fack_server()
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un address = {0};
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, unix_socket, sizeof(address.sun_path) -1);
    remove(unix_socket);
    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        EXPECT_TRUE(0)<<"bind server socket failed"<<strerror(errno);
    }
    signal(SIGQUIT,sig_exit);
    listen(fd, 10);
    while (run)
    {
        char buffer[1024]={0};
        struct sockaddr addr;
        socklen_t addrlen;
        int cfd = accept(fd, &addr, &addrlen);
        if (cfd == -1)
        {
            break;
        }
        pp_trace("recv cfd:%d",cfd);
        Json::Value agentInfo;

        agentInfo["time"]=1234567;
        agentInfo["id"]="test-app";
        agentInfo["name"]="test-name";

        Json::FastWriter writer;
        std::string msg = writer.write(agentInfo);
        strcpy((char*)buffer+sizeof(Header),msg.c_str());

        int len = msg.length();
        Header* header = (Header*)buffer;
        header->type = htonl(RESPONSE_AGENT_INFO);
        header->length = htonl(len);
        int ret = send(cfd,buffer,len+sizeof(*header),0);
        pp_trace("send[%d] %s [%d] %s ",cfd,buffer,ret,strerror(errno));
        recv(cfd,buffer,1024,0);
        close(cfd);
    }
    return 0;
}

void handle_agent_info(int type,const char* buf,size_t len)
{
    printf("%s",buf);
    EXPECT_EQ(type,RESPONSE_AGENT_INFO);
    Json::Value  root;
    Json::Reader reader;
    reader.parse(buf,buf+len,root);
    EXPECT_EQ(root["time"].asInt64(),1234567);
    EXPECT_STREQ(root["id"].asCString(),"test-app");
    EXPECT_STREQ(root["name"].asCString(),"test-name");
    run = false;
}



TEST(translayer, unix_socket_layer)
{
    pid_t pid = fork();
    if(pid == 0){
        fack_server();
        exit(0);
    }
    std::string remote = global_agent_info.co_host;
    TransLayer layer(remote,10);
    using namespace std::placeholders;
    layer.registerPeerMsgCallback(std::bind(handle_agent_info,_1,_2,_3),NULL);
    while(run){
        layer.trans_layer_pool();
    }
    std::string data="msg-1918";
    layer.sendMsgToAgent(data);
    layer.forceFlushMsg(10);
    kill(pid,SIGQUIT);
    waitpid(pid,0,0);

}

TEST(translayer, stream_socket_layer)
{
    int fd = -1;
    fd = TransLayer::connect_stream_remote("www.naver.com:80");
    EXPECT_GT(fd,2);
    close(fd);

    fd = TransLayer::connect_stream_remote("172.217.175.68:80");
    EXPECT_GT(fd,2);
    close(fd);


    fd = TransLayer::connect_stream_remote("-.217.175.68:80");
    EXPECT_EQ(fd,-1);

    fd = TransLayer::connect_stream_remote("-.217.175.68:-80");
    EXPECT_EQ(fd,-1);

}

TEST(ConnectionPool, API)
{
    std::string remote = global_agent_info.co_host;
    SpanConnectionPool _pool(remote.c_str(),10);

    // SpanConnectionPool _pool1 = _pool;
    // (void)_pool1;

    TransConnection _conn = _pool.getConnection();

    EXPECT_EQ(_conn->connect_stream_remote("-.217.175.68:-80"),-1);

    _pool.giveBackConnection(_conn);

}

