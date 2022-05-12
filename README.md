![Pinpoint](images/logo.png)

[![Build](https://github.com/pinpoint-apm/pinpoint-c-agent/workflows/Build/badge.svg?branch=master)](https://github.com/pinpoint-apm/pinpoint-c-agent/actions) [![Gitter](https://badges.gitter.im/naver/pinpoint-c-agent.svg)](https://gitter.im/naver/pinpoint-c-agent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![codecov](https://codecov.io/gh/pinpoint-apm/pinpoint-c-agent/branch/master/graph/badge.svg?token=KswbmFvWp3)](https://codecov.io/gh/pinpoint-apm/pinpoint-c-agent) [![License](https://img.shields.io/github/license/pinpoint-apm/pinpoint-c-agent)](LICENSE)



**Visit [our official website](http://pinpoint-apm.github.io/pinpoint/) for more information and [the Latest updates on Pinpoint](https://pinpoint-apm.github.io/pinpoint/news.html)**  


The current stable version is [the Latest](https://github.com/pinpoint-apm/pinpoint-c-agent/releases).

# Pinpoint Common Agent

It is an agent written by C++, PHP and Python languages. And we hope to support other languages by this agent. Until now, it supports **_PHP_**, **_C/CPP_** and **_PYTHON_**.

## Overview Pinpoint Common Agent

### How does it work

![How does it work](images/pinpoint_v0.5.x.png)

### Distributed Tracking system

![php_agent_example](images/php_agent_example.png)

### Call Stack

![php_agent_example_detail](images/php_agent_example_detail.png) | ![php_agent_example_memcached](images/callstack-memcached.png)
--- | ---
![php_agent_example_pdo](images/callstack-pdo.png) | ![php_agent_example_redis](images/callstack-redis.png)

### Real-time Tracking Chart

 CPU | Response Time
 --- | ---
 ![php_agent_example_pdo](images/cpu.png) | ![php_agent_example_redis](images/responsetime.png)


## Installation guide

**Components:**
* Collector-Agent (**Required**)
* PHP-Agent
* Python-Agent
* c/cpp=Agent
* Golang-Agent

### Install Collector-Agent

[How to install Collector-Agent](DOC/collector-agent/readme.md)

### Install PHP / Python / C/CPP / Golang Agent

language| tutorial
---|---
php|[English](DOC/PHP/Readme.md) [中文](DOC/PHP/Readme-CN.md) [한국어](DOC/PHP/Readme-KR.md)
python3|[English](DOC/PY/Readme.md) [中文](DOC/PY/Readme-CN.md) [한국어]((DOC/PY/Readme-KR.md))
c/cpp|[English](DOC/C-CPP/Readme.md)
golang|[go-aop-agent](https://github.com/pinpoint-apm/go-aop-agent) 

## Contact Us

* Submit an [issue](https://github.com/pinpoint-apm/pinpoint-c-agent/issues)
* [Gitter char room](https://gitter.im/naver/pinpoint-c-agent)
* Chinese Community (QQ Group: 882020485)
    <details>
    <summary> show more 
    </summary>

    QQ Group1: 897594820 | QQ Group2: 812507584 | QQ Group3: 882020485| DING Group : 21981598
    :----------------: |:----------------: | :-----------: | :-----------: 
    ![QQ Group1](images/NAVERPinpoint.png) | ![QQ Group2](images/NAVERPinpoint2.png)| ![QQ Group3](images/NAVERPinpoint3.png)| ![DING Group](images/NaverPinpoint交流群-DING.jpg)

</details>

## Contributing
We are looking forward to your contributions via pull requests.

### Contributors

<table>
<tr>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/eeliu>
            <img src=https://avatars.githubusercontent.com/u/27064129?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=eeliu/>
            <br />
            <sub style="font-size:7px"><b>eeliu</b></sub>
        </a>
    </td>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/EyelynSu>
            <img src=https://avatars.githubusercontent.com/u/41946743?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=Evelyn/>
            <br />
            <sub style="font-size:7px"><b>Evelyn</b></sub>
        </a>
    </td>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/Zoey-dot>
            <img src=https://avatars.githubusercontent.com/u/59548335?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=Zoey/>
            <br />
            <sub style="font-size:7px"><b>Zoey</b></sub>
        </a>
    </td>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/ChenGXQQ>
            <img src=https://avatars.githubusercontent.com/u/20234781?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=chenguoxi/>
            <br />
            <sub style="font-size:7px"><b>chenguoxi</b></sub>
        </a>
    </td>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/marty-macfly>
            <img src=https://avatars.githubusercontent.com/u/12715929?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=Macfly/>
            <br />
            <sub style="font-size:7px"><b>Macfly</b></sub>
        </a>
    </td>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/mayingping-Bella>
            <img src=https://avatars.githubusercontent.com/u/72844069?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=mayingping-Bella/>
            <br />
            <sub style="font-size:7px"><b>mayingping-Bella</b></sub>
        </a>
    </td>
</tr>
<tr>
    <td align="center" style="word-wrap: break-word; width: 75.0; height: 75.0">
        <a href=https://github.com/emeroad>
            <img src=https://avatars.githubusercontent.com/u/7564547?v=4 width="50;"  style="border-radius:50%;align-items:center;justify-content:center;overflow:hidden;padding-top:10px" alt=Woonduk Kang/>
            <br />
            <sub style="font-size:7px"><b>Woonduk Kang</b></sub>
        </a>
    </td>
</tr>
</table>



## License
This project is licensed under the Apache License, Version 2.0.
See [LICENSE](LICENSE) for full license text.

```
Copyright 2020 NAVER Corp.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
