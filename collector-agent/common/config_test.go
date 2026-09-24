package common

import "testing"

func TestParseServerAddress_Tcp(t *testing.T) {
	c := &Config{User: &UserSetting{BindAddress: "0.0.0.0@9999"}}
	socketType, address := c.ParseServerAddress()
	if socketType != "tcp" {
		t.Errorf("socketType = %q, want %q", socketType, "tcp")
	}
	if address != "0.0.0.0:9999" {
		t.Errorf("address = %q, want %q", address, "0.0.0.0:9999")
	}
}

func TestParseServerAddress_Unix(t *testing.T) {
	c := &Config{User: &UserSetting{BindAddress: "sock/tmp/pinpoint.sock"}}
	socketType, address := c.ParseServerAddress()
	if socketType != "unix" {
		t.Errorf("socketType = %q, want %q", socketType, "unix")
	}
	if address != "/tmp/pinpoint.sock" {
		t.Errorf("address = %q, want %q", address, "/tmp/pinpoint.sock")
	}
}
