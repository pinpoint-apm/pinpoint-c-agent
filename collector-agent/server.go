package main

import (
	"CollectorAgent/server"
	"net/http"
	_ "net/http/pprof"
	"runtime"

	log "github.com/sirupsen/logrus"
)

func main() {
	server.InitServerConfig()
	spanServer := server.SpanServer{}
	go func() {
		log.Println(http.ListenAndServe("0.0.0.0:8081", nil))
		runtime.SetBlockProfileRate(1)
	}()

	if _, err := spanServer.Run(); err != nil {
		log.Warn("SpanServer is exit ....")
	}
}
