[English](Readme.md) | [中文](Readme-CN.md) | [한국어](Readme-KR.md)

![pinpoint](images/logo.png)

[![CI](https://github.com/pinpoint-apm/pinpoint-c-agent/actions/workflows/main.yml/badge.svg)](https://github.com/pinpoint-apm/pinpoint-c-agent/actions/workflows/main.yml) [![Gitter](https://badges.gitter.im/naver/pinpoint-c-agent.svg)](https://gitter.im/naver/pinpoint-c-agent?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![codecov](https://codecov.io/gh/pinpoint-apm/pinpoint-c-agent/branch/master/graph/badge.svg?token=KswbmFvWp3)](https://codecov.io/gh/pinpoint-apm/pinpoint-c-agent) [![License](https://img.shields.io/github/license/pinpoint-apm/pinpoint-c-agent)](LICENSE) \
`pinpoint_php_ext:` [![pinpoint_php](https://img.shields.io/badge/php-7.1~8.3-8892BF)](https://pecl.php.net/package/pinpoint_php)\
`pinpoint-php-aop:` ![Packagist License](https://img.shields.io/packagist/l/pinpoint-apm/pinpoint-php-aop)
 [![Total Downloads](https://img.shields.io/packagist/dt/pinpoint-apm/pinpoint-php-aop.svg?style=flat-square)](https://packagist.org/packages/pinpoint-apm/pinpoint-php-aop)\
`pinpointpy:` ![PyPI - License](https://img.shields.io/pypi/l/pinpointpy)
 [![pypi](https://badge.fury.io/py/pinpointpy.svg)](https://pypi.org/project/pinpointpy/)
![PyPI - Downloads](https://img.shields.io/pypi/dm/pinpointpy)


**Visit [our official website](http://pinpoint-apm.github.io/pinpoint/) for more information and [the Latest updates on Pinpoint](https://pinpoint-apm.github.io/pinpoint/news.html)**  


# Pinpoint `C` Agent

> 为啥取名为 `c-agent`

1. 使用了 `C/Cpp` common 库的接口
2. `C`ross-platform: 支持 windows/*nux/macOS 平台
3. `C`llector-agent: 添加了一个桥接模块支持pinpoint-collector

Pinpoint-c-agent 帮助您接入 [pinpoint-apm](https://github.com/pinpoint-apm/pinpoint) 监控平台.\
我们的特点:
1. 一直在维护和迭代. (第一个版本 @2018-7-31)
2. 支持自动切入代码，对您的项目更友好。
    - python: 使用了 [PEP 318 Decorator](https://peps.python.org/pep-0318/) 
    - php: 使用  PHP内核提供的`CG(*_table)` 和AST 解析工具 ([nikic/PHP-Parser](https://github.com/nikic/PHP-Parser))
3. 支持多种平台: windows/*nux/macOS. 方便你在开发的时候就接入项目，提前发现问题。

## Python Agent

> py 3.8+

`pip install pinpointPy`

[更详细的接入文档](DOC/PY/Readme.md) 

## PHP Agent

> php 7.1+

1. 安装 pinpoint_php 扩展 (pinpoint_php.so/pinpoint_php.dll)

` pecl install pinpoint_php `

2. 导入切面插件

`composer require pinpoint-apm/pinpoint-php-aop`

[更详细的接入文档](DOC/PHP/Readme.md) 

## C/CPP built in API

[更详细的接入文档](DOC/C-CPP/Readme.md)


## 提供的测试项目

`cd testapps && docker compose build --build-arg PHP_VERSION=7.4 && docker compose up`

[测试项目说明](/testapps/readme.md)

### 博客 

- [Intro pinpoint php aop](https://github.com/pinpoint-apm/pinpoint-php-aop/wiki/Intro-pinpoint-php-aop)
- [pinpoint php aop 内部原理](https://github.com/pinpoint-apm/pinpoint-php-aop/wiki/pinpoint-php-aop-%E5%86%85%E9%83%A8%E5%8E%9F%E7%90%86)
- [pinpoint php aop 내부 원리](https://github.com/pinpoint-apm/pinpoint-php-aop/wiki/pinpoint-php-aop-%EB%82%B4%EB%B6%80-%EC%9B%90%EB%A6%AC)


## Contact Us

* Submit an [issue](https://github.com/pinpoint-apm/pinpoint-c-agent/issues) 👍👍
* [Email Us](mailto:dl_cd_pinpoint@navercorp.com)   👍👍
* [Gitter char room](https://gitter.im/naver/pinpoint-c-agent)
* QQ us (QQ Group: 882020485)
    <details>
    <summary> show more 
    </summary>

    |          QQ Group1: 897594820          |          QQ Group2: 812507584           |          QQ Group3: 882020485           |               DING Group : 21981598                |
    | :------------------------------------: | :-------------------------------------: | :-------------------------------------: | :------------------------------------------------: |
    | ![QQ Group1](images/NAVERPinpoint.png) | ![QQ Group2](images/NAVERPinpoint2.png) | ![QQ Group3](images/NAVERPinpoint3.png) | ![DING Group](images/NaverPinpoint交流群-DING.jpg) |
</details>

## Contributing

Thanks all [![GitHub contributors](https://img.shields.io/github/contributors/pinpoint-apm/pinpoint-c-agent)](https://github.com/pinpoint-apm/pinpoint-c-agent/graphs/contributors)

We are looking forward to your contributions via pull requests.

## License
This project is licensed under the Apache License, Version 2.0.
See [LICENSE](LICENSE) for full license text.

```
Copyright 2024 NAVER Corp.

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
