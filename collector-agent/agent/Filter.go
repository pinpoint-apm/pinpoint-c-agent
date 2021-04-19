package agent

type Filter interface {
	Interceptor(map[string]interface{}) bool
}
