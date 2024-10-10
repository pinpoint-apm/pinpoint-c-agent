package common

type Api_Type int

const (
	API_WEB_REQUEST Api_Type = 100
	API_DEFAULT     Api_Type = 0
	API_INVOCATION  Api_Type = 200
)

type Meta_Type int32

const (
	META_Default_api Meta_Type = iota
	META_Web_request_api
	META_String_api
	META_Sql_uid_api
	META_INVOCATION_API
)
