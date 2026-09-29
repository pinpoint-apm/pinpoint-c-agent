package common

import (
	"reflect"
	"testing"
)

func Test_ParseStringField(t *testing.T) {

	ar := ParseStringField("t=991424704447256 D=3775428 i=51 b=49")
	dst := map[string]string{
		"t": "991424704447256",
		"D": "3775428",
		"i": "51",
		"b": "49",
	}

	if !reflect.DeepEqual(ar, dst) {
		t.Fail()
	}
}

func Test_ParseDotFormatToTime(t *testing.T) {
	time, err := ParseDotFormatToTime("1504248328.423")

	if err != nil {
		t.Fail()
	}

	if time != 1504248328423 {
		t.Fail()
	}

}

// Regression test for the security fix: input without a "." must return an
// error instead of panicking with an index-out-of-range (previously crashed
// the whole collector-agent process via the unauthenticated span path).
func Test_ParseDotFormatToTime_NoDot(t *testing.T) {
	for _, input := range []string{"123", "", "abc"} {
		if _, err := ParseDotFormatToTime(input); err == nil {
			t.Errorf("ParseDotFormatToTime(%q) expected error, got nil", input)
		}
	}
}
