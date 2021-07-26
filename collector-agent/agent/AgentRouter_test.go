package agent

import "testing"

func BenchmarkGetAgentInfo(b *testing.B) {
	spanMap := map[string]interface{}{
		"appid":   "sfdaefe",
		"appname": "sfdaefe",
		"FT":      23412,
		"tid":     "234123424^41234^2333",
	}

	for i := 0; i < b.N; i++ {
		GetAgentInfo(spanMap)
	}
}

func TestGetAgentInfo(t *testing.T) {
	spanMap := map[string]interface{}{
		"appid":   "sfdaefe",
		"appname": "sfdaefe",
		"FT":      float64(23412),
		"tid":     "234123424^41234^2333",
	}

	id, name, ft, startTime, err := GetAgentInfo(spanMap)

	if id != "sfdaefe" && name != "sfdaefe" && ft != 23412 && startTime != "234123424" && err != nil {
		t.Fail()
	}

}
