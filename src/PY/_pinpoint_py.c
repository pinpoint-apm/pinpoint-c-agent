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
#include "_pinpoint_py.h"

static PyObject *py_obj_msg_callback;
// global g_collector_host is suck
// static char* g_collector_host;

#if defined(__linux__) || defined(_UNIX) || defined(__APPLE__)
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
static void _init_common_shared(void) __attribute__((constructor));
static void _free_common_shared(void) __attribute__((destructor));
static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

static void _get_read_lock(void)
{
    pthread_rwlock_rdlock(&rwlock);
}

static void _release_lock(void)
{
    pthread_rwlock_unlock(&rwlock);
}

static void _get_write_lock(void)
{
    pthread_rwlock_wrlock(&rwlock);
}

void _init_common_shared(void)
{
    pthread_rwlock_init(&rwlock, NULL);
    global_agent_info.get_read_lock = _get_read_lock;
    global_agent_info.get_write_lock = _get_write_lock;
    global_agent_info.release_lock = _release_lock;
}

void _free_common_shared(void)
{
    pthread_rwlock_destroy(&rwlock);
}
#else

#error "your platform not support"

#endif

#define MODNAME "_pinpointPy"

#ifndef CYTHON_UNUSED
#if defined(__GNUC__)
#if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#define CYTHON_UNUSED __attribute__((__unused__))
#else
#define CYTHON_UNUSED
#endif
#elif defined(__ICC) || (defined(__INTEL_COMPILER) && !defined(_MSC_VER))
#define CYTHON_UNUSED __attribute__((__unused__))
#else
#define CYTHON_UNUSED
#endif
#endif

/**
 * void pinpoint_add_clues(const  char* key,const  char* value);
 * */
static PyObject *py_pinpoint_add_clues(PyObject *self, PyObject *args)
{
    char *key = NULL;
    char *value = NULL;
    int id = -1;
    int loc = 0;
    if (PyArg_ParseTuple(args, "ss|ii", &key, &value, &id, &loc))
    {
        // python is special, use thread-id
        if (id == -1)
            id = pinpoint_get_per_thread_id();

        pinpoint_add_clues(id, key, value, loc);
    }
    return Py_BuildValue("O", Py_True);
}

/**
 * void pinpoint_add_clue(const  char* key,const  char* value);
 */
static PyObject *py_pinpoint_add_clue(PyObject *self, PyObject *args)
{
    char *key = NULL;
    char *value = NULL;
    int id = -1;
    int loc = 0;
    if (PyArg_ParseTuple(args, "ss|ii", &key, &value, &id, &loc))
    {
        if (id == -1)
            id = pinpoint_get_per_thread_id();

        pinpoint_add_clue(id, key, value, loc);
    }
    return Py_BuildValue("O", Py_True);
}

/**
 * void pinpoint_set_special_key(const  char* key,const  char* value);
 */
static PyObject *py_pinpoint_context_key(PyObject *self, PyObject *args)
{
    char *key = NULL;
    char *value = NULL;
    int id = -1;
    if (PyArg_ParseTuple(args, "ss|i", &key, &value, &id))
    {
        if (id == -1)
            id = pinpoint_get_per_thread_id();

        pinpoint_set_context_key(id, key, value);
    }
    return Py_BuildValue("O", Py_True);
}

/**
 * const  char* pinpoint_get_special_key(const  char* key);
 */
static PyObject *py_pinpoint_get_key(PyObject *self, PyObject *args)
{
    char *key = NULL;
    int id = -1;
    if (PyArg_ParseTuple(args, "s|i", &key, &id))
    {
        if (id == -1)
            id = pinpoint_get_per_thread_id();
        char buf[1024] = {0};
        if (pinpoint_get_context_key(id, key, buf, sizeof(buf)) > 0)
        {
            return Py_BuildValue("s", buf);
        }
        else
        {
            return Py_BuildValue("O", Py_False);
        }
    }
    else
    {
        return Py_BuildValue("O", Py_False);
    }
}

/**
 * bool check_tracelimit(int64_t timestamp);
 */
static PyObject *py_check_tracelimit(PyObject *self, PyObject *args)
{
    int64_t timestamp = -1;
    bool ret;
    if (!PyArg_ParseTuple(args, "|l", &timestamp))
    {
        return NULL;
    }
    ret = check_tracelimit(timestamp);
    if (ret == true)
    {
        return Py_BuildValue("O", Py_True);
    }
    else
    {
        return Py_BuildValue("O", Py_False);
    }
}

static PyObject *py_force_flush_span(PyObject *self, PyObject *args)
{
    int32_t timeout = 3;
    int id = -1;
    if (!PyArg_ParseTuple(args, "|ii", &timeout, &id))
    {
        return NULL;
    }

    if (id == -1)
        id = pinpoint_get_per_thread_id();

    if (global_agent_info.inter_flag & E_DISABLE_GIL)
    {
        pinpoint_force_end_trace(id, timeout);
    }
    else
    {
        Py_BEGIN_ALLOW_THREADS
            pinpoint_force_end_trace(id, timeout);
        Py_END_ALLOW_THREADS
    }

    return Py_BuildValue("O", Py_True);
}

static inline uint32_t startTraceWithPerThreadId(void)
{
    uint32_t id = pinpoint_start_trace(pinpoint_get_per_thread_id());
    pinpoint_update_per_thread_id(id);
    return id;
}

static PyObject *py_pinpoint_start_trace(PyObject *self, PyObject *args)
{
    int ret = 0;
    int32_t id = -1;

    if (!PyArg_ParseTuple(args, "|i", &id))
    {
        return NULL;
    }

    if (global_agent_info.inter_flag & E_DISABLE_GIL)
    {
        if (id == -1)
        {
            ret = startTraceWithPerThreadId();
        }
        else
        {
            ret = pinpoint_start_trace(id);
        }
    }
    else
    {
        Py_BEGIN_ALLOW_THREADS if (id == -1)
        {
            ret = startTraceWithPerThreadId();
        }
        else
        {
            ret = pinpoint_start_trace(id);
        }
        Py_END_ALLOW_THREADS
    }

    return Py_BuildValue("i", ret);
}

static inline int endTraceWithPerThreadId(void)
{
    int cid = pinpoint_get_per_thread_id();
    int id = pinpoint_end_trace(cid);
    pinpoint_update_per_thread_id(id);
    return id;
}

static PyObject *py_pinpoint_end_trace(PyObject *self, PyObject *args)
{
    int ret = 0;
    int32_t id = -1;
    if (!PyArg_ParseTuple(args, "|i", &id))
    {
        return NULL;
    }

    if (global_agent_info.inter_flag & E_DISABLE_GIL)
    {
        if (id == -1)
        {
            ret = endTraceWithPerThreadId();
        }
        else
        {
            ret = pinpoint_end_trace(id);
        }
    }
    else
    {
        Py_BEGIN_ALLOW_THREADS

            if (id == -1)
        {
            ret = endTraceWithPerThreadId();
        }
        else
        {
            ret = pinpoint_end_trace(id);
        }

        Py_END_ALLOW_THREADS
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_generate_unique_id(PyObject *self, CYTHON_UNUSED PyObject *unused)
{
    uint64_t ret = generate_unique_id();
    return Py_BuildValue("l", ret);
}

static PyObject *py_trace_has_root(PyObject *self, PyObject *args)
{
    int id = -1;
    if (!PyArg_ParseTuple(args, "|i", &id))
    {
        return NULL;
    }
    if (id == -1)
    {
        id = pinpoint_get_per_thread_id();
    }

    if (id == 0)
    {
        return Py_BuildValue("O", Py_False);
    }
    else
    {
        // check the input id
        int ret = pinpoint_trace_is_root(id);
        if (ret == -1)
        {
            PyErr_SetString(PyExc_Exception, "input traceId is not exist");
            return Py_BuildValue("O", Py_False);
            ;
        }
        return Py_BuildValue("O", Py_True);
    }
}

static PyObject *py_pinpoint_drop_trace(PyObject *self, PyObject *args)
{
    int id = -1;

    if (!PyArg_ParseTuple(args, "|i", &id))
    {
        return NULL;
    }

    if (id == -1)
        id = pinpoint_get_per_thread_id();

    mark_current_trace_status(id, E_TRACE_BLOCK);

    return Py_BuildValue("O", Py_True);
}

static PyObject *py_pinpoint_start_time(PyObject *self, CYTHON_UNUSED PyObject *unused)
{
    uint64_t start_time = pinpoint_start_time();
    return Py_BuildValue("l", start_time);
}

// origin from https://docs.python.org/3/extending/extending.html#calling-python-functions-from-c

static void msg_log_error_cb(char *msg)
{
    if (py_obj_msg_callback && msg)
    {
        PyObject *arglist;
        PyObject *result;
        arglist = Py_BuildValue("(s)", msg);
        result = PyObject_CallObject(py_obj_msg_callback, arglist);
        if (result == NULL)
        {
            fprintf(stderr, "%s", msg);
            PyErr_SetString(PyExc_TypeError, msg);
            return;
        }
        Py_XDECREF(result); // I don't care return
        Py_DECREF(arglist);
    }
}

static PyObject *py_pinpoint_enable_utest(PyObject *self, PyObject *args)
{
    global_agent_info.inter_flag |= E_LOGGING;
    global_agent_info.inter_flag |= E_DISABLE_GIL;

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
    global_agent_info.inter_flag |= E_UTEST;

    return Py_BuildValue("O", Py_True);
}

bool set_collector_host(char *host)
{
    if (strcasestr(host, "unix") || strcasestr(host, "tcp"))
    {
        strncpy(global_agent_info.co_host, host, MAX_ADDRESS_SIZE);
        return true;
    }
    PyErr_SetString(PyExc_TypeError, "collector_host must start with unix/tcp");
    return false;
}

/**
 * def set_agent(collector_host=, trace_limit=,enable_coroutines=)
 */
static PyObject *py_set_agent(PyObject *self, PyObject *args, PyObject *keywds)
{
    // PyObject* setting;
    bool ret = false;
    static char *kwlist[] = {"collector_host", "trace_limit", NULL};
    char *collector_host = "unix:/tmp/collector-agent.sock";
    long trace_limit = -1;
    if (PyArg_ParseTupleAndKeywords(args, keywds, "s|l", kwlist, &collector_host, &trace_limit))
    {
        global_agent_info.get_write_lock();
        ret = set_collector_host(collector_host);
        if (!ret)
        {
            goto END_OF_PARSE;
        }

        global_agent_info.trace_limit = trace_limit;

        pp_trace("collector_host:%s", collector_host);
        pp_trace("trace_limit:%ld", trace_limit);

    END_OF_PARSE:
        global_agent_info.release_lock();

        if (ret == true)
        {
            return Py_BuildValue("O", Py_True);
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

static PyObject *py_pinpoint_mark_an_error(PyObject *self, PyObject *args)
{
    char *msg = NULL;
    char *file_name = NULL;
    uint32_t line_no = 0;
    int id = -1;
    if (PyArg_ParseTuple(args, "ssi|i", &msg, &file_name, &line_no, &id))
    {
        if (id == -1)
            id = pinpoint_get_per_thread_id();

        catch_error(id, msg, file_name, line_no);
    }

    return Py_BuildValue("O", Py_True);
}

/* Module method table */
static PyMethodDef PinpointMethods[] = {
    {"start_trace", py_pinpoint_start_trace, METH_VARARGS, "def start_trace(int id=-1):# create a new trace and insert into trace chain"},
    {"end_trace", py_pinpoint_end_trace, METH_VARARGS, "def end_trace(int id=-1):# end currently matched trace"},
    {"unique_id", py_generate_unique_id, METH_NOARGS, "def unique_id()-> long"},
    {"trace_has_root", py_trace_has_root, METH_VARARGS, "def trace_has_root(int id=-1)-> long # check current whether have a root. \n True: \nFalse: \n Note: If the id is invalid, return false"},
    {"drop_trace", py_pinpoint_drop_trace, METH_VARARGS, "def drop_trace(int id=-1):# drop this trace"},
    {"start_time", py_pinpoint_start_time, METH_NOARGS, "def start_time()->long"},
    {"add_clues", py_pinpoint_add_clues, METH_VARARGS, "def add_clues(string key,string value,int id=-1,int loc=0)"},
    {"add_clue", py_pinpoint_add_clue, METH_VARARGS, "def add_clue(string key,string value,int id=-1,int loc=0)"},
    {"set_context_key", py_pinpoint_context_key, METH_VARARGS, "def set_context_key(string key,string value,int id=-1): # create a key-value pair that bases on current trace chain"},
    {"get_context_key", py_pinpoint_get_key, METH_VARARGS, "def get_context_key(string key,int id=-1)->string "},
    {"check_tracelimit", py_check_tracelimit, METH_VARARGS, "def check_tracelimit(long timestamp=-1): #check trace whether is limit"},
    {"enable_debug", py_pinpoint_enable_utest, METH_VARARGS, "def enable_debug(callback):#enable logging output(callback )"},
    {"force_flush_trace", py_force_flush_span, METH_VARARGS, "def force_flush_trace(timeout=3,int id=-1): #force flush span during timeout"},
    {"mark_as_error", py_pinpoint_mark_an_error, METH_VARARGS, "def mark_as_error(string msg,string file_name,uint line_no,int id=-1): #Found an error in this trace"},
    {"set_agent", (PyCFunction)py_set_agent, METH_VARARGS | METH_KEYWORDS, "def set_agent(collector_host=\"unix:/tmp/collector-agent.sock or tcp:host:port\",trace_limit=-1): # set pinpint collector information"},
    {NULL, NULL, 0, NULL}};

#if PY_MAJOR_VERSION > 2

static void free_pinpoint_module(void *module)
{
    Py_XDECREF(py_obj_msg_callback);

    if (py_obj_msg_callback)
    {
        Py_DECREF(py_obj_msg_callback);
    }
}

/* Module structure */
static struct PyModuleDef _pinpointPymodule = {

    PyModuleDef_HEAD_INIT,

    MODNAME,                              /* name of module */
    "python agent for pinpoint platform", /* Doc string (may be NULL) */
    -1,                                   /* Size of per-interpreter state or -1 */
    PinpointMethods,                      /* Method table */
    NULL,
    NULL,
    NULL,
    free_pinpoint_module /* free global variables*/
};

/* Module initialization function */
PyMODINIT_FUNC
PyInit__pinpointPy(void)
{

    global_agent_info.agent_type = 1700;
    strncpy(global_agent_info.co_host, "unix:/tmp/collector.sock", MAX_ADDRESS_SIZE);
    global_agent_info.inter_flag = 0;
    global_agent_info.timeout_ms = 0;
    global_agent_info.trace_limit = -1;
    register_error_cb(NULL);

    PyObject *m = PyModule_Create(&_pinpointPymodule);
    if (m == NULL)
        return NULL;

    return m;
}
#else

PyDoc_STRVAR(pinpointPy__doc__, "python agent for pinpoint platform");

PyMODINIT_FUNC
init_pinpointPy(void)
{
    PyObject *m;

    m = Py_InitModule3(MODNAME,
                       PinpointMethods,
                       pinpointPy__doc__);
    if (m == NULL)
        return;

    global_agent_info.agent_type = 1700;
    strncpy(global_agent_info.co_host, "unix:/tmp/collector.sock", MAX_ADDRESS_SIZE);
    global_agent_info.inter_flag = 0;
    global_agent_info.timeout_ms = 0;
    global_agent_info.trace_limit = -1;

    register_error_cb(NULL);
}

#endif
