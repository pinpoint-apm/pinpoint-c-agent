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
// Created by eeliu on 1/3/2020.
//

#include <cstdio>
#include <functional>
#include <iostream>
#include <map>
#include "common.h"
#include "TransLayer.h"
#include "SharedObj.h"
#include "json/json.h"


static inline uint64_t get_current_msec_stamp();

const static char* CLUSE="clues";
static TraceStoreLayer* _storelayer;

typedef struct _SharedState{
    int64_t uid;
    int64_t timestamp;
    int64_t tick;
    uint64_t starttime;
    int64_t state;
}SharedState;


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

typedef std::stack<TraceNode> Stack;
class PerThreadAgent{
public:
    PerThreadAgent(PPAgentT* agent):
    timeout_ms(agent->timeout_ms),
    trace_limit(agent->trace_limit),
    translayer(TransLayer(agent,agent->timeout_ms)),
    json_writer()
    {
        this->fetal_error_time = 0;

//        /**
//         * Max [ 0 ~ pagesize ]
//         * |  uid   | timestamp | triger  |
//         *   [0~7]      [8~15]    [16~23]
//         */
//        this->uid = (int64_t*)((char*)fetch_shared_obj_addr() + UNIQUE_ID_OFFSET);
//        this->triger_timestamp = (int64_t*)((char*)fetch_shared_obj_addr() + TRACE_LIMIT_OFFSET);
//        this->tick_tick  =  (int64_t*)((char*)fetch_shared_obj_addr() + TRIGER_OFFSET );
//        this->start_time =(uint64_t*)((char*)fetch_shared_obj_addr() + START_TIME_OFFSET );

        this->_state = (SharedState*)fetch_shared_obj_addr();
        this->limit   = (this->_state->state & E_OFFLINE) ? (E_OFFLINE) : (E_TRACE_PASS);
        this->app_name = "collector_blocking";
        this->app_id = "collector_blocking";
        json_writer.dropNullPlaceholders();
        json_writer.omitEndingLineFeed();
        using namespace std::placeholders;
        this->translayer.registerPeerMsgCallback(
                std::bind(&PerThreadAgent::_handleMsgFromCollector,this,_1,_2,_3),
                std::bind(&PerThreadAgent::_handleTransLayerState,this,_1)
        );

    }

    ~PerThreadAgent()
    {

    }

    void setKey(const char*key,const char*value)
    {
        const std::string data = value;
        const std::string tkey = key;
        std::pair<const std::string,const std::string> pair(tkey,data);
        this->special_keys.insert(pair);
    }

    const char* getKey(const char*key)
    {
        std::map<const std::string,const std::string>::const_iterator iter  =  this->special_keys.find(key);
        if(iter != this->special_keys.end())
        {
            return iter->second.c_str();
        }
        else{
            return NULL;
        }
    }

    int32_t endTrace()
    {
        if( this->stack.size() == 1 ) // ancestor node
        {
            TraceNode& ancestor =  this->stack.top();
            if(this->limit== E_TRACE_PASS || (global_agent_info.inter_flag & E_UTEST) )
            {
                uint64_t timestamp =  get_current_msec_stamp();
                ancestor.node["E"] = this->fetal_error_time != 0?( this->fetal_error_time - ancestor.start_time) : timestamp - ancestor.start_time;
                std::string trace = this->json_writer.write(ancestor.node);
                Header header;
                header.length  = htonl(trace.size());
                header.type    = htonl(REQ_UPDATE_SPAN);

                std::string buf((char*)&header,sizeof(header));
                buf += trace;

                this->translayer.sendMsgToAgent(buf);
                pp_trace("this span:(%s)",trace.c_str());
            }else if(this->limit== E_TRACE_BLOCK){
                // do nothing
            }
            else { // E_OFF
            }
            ancestor.node.clear();
            this->stack.pop();
            this->translayer.trans_layer_pool();
            // after this, special keys are dropped
            this->special_keys.clear();
        }
        else if(this->stack.size() > 1) // descendants
        {
            TraceNode& descendant =this->stack.top();
            uint64_t timestamp =  get_current_msec_stamp();
            descendant.node["E"] =  this->fetal_error_time != 0? ( this->fetal_error_time - descendant.start_time ): timestamp - descendant.start_time;
            this->stack.pop();
        }

        pp_trace("pinpoint_end_trace Done!");
        return this->stack.size();
    }
    
    int32_t startTrace()
    {
        pp_trace("pinpoint_start start");
        uint64_t timestamp =  get_current_msec_stamp();
        if(!this->stack.empty()) // child
        {
            TraceNode& parent = this->stack.top();
            uint node_length = parent.node["calls"].size();
            TraceNode child(parent.node["calls"][node_length]);
            child.node["S"] = timestamp - parent.ancestor_start_time;
            child.ancestor_start_time = parent.ancestor_start_time;
            child.start_time = timestamp;
            this->stack.push(child);
            this->fetal_error_time = 0; // reset fetal_error_time
        }else{ // ancestor
            this->translayer.trans_layer_pool();
            TraceNode ancestor(this->root);
            ancestor.node["S"] = timestamp;
            ancestor.node["FT"]= global_agent_info.agent_type;
            ancestor.ancestor_start_time = timestamp;
            ancestor.start_time = timestamp;
            this->stack.push(ancestor);
        }
        return this->stack.size();
    }

    inline uint getStackSize()
    {
        return this->stack.size();
    }

    inline void forceFlushTrace(uint32_t timeout)
    {
        this->translayer.forceFlushMsg(timeout);
    }

    void AddClue(const char* key,const char* value)
    {

        if(this->stack.size()> 0)
        {
            Json::Value& curNode = this->stack.top().node;
            curNode[key] = value;
            pp_trace("key:%s value:%s",key,value);
        }else{
            pp_trace("%d: stack is empty,error found.",__LINE__);
        }
    }

    void AddClues(const char* key,const char* value)
    {

        if(this->stack.size()> 0)
        {
            Json::Value& curNode = this->stack.top().node;
            std::string cvalue ="";
            cvalue+=key;
            cvalue+=':';
            cvalue+=value;
            curNode[CLUSE].append(cvalue);
            pp_trace("add clues:%s:%s",key,value);
        }else{
           pp_trace("%d: stack is empty,error found.",__LINE__);
        }
    }

    bool checkTraceLimit(int64_t timestamp)
    {
        time_t ts = (timestamp != -1) ?(timestamp) :(time(NULL));
        
        // disable this checking, make it working even offline
        // if(this->limit == E_OFFLINE)
        // {
        //     pp_trace("collector-agent not available");
        //     goto OFFLINE;
        // }

        if(this->trace_limit < 0)
        {
            // let it pass
        }else if(this->trace_limit == 0)
        {
            goto BLOCK;
        }
        else if( this->_state->timestamp != ts )
        {
            this->_state->timestamp = ts;
            this->_state->tick = 0 ;
            __sync_synchronize();
        }
        else if(this->_state->tick >= this->trace_limit)
        {
            goto BLOCK;
        }else
        {
            __sync_add_and_fetch(&this->_state->tick,1);
        }
        this->limit = E_TRACE_PASS;
        return false;
BLOCK:
        pp_trace("This span dropped. trace_limit:%d limit:%d tick:%d",this->trace_limit,this->limit,this->_state->tick);
// OFFLINE:
        this->limit = E_TRACE_BLOCK;
        return true;

    }

    void catchFetalError(const char* msg,const char* error_filename,uint error_lineno)
    {
        this->fetal_error_time = get_current_msec_stamp();
        Json::Value eMsg;
        eMsg["msg"] = msg;
        eMsg["file"]  = error_filename;
        eMsg["line"]  = error_lineno;
        this->root["ERR"] = eMsg;
    }

    inline void setLimit(E_ANGET_STATUS status )
    {
        pp_trace("agent status changed: %d ->%d",this->limit ,status);
        this->limit = status;
    }

    uint64_t generateUniqueId()
    {
        return __sync_fetch_and_add(&this->_state->uid,1);
    }

    inline uint64_t getStartTime() const
    {
        return this->_state->starttime;
    }

    inline const std::string& getAppId() const
    {
        return this->app_id;
    }

    inline const std::string& getAppName() const
    {
        return this->app_name;
    }

private:

    void _handleTransLayerState(int state)
    {
        if(state == E_OFFLINE)
        {
            this->limit = E_OFFLINE;
            this->_state->state |= E_OFFLINE;
        }
    }


    void _handleMsgFromCollector(int type,const char* buf,size_t len)
    {
        switch (type)
        {

        case RESPONSE_AGENT_INFO:
            this->_handle_agent_info(type,buf,len);
            break;
        
        default:
            break;
        }
    
    }

    void _handle_agent_info(int type,const char* buf,size_t len)
    {
        Json::Value  root;
        Json::Reader reader;
        int ret = reader.parse(buf,buf+len,root);
        if(!ret)
        {
            return ;
        }

        if(root.isMember("time")){
            this->_state->starttime= atoll(root["time"].asCString());
        }

        if(root.isMember("id")){
           this->app_id      =  root["id"].asString();
        }

       if(root.isMember("name")){
           this->app_name    = root["name"].asString();
       }

        this->limit= E_TRACE_PASS;
        this->_state->state |= E_TRACE_PASS;
        pp_trace("starttime:%ld appid:%s appname:%s",this->_state->starttime,this->app_id.c_str(),this->app_name.c_str());
    }



private:
    // const char**co_host; // tcp:ip:port should support dns
    uint  timeout_ms;
    uint64_t  limit;
    const int   trace_limit;
    uint64_t   fetal_error_time;
    SharedState* _state;

    char log_buf[LOG_SIZE]={0};
    std::string app_name;
    std::string app_id;
    Json::Value root;
    TransLayer translayer;
    Stack stack;
    Json::FastWriter json_writer;
    std::map<const std::string,const std::string> special_keys;
};


#ifdef __linux__
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_mutex_t  thread_agent_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
static void _init_common_shared() __attribute__((constructor));
static void _free_common_shared() __attribute__((destructor));
static void thread_init(void);

static TraceStoreLayer posix_store_layer;

static std::stack<PerThreadAgent*> agentPool;
static inline void lock_agent_pool()
{
    pthread_mutex_lock(&thread_agent_pool_mutex);
}

static inline void unlock_agent_pool()
{
    pthread_mutex_unlock(&thread_agent_pool_mutex);
}

static inline void* poxic_get_cur_trace()
{
    return pthread_getspecific(key);
}

static inline void poxic_set_cur_trace(void* agent)
{
   pthread_setspecific(key,agent);
}

static void _get_read_lock()
{
    pthread_rwlock_rdlock(&rwlock);
}

static void _release_lock()
{
    pthread_rwlock_unlock(&rwlock);
}

static void _get_write_lock()
{
    pthread_rwlock_wrlock(&rwlock);
}


void _init_common_shared()
{
    // initialize thread key
    pthread_once(&init_done,thread_init);
    // initialize shm object
    if(!checking_and_init())
    {
        fprintf(stderr,"[pinpoint] initialize pinpoint module failed!");
    }

    pthread_rwlock_init(&rwlock,NULL);
    // initialize bind global
    global_agent_info.get_read_lock  = _get_read_lock;
    global_agent_info.get_write_lock = _get_write_lock;
    global_agent_info.release_lock   = _release_lock;

    // initialize default storage

    posix_store_layer.get_cur_trace_cb = poxic_get_cur_trace;
    posix_store_layer.set_cur_trace_cb = poxic_set_cur_trace;
    _storelayer = &posix_store_layer;
}

void _free_common_shared()
{
    detach_shared_obj();
    pthread_rwlock_destroy(&rwlock);

    /// reset store layer
    _storelayer  = NULL;
}


void thread_init(void)
{
   pthread_key_create(&key, give_back_agent);
}

void* create_or_reuse_agent(void)
{
    PerThreadAgent * agent = NULL;
    lock_agent_pool();
    if(agentPool.empty())
    {
        unlock_agent_pool();
        try{
            agent = new PerThreadAgent(&global_agent_info);
        }catch(...){
            return NULL;
        }
    }else
    {
        agent =agentPool.top();
        agentPool.pop();
        unlock_agent_pool();
        if(agent == NULL)
        {
            pp_trace("Found an error:%s:%d",__FILE__,__LINE__);
            return NULL;
        }
    }

    return agent;
}


static PerThreadAgent* get_agent()
{
    void* spec = _storelayer->get_cur_trace_cb(); //pthread_getspecific(key);
    if( unlikely(spec == NULL) )
    {
        // create a new agent or fetch one from poll
        PerThreadAgent* agent = static_cast<PerThreadAgent*>(create_or_reuse_agent());
        _storelayer->set_cur_trace_cb(agent); //  pthread_setspecific(key,agent);
        return agent;
    }else
    {
        return static_cast<PerThreadAgent*>(spec);
    }
}

void give_back_agent(void *agent)
{
    if(agent){
        lock_agent_pool();
        agentPool.push(static_cast<PerThreadAgent*>(agent));
        unlock_agent_pool();
    }
}

#elif _WIN32
#else
#error "not support"
#endif

int32_t pinpoint_start_trace()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return 0;
    }
    return p_agent->startTrace();
}


int32_t pinpoint_end_trace()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return 0;
    }
    return p_agent->endTrace();
}

inline uint64_t get_current_msec_stamp()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec*1000 + tv.tv_usec /1000;
}

void pinpoint_add_clue(const  char* key,const  char* value)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return ;
    }
    return p_agent->AddClue(key,value);
}

void pinpoint_add_clues(const  char* key, const char* value)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return ;
    }
    return p_agent->AddClues(key,value);
}

/**
 * True: drop this trace as time_limit
 * False: do nothing
 */
bool check_tracelimit(int64_t timestamp)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return false;
    }

    return p_agent->checkTraceLimit(timestamp);
}

int64_t generate_unique_id()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return 0;
    }
    return (int64_t)p_agent->generateUniqueId();
}

void catch_error(const char* msg,const char* error_filename,uint error_lineno)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return ;
    }
    pp_trace("catch an Error");
    p_agent->catchFetalError(msg,error_filename,error_lineno);
}

void pinpoint_drop_trace()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return ;
    }
    p_agent->setLimit(E_TRACE_BLOCK); 
}

uint64_t pinpoint_start_time()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return 0;
    }
    return p_agent->getStartTime();
}

const char* pinpoint_app_id()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return "agent-not-found";
    }
    return p_agent->getAppId().c_str();
}

const char* pinpoint_app_name()
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return "agent-not-found";
    }
    return p_agent->getAppName().c_str();
}


void pinpoint_set_special_key(const char* key,const char* value)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return ;
    }
    p_agent->setKey(key,value);
}

const char* pinpoint_get_special_key(const char* key)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return NULL;
    }
    return p_agent->getKey(key);
}

void pinpoint_force_flush_span(uint32_t timeout)
{
    PerThreadAgent* p_agent = get_agent();
    if(p_agent == NULL)
    {
        return ;
    }

    return p_agent->forceFlushTrace(timeout);
}

void pinpoint_reset_store_layer(TraceStoreLayer* storeLayer)
{
    _storelayer = storeLayer;
}


