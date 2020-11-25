<?php

/**
 * start trace,if callstack is empty, creat a span. Otherwise, create a span event
 */
function pinpoint_start_trace(){}

/**
 * pop a trace.
 * @return current stacksize
 */
function pinpoint_end_trace(){}

/**
 * insert key,value into current span/spanevent
 * @param string $key
 * @param string $value
 */
function pinpoint_add_clue(string $key, string $value){}

/**
 * insert key,value into current span/spanevent annotation
 * @param int $key
 * @param string $value
 */
function pinpoint_add_clues(int $key, string $value){}

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
function pinpoint_drop_trace(){}

/**
 * Get appName
 */
function pinpoint_app_name(){}

/**
 *Get appID
 */
function pinpoint_app_id(){}

/**
 * @return collector-agent starttime
 */
function pinpoint_start_time(){}