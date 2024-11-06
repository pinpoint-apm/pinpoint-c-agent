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

> 왜 `c-agent` 라고 이름을 지었을까?

1. `C/Cpp` common 라이브러리의 인터페이스를 사용합니다.
2. `C`ross-platform: windows/*nux/macOS 플랫폼을 지원합니다.
3. `C`ollector-agent: pinpoint-collector를 지원하기 위한 브리지 모듈을 추가했습니다.

Pinpoint-c-agent는 [pinpoint-apm](https://github.com/pinpoint-apm/pinpoint) 모니터링 플랫폼에 연동하는데 도움을 줍니다.\
특징은 다음과 같습니다:
1. 지속적으로 유지 보수와 업데이트를 진행하고 있습니다. (첫 번째 버전 @2018-7-31)
2. 코드 자동 삽입을 지원함으로써 친화성이 높습니다.
    - python:  [PEP 318 Decorator](https://peps.python.org/pep-0318/)를 사용합니다.
    - php: PHP커널이 제공하는 `CG(*_table)` 과 AST 파싱 도구 ([nikic/PHP-Parser](https://github.com/nikic/PHP-Parser))를 사용합니다.
3. 다양한 플랫폼 지원: windows/*nux/macOS. 개발 단계에서 프로젝트에 연동하여 문제를 미리 찾아낼 수 있습니다.

## Python Agent

> py 3.9+

`pip install pinpointPy`

[더 자세한 연동 가이드](DOC/PY/Readme.md) 

## PHP Agent

> php 7.1+

1. pinpoint_php 익스텐션 (pinpoint_php.so/pinpoint_php.dll)을 설치해주세요.

` pecl install pinpoint_php `

2. 에스팩트 플러그인을 임포트하세요.

`composer require pinpoint-apm/pinpoint-php-aop`

[더 자세한 연동 가이드](DOC/PHP/Readme.md) 

## C/CPP 빌트인 API

[더 자세한 연동 가이드](DOC/C-CPP/Readme.md)


## 제공되는 테스트 프로젝트

`cd testapps && docker compose build --build-arg PHP_VERSION=7.4 && docker compose up`

[테스트 프로젝트에 대한 설명](/testapps/readme.md)

### 블로그 

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
