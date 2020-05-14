## Getting Started

### Requirement

depends| verson
---|----
python |python 3.5+
python async|python 3.7.1+
gcc|gcc 4.7+
cmake| 3.0+
*nix| 
pinpoint| 1.8.0-RC1(thrift) <br> 2.0+(GRPC)

### Installation

> Steps

1. Compile pinpointPy module
```
$ cd common
$ mkdir -p build
$ cd build
$ cmake -DWITH_TEST_CASE=1 -DCMAKE_BUILD_TYPE=Release .. 
$ make 
$ ./test/unittest
```
2. Install python virtual environment,refer to 
https://packaging.python.org/guides/installing-using-pip-and-virtual-environments/

3. Come back to the root directory, Install pinpointPy(Python virtualenv is recommend).
```shell
$ pip install -r requirements.txt 
$ python setup.py install
$ nosetests src/PY/test
```

### [How To Use](Example/PY/Readme.md)

## Changes

