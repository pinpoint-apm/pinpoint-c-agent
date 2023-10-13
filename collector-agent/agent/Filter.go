package agent

type Filter interface {
	Interceptor(span *TSpan) bool
}
