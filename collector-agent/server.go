package main

import (
	"net/http"
	_ "net/http/pprof"
	"runtime"

	"flag"

	"github.com/pinpoint-apm/pinpoint-c-agent/collector-agent/server"

	log "github.com/sirupsen/logrus"
)

var (
	server_recv_buf = flag.Int("RecvBufSize", server.RECV_BUF_SIZE_DEF, "Set recv buf; eg: -RecvBufSize=409600")
)

func initServerSetting() {
	if *server_recv_buf > server.RECV_BUF_SIZE_DEF {
		server.Setting.RecvBufSize = *server_recv_buf
	}
}

func main() {
	flag.Parse()

	initServerSetting()

	server.InitServerConfig()
	spanServer := server.SpanServer{}
	// disable performance profile
	go func() {
		log.Println(http.ListenAndServe("0.0.0.0:8081", nil))
		runtime.SetBlockProfileRate(1)
	}()

	if _, err := spanServer.Run(); err != nil {
		log.Warn("SpanServer is exit ....")
	}
}
