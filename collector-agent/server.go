package main

import (
	"CollectorAgent/server"
	_ "net/http/pprof"

	log "github.com/sirupsen/logrus"
)

func main() {
	server.InitServerConfig()
	spanServer := server.SpanServer{}
	// disable performance profile
	// go func() {
	// 	log.Println(http.ListenAndServe("0.0.0.0:8081", nil))
	// 	runtime.SetBlockProfileRate(1)
	// }()

	if _, err := spanServer.Run(); err != nil {
		log.Warn("SpanServer is exit ....")
	}
}
