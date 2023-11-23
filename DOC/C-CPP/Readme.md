## Getting Started

>At present, we don't find an easy and stable way to hack the code. You should link the common library and write your plugins on `remote call`, `database`,  `cache` and so on.

While, check examples `src/CPP/test_pinpoint.cpp` and `src/CPP/test_pinpoint.c`.
### Requirement

Dependency|Version
---|----
cmake| 3.2+
GCC| GCC `4.7+`

#### Build

>@obsoleted
Use CMake `add_subdirectory`;
[CMakeLists.txt example](../../src/CPP/CMakeLists.txt)

```
$ mkdir build
$ cd build
$ cmake .. && make 
```
#### Integrate in your own project 

> Use cmake FetchContent

[CMakeLists.txt example](../../src/CPP/CMakeLists.txt)

```shell
include(FetchContent)
FetchContent_Declare(
  pinpoint
  GIT_REPOSITORY https://github.com/pinpoint-c-agent/pinpoint-c-agent.git
  # GIT_TAG        74bc39d813d664cb56b78b1506d91932c8131396 
  # not recommended, please use hash key like `74bc39d813d664cb56b78b1506d91932c8131396`
  GIT_TAG        origin/dev
)

FetchContent_GetProperties(pinpoint)
if (NOT pinpoint_POPULATED)
    FetchContent_Populate(pinpoint)
    add_subdirectory(${pinpoint_SOURCE_DIR}/common ${pinpoint_BINARY_DIR})
endif ()

FetchContent_MakeAvailable(pinpoint)

add_executable(app test_pinpoint.cpp)
target_link_libraries(app PRIVATE  pinpoint_common_static rt)
```


### Run

```
$ ./test_pinpoint.c
$ ./test_pinpoint.cpp
```

### Result

C application | Cpp application | callstack
-----|-----|----
![c](../images/c-test-name.png)|![cpp](../images/cpp-test-name.png)|![callstack](../images/c-cpp-callstack.png)






