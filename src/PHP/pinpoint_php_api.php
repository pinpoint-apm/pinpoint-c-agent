<?php
define("PINPOINT_ROOT_LOC",1);
/**
 * start trace,if callstack is empty, create a span. Otherwise, create a span event
 */
function pinpoint_start_trace($id=-1){}

/**
 * pop a trace.
 * @return current stack size
 */
function pinpoint_end_trace($id=-1){}

/**
 * insert key,value into current span/spanevent
 * @param string $key
 * @param string $value
 */
function pinpoint_add_clue(string $key, string $value,$id=-1,$loc=0){}

/**
 * insert key,value into current trace context
 * @param string $key
 * @param string $value
 */
function pinpoint_set_context(string $key, string $value,$id=-1,$loc=0){}

/**
 * get key-value from current trace
 * @param string $key
 * @return string value
 */
function pinpoint_get_context(string $key,$id=-1){}

/**
 * insert key,value into current span/spanevent annotation
 * @param int $key
 * @param string $value
 */
function pinpoint_add_clues(int $key, string $value,$id=-1,$loc=0){}

/**
 * create a process unique id
 * @return int
 */
function pinpoint_unique_id(){}

/**
 * if $timestamp is given, use $timestamp. Or use time()
 * @param int $timestamp
 */
function pinpoint_tracelimit(int $timestamp=null){}

/**
 *Drop current span. Will not send to collector-agent
 */
function pinpoint_drop_trace($id=-1){}

/**
 * @return collector-agent start time
 */
function pinpoint_start_time(){}