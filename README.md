![Pinpoint](images/logo.png)

[![Build Status](https://travis-ci.com/naver/pinpoint-c-agent.svg?branch=dev)](https://travis-ci.com/naver/pinpoint-c-agent)

**Visit [our official web site](http://naver.github.io/pinpoint/) for more information and [Latest updates on Pinpoint](https://naver.github.io/pinpoint/news.html)**  


The current stable version is [v0.1.1](https://github.com/naver/pinpoint-c-agent/releases).

# Pinpoint C Agent

It is an agent written by C/C++ PHP python language. And we hope to support other languages by this agent. Until now, it supports PHP language.

## Getting Started

### Requirement

Dependency|Version
---|----
APACHE| 2.2.x 2.4.x
PHP| php 7+
OS| 64bit only
Python | Python 3
pinpoint| 1.8.0-RC1
composer| 1.9+



### Installation

#### Pre-install
- Download pinpoint-c-agent:  git clone https://github.com/naver/pinpoint-c-agent.git
 

- Install Build-essential


- Install third-library  



     
-  [phpize](http://php.net/manual/en/install.pecl.phpize.php) (In your php installation path)
  
#### Build php-agent


#### Startup 

#### Collect result from the Pinpoint 
1. Configure pinpoint_c_agent/quickstart/php/web/ as your web side.
2. Access http://\$serverip:\$port/index.php 
3. Log into pinpoint-web and choose the right ApplicationList 

## Overview

### Distributed Tracking system
![CallStack](images/1.png)

### Call Stack
![CallStack](images/2.png)


## Compatibility

Pinpoint Version | PHP| Python
---------------- | ----- | -----
1.8.0-RC1 | PHP 7+ | Python3

## License
This project is licensed under the Apache License, Version 2.0.
See [LICENSE](LICENSE) for full license text.

```
Copyright 2018 NAVER Corp.

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
