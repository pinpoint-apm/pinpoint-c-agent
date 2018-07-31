/*
 * php_type.h
 *
 *  Created on: Mar 15, 2017
 *      Author: bluse
 */

#ifndef PINPOINT_PHP_PINPOINT_TYPE_H_
#define PINPOINT_PHP_PINPOINT_TYPE_H_

/*******************************************/
/* host proccess info str  */
/* take php string as stander*/
/*******************************************/
namespace Pinpoint
{

namespace Naming
{

extern const char* gNameTable[];

enum eName
{
    ENAME_MIN,
    USER = 0,
    HOME,
    FCGI_ROLE,
    SCRIPT_FILENAME,
    QUERY_STRING,
    REQUEST_METHOD,
    CONTENT_TYPE,
    CONTENT_LENGTH,
    SCRIPT_NAME,
    REQUEST_URI,
    DOCUMENT_URI,
    DOCUMENT_ROOT,
    SERVER_PROTOCOL,
    REQUEST_SCHEME,
    GATEWAY_INTERFACE,
    SERVER_SOFTWARE,
    REMOTE_ADDR,
    REMOTE_PORT,
    SERVER_ADDR,
    SERVER_PORT,
    SERVER_NAME,
    REDIRECT_STATUS,
    HTTP_HOST,
    HTTP_CONNECTION,
    HTTP_XIP,
    HTTP_CACHE_CONTROL,
    HTTP_DF,
    HTTP_USER_AGENT,
    HTTP_ASFDAS,
    HTTP_ASDFASDF,
    HTTP_ASDFSA,
    HTTP_POSTMAN_TOKEN,
    HTTP_ACCEPT,
    HTTP_ACCEPT_ENCODING,
    HTTP_ACCEPT_LANGUAGE,
    PHP_SELF,
    REQUEST_TIME_FLOAT,
    REQUEST_TIME,
    ENAME_MAX
};

////////////////////////////////////////////////////
#define MAINPROCCESSID "mpid"

}

}

/*******************************************/
#endif /* PINPOINT_PHP_PINPOINT_TYPE_H_ */
