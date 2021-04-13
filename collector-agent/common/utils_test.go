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
