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
#include <Python.h>
#include <string.h>
#include "pinpoint_py.h"


PPAgentT global_agent_info;
static PyObject *py_obj_msg_callback;
static char* g_collector_host;
// for pinpoint_coro_local
static TraceStoreLayer _coro_storage;
static PyObject* coro_local;
#define CORO_LOCAL_NAME "pinpoint_coro_local"

#ifndef CYTHON_UNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define CYTHON_UNUSED __attribute__ ((__unused__))
#   else
#     define CYTHON_UNUSED
#   endif
# elif defined(__ICC) || (defined(__INTEL_COMPILER) && !defined(_MSC_VER))
#   define CYTHON_UNUSED __attribute__ ((__unused__))
# else
#   define CYTHON_UNUSED
# endif
#endif


/**
 * void pinpoint_add_clues(const  char* key,const  char* value);
 * */
static PyObject *py_pinpoint_add_clues(PyObject *self, PyObject *args) 
{
    char* key = NULL;
    char* value = NULL;
    if(PyArg_ParseTuple(args,"ss",&key,&value))
    {
        pinpoint_add_clues(key,value);
    }
    return Py_BuildValue("O",Py_True);
}

/**
 * void pinpoint_add_clue(const  char* key,const  char* value);
*/
static PyObject *py_pinpoint_add_clue(PyObject *self, PyObject *args)
{
    char* key = NULL;
    char* value = NULL;
    if(PyArg_ParseTuple(args,"ss",&key,&value))
    {
       pinpoint_add_clue(key,value);
    }
    return Py_BuildValue("O",Py_True);
}


/**
 * void pinpoint_set_special_key(const  char* key,const  char* value);
*/
static PyObject *py_pinpoint_set_key(PyObject *self, PyObject *args)
{
    char* key = NULL;
    char* value = NULL;
    if(PyArg_ParseTuple(args,"ss",&key,&value))
    {
       pinpoint_set_special_key(key,value);
    }
    return Py_BuildValue("O",Py_True);
}


/**
 * const  char* pinpoint_get_special_key(const  char* key);
*/
static PyObject *py_pinpoint_get_key(PyObject *self, PyObject *args)
{
    char* key = NULL;
    if(PyArg_ParseTuple(args,"s",&key))
    {
        const char* data = pinpoint_get_special_key(key);
        return Py_BuildValue("s",data);
    }
    else
    {
        return Py_BuildValue("O",Py_False);
    }
}

/**
 * bool check_tracelimit(int64_t timestamp);
*/
static PyObject *py_check_tracelimit(PyObject *self, PyObject *args)
{
    int64_t timestamp= -1;
    bool ret;
    if(! PyArg_ParseTuple(args,"|l",&timestamp))
    {
        return NULL;
    }
    ret = check_tracelimit(timestamp);
    if(ret == true){
         return Py_BuildValue("O",Py_True);
    }else{
        return Py_BuildValue("O",Py_False);
    }

}


static PyObject *py_force_flush_span(PyObject *self, PyObject *args)
{
    int32_t timeout= 3;

    if(! PyArg_ParseTuple(args,"|i",&timeout))
    {
        return NULL;
    }
    if(global_agent_info.inter_flag & E_DISABLE_GIL)
    {
        pinpoint_force_flush_span(timeout);
    }
    else
    {
        Py_BEGIN_ALLOW_THREADS
        pinpoint_force_flush_span(timeout);
        Py_END_ALLOW_THREADS
    }

    return Py_BuildValue("O",Py_True);
}



static PyObject *py_pinpoint_start_trace(PyObject *self,CYTHON_UNUSED  PyObject *unused)
{
    int ret = 0;
    if(global_agent_info.inter_flag & E_DISABLE_GIL )
    {
        ret = pinpoint_start_trace();
    }else{
        Py_BEGIN_ALLOW_THREADS
        ret = pinpoint_start_trace();
        Py_END_ALLOW_THREADS
    }

    return Py_BuildValue("i", ret);
}


static PyObject *py_pinpoint_end_trace(PyObject *self, CYTHON_UNUSED PyObject *unused)
{
    int ret = 0;
    if(global_agent_info.inter_flag & E_DISABLE_GIL)
    {
        ret = pinpoint_end_trace();
    }else
    {
        Py_BEGIN_ALLOW_THREADS
        ret = pinpoint_end_trace();
        Py_END_ALLOW_THREADS
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_generate_unique_id(PyObject *self, CYTHON_UNUSED PyObject *unused)
{
    uint64_t ret = generate_unique_id();
    return Py_BuildValue("l", ret);
}

static PyObject *py_pinpoint_drop_trace(PyObject *self, CYTHON_UNUSED PyObject *unused)
{
    pinpoint_drop_trace();
    return Py_BuildValue("O",Py_True);
}

// static PyObject *py_pinpoint_app_id(PyObject *self, CYTHON_UNUSED PyObject *unused)
// {
//     const char* app_id = pinpoint_app_id();

//     return Py_BuildValue("s",app_id);
// }

// static PyObject *py_pinpoint_app_name(PyObject *self, CYTHON_UNUSED PyObject *unused)
// {
//     const char* app_name = pinpoint_app_name();

//     return Py_BuildValue("s",app_name);
// }


static PyObject *py_pinpoint_start_time(PyObject *self, CYTHON_UNUSED PyObject *unused)
{
    uint64_t start_time = pinpoint_start_time();
    return Py_BuildValue("l",start_time);
}

//origin from https://docs.python.org/3/extending/extending.html#calling-python-functions-from-c

static void msg_log_error_cb(char* msg)
{
    if(py_obj_msg_callback && msg)
    {
        PyObject *arglist;
        PyObject *result;
        arglist = Py_BuildValue("(s)", msg);
        result = PyObject_CallObject(py_obj_msg_callback, arglist);
        if(result == NULL)
        {
            fprintf(stderr,"%s",msg);
            PyErr_SetString(PyExc_TypeError, msg);
            abort();
            return ;
        }
        Py_XDECREF(result); // I don't care return
        Py_DECREF(arglist);
    }
}


static PyObject *py_pinpoint_enable_utest(PyObject *self, PyObject *args)
{
    global_agent_info.inter_flag |=E_LOGGING ;
    global_agent_info.inter_flag |=E_DISABLE_GIL ;

    PyObject *temp;
    if (PyArg_ParseTuple(args, "O:callback", &temp)) 
    {
        if (PyCallable_Check(temp)) 
        {
            Py_XINCREF(temp);
            Py_XDECREF(py_obj_msg_callback);
            py_obj_msg_callback = temp;  
            register_error_cb(msg_log_error_cb);
        }
    }
// disable logging callback
    enable_trace_utest();

    return Py_BuildValue("O",Py_True);
}

bool set_collector_host(char* host)
{
    if(strcasestr(host,"unix") || strcasestr(host,"tcp"))
    {
        if(g_collector_host)
        {
            free(g_collector_host);
            g_collector_host = NULL;
        }

        g_collector_host = strdup(host);
        
        // NOTE: co_host must be protected when writting
        
        global_agent_info.co_host = g_collector_host;

        return true;
    }
    PyErr_SetString(PyExc_TypeError, "collector_host must start with unix/tcp");
    return false;
}


static PyObject *py_set_agent(PyObject *self, PyObject *args, PyObject *keywds)
{
    // PyObject* setting;
    bool ret = false;
    static char *kwlist[] = {"collector_host", "trace_limit","enable_coroutines", NULL};
    char* collector_host = "unix:/tmp/collector-agent.sock";
    long trace_limit = -1;
    int enable_coro = 0;
    if(PyArg_ParseTupleAndKeywords(args,keywds,"s|lp",kwlist,&collector_host, &trace_limit,&enable_coro))
    {

        global_agent_info.get_write_lock();
        ret = set_collector_host(collector_host);
        if( !ret )
        {
            goto END_OF_PARSE;
        }

        global_agent_info.trace_limit = trace_limit;

        if(enable_coro)
        {
            // todo must hold GIL
            pinpoint_reset_store_layer(get_coro_store_layer());
            global_agent_info.inter_flag |= E_DISABLE_GIL ;
        }


        pp_trace("collector_host:%s",collector_host);
        pp_trace("trace_limit:%ld",trace_limit);
        pp_trace("enable_coro:%d",enable_coro);



END_OF_PARSE:
        global_agent_info.release_lock();

        if( ret == true ){
            return Py_BuildValue("O",Py_True);
        }
        return NULL;

//         PyObject* py_collector_host = Py_BuildValue("s","collector_host");
//         PyObject* py_trace_limit = Py_BuildValue("s","trace_limit");
//         global_agent_info.get_write_lock();

//         if(PyDict_Contains(setting,py_collector_host) == 1)
//         {
//             PyObject* py_host = PyDict_GetItem(setting,py_collector_host);
//             if(PyBytes_Check(py_host))
//             {
//                 char* host = strdup(PyBytes_AsString(py_host));
//                 ret = set_collector_host(host);
//                 free(host);
//                 if( !ret )
//                 {
//                     goto END_OF_PARSE;
//                 }
//             }else{
//                 PyErr_SetString(PyExc_TypeError, "collector_host must a string");
//                 goto END_OF_PARSE;
//             }
//         }

//         if(PyDict_Contains(setting,py_trace_limit) == 1)
//         {
//             PyObject* py_limit = PyDict_GetItem(setting,py_trace_limit);
//             if(!PyLong_Check(py_limit))
//             {
//                 PyErr_SetString(PyExc_TypeError, "trace_limit must a long");
//                 goto END_OF_PARSE;
//             }
//             global_agent_info.trace_limit = PyLong_AsLong(py_limit);
//         }

//  END_OF_PARSE:
//         global_agent_info.release_lock();
//         Py_DECREF(py_collector_host);
//         Py_DECREF(py_trace_limit);
//         if( ret == true ){
//              return Py_BuildValue("O",Py_True);
//         }
//         return NULL;
    }
    else
    {
        return NULL;
    }
}


static void free_pinpoint_module(void * module)
{
    Py_XDECREF(py_obj_msg_callback);
    if (g_collector_host)
    {
        free(g_collector_host);
    }

    if(coro_local)
    {
        Py_DECREF(coro_local);
    }

    if(py_obj_msg_callback)
    {
        Py_DECREF(py_obj_msg_callback);
    }
}

static PyObject *py_pinpoint_mark_an_error(PyObject *self, PyObject *args)
{
    char * msg;
    char * file_name;
    uint line_no;
    if(PyArg_ParseTuple(args,"ssl",&msg,&file_name,&line_no))
    {
        catch_error(msg,file_name,line_no);
    }

    return Py_BuildValue("O",Py_True);
}

static PyObject *Agent_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyAgentObj *self;
    self = (PyAgentObj*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->agent = NULL;
    }
    pp_trace("Agent_new %p",self);
    return (PyObject*)self;
}

static void Agent_dealloc(PyAgentObj *self) {
    pp_trace("Agent_dealloc %p",self);
    give_back_agent(self->agent);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

// static int set_raw(KeyObject *self, PyObject *value, void *closure) 
// {
//     if (value == NULL) {
//         PyErr_SetString(PyExc_TypeError, "Cannot delete 'x'");
//         return -1;
//     }
// }

// static PyObject *get_raw(KeyObject *self, void *closure) 
// {

// }

// static PyGetSetDef Agent_getsets[] = {
//     {"raw", (getter)get_raw, (setter)set_raw, "raw", NULL},
//     {NULL}  // Sentinel
// };



/* Module method table */
static PyMethodDef PinpointMethods[] = {
    {"start_trace", py_pinpoint_start_trace, METH_NOARGS, "def start_trace():# create a new trace and insert into trace chain"},
    {"end_trace", py_pinpoint_end_trace, METH_NOARGS, "def end_trace():# end currently matched trace"},
    {"unique_id", py_generate_unique_id, METH_NOARGS, "def unique_id()-> long"},
    {"drop_trace", py_pinpoint_drop_trace, METH_NOARGS, "def drop_trace():# drop this trace"},
    // {"app_id", py_pinpoint_app_id, METH_NOARGS, "def app_id()->string"},
    // {"app_name", py_pinpoint_app_name, METH_NOARGS, "def app_name()->string"},
    {"start_time", py_pinpoint_start_time, METH_NOARGS, "def start_time()->long"},
    {"add_clues", py_pinpoint_add_clues, METH_VARARGS, "def add_clues(string key,string value)"},
    {"add_clue", py_pinpoint_add_clue, METH_VARARGS, "def add_clue(string key,string value)"},
    {"set_special_key", py_pinpoint_set_key, METH_VARARGS, "def set_special_key(string key,string value): # create a key-value pair that bases on current trace chain"},
    {"get_special_key", py_pinpoint_get_key, METH_VARARGS, "def get_special_key(key)->string "},
    {"check_tracelimit", py_check_tracelimit, METH_VARARGS, "check_tracelimit(long timestamp): check trace whether is limit"},
    {"enable_debug", py_pinpoint_enable_utest, METH_VARARGS, "enable logging output(callback )"},
    {"force_flush_trace", py_force_flush_span, METH_VARARGS, "force flush span during timeout"},
    {"mark_as_error",py_pinpoint_mark_an_error,METH_VARARGS,"def mark_as_error(string msg,string file_name,uint line_no) #This trace found an error"},
    {"set_agent",(PyCFunction)py_set_agent, METH_VARARGS|METH_KEYWORDS, "def set_agent(collector_host=\"unix:/tmp/collector-agent.sock or tcp:host:port\",trace_limit=100,enable_coroutines=False)"},
    { NULL, NULL, 0, NULL}
};
/* Module structure */
static struct PyModuleDef pinpointPymodule = {
    PyModuleDef_HEAD_INIT,
    "pinpointPy",           /* name of module */
    "python agent for pinpoint platform",  /* Doc string (may be NULL) */
    -1,                 /* Size of per-interpreter state or -1 */
    PinpointMethods,       /* Method table */
    NULL,
    NULL,
    NULL,
    free_pinpoint_module /* free global variables*/
};

static PyTypeObject Agent_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pinpointPy."SubObjName, /* tp_name */
    sizeof(PyAgentObj),         /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Agent_dealloc,   /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    SubObjName" objects",  /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,                         /* tp_methods */
    0,                         /* tp_members */
    0,               /* tp_getset Agent_getsets */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    Agent_new,                   /* tp_new */
};



/* Module initialization function */
PyMODINIT_FUNC
PyInit_pinpointPy(void) {
    
    global_agent_info.agent_type=1700;
    global_agent_info.co_host = "unix:/tmp/collector.sock";
    global_agent_info.inter_flag = 0;
    global_agent_info.timeout_ms = 0;
    global_agent_info.trace_limit = -1;
    register_error_cb(NULL);

    // register Agent_Type

    if (PyType_Ready(&Agent_Type) < 0)
    {
        return NULL;
    }

    PyObject* m = PyModule_Create(&pinpointPymodule);
     if (m == NULL)
        return NULL;
    Py_INCREF(&Agent_Type);
    if (PyModule_AddObject(m, SubObjName, (PyObject*)&Agent_Type) <0)
    {
        Py_DECREF(&Agent_Type);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}

static void set_coro_local(void* agent)
{
    PyObject *pp_agent_obj = PyObject_CallObject((PyObject *) &Agent_Type,NULL);
    assert(pp_agent_obj);
    ((PyAgentObj*)pp_agent_obj)->agent = agent;
    pp_trace("new  agent obj:%p",pp_agent_obj);
    PyObject *tok = PyContextVar_Set(coro_local, pp_agent_obj);
    if(tok == NULL ){
        Py_DECREF(pp_agent_obj);
        return;
    }
    Py_DECREF(tok);
    Py_DECREF(pp_agent_obj);
    pp_trace("set agent:%p success",agent);
}

static void* get_coro_local(void)
{
    PyObject *pp_agent_obj;
    if(PyContextVar_Get(coro_local,NULL,&pp_agent_obj)<0){
        pp_trace("get coro local failed");
        return NULL;
    }

    if (pp_agent_obj != NULL) {
        pp_trace("get_coro_local:%p",((PyAgentObj*)pp_agent_obj)->agent);
        Py_DECREF(pp_agent_obj);
        return ((PyAgentObj*)pp_agent_obj)->agent;
    }
    pp_trace("why agent is NULL");
    return NULL;
}

TraceStoreLayer* get_coro_store_layer(void)
{

    if(coro_local == NULL)
    {
        coro_local = PyContextVar_New(CORO_LOCAL_NAME, NULL);
    }

    _coro_storage.get_cur_trace_cb = get_coro_local;
    _coro_storage.set_cur_trace_cb = set_coro_local;

    return &_coro_storage;
}
