package server

type ServerSetting struct{
	RecvBufSize int
}

const RECV_BUF_SIZE_DEF = 4096*100

var Setting = &ServerSetting{
	RecvBufSize: RECV_BUF_SIZE_DEF,
}