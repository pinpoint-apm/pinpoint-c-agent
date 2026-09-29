# Security Policy

## ⚠️ Known Vulnerability (Fixed in v0.7.9)

collector-agent **v0.7.8 (and earlier)** contains a remote denial-of-service
vulnerability: an unauthenticated remote attacker can crash the collector-agent
with a single TCP packet (type=1 `REQ_UPDATE_SPAN`), causing a denial of service
for all traced applications.

**Action required**: upgrade the collector-agent to
**[v0.7.9](https://github.com/pinpoint-apm/pinpoint-c-agent/releases/tag/v0.7.9)**
as soon as possible.

## [pinpointPy@pypi](https://pypi.org/project/pinpointPy/) 

### Supported Versions


| Version | Supported          | Collector-Agent |
| ------- | ------------------ | --------------- |
| 1.4.x   | :white_check_mark: | v0.7.x          |
| 1.3.x   | :white_check_mark: | v0.6.x          |
| 1.2.x   | :x:                |
| 1.1.x   | :x:                |

## [pinpoint-php@pecl](https://pecl.php.net/package/pinpoint_php) 

### Supported Versions


| Version | Supported          | Collector-Agent |
| ------- | ------------------ | --------------- |
| 0.6.x   | :white_check_mark: | v0.7.x          |
| 0.5.x   | :white_check_mark: | v0.6.x          |



## Reporting a Vulnerability

* [Email Us](mailto:dl_cd_pinpoint@navercorp.com)
* Submit an [issue](https://github.com/pinpoint-apm/pinpoint-c-agent/issues) 
