## Install pinpoint_common

### Requirement

Dependency| Version| More
---|----|---
gcc|gcc 4.7+| c++11
cmake| 3.13+ | ✔
OS| *inx | 

> Steps

1. Execute commands below:
    ```shell
     $ git clone https://github.com/pinpoint-apm/pinpoint-c-agent.git
     $ cd pinpoint-c-agent/common
     $ make && make install
    ```
    If the installation completes successfully, you will see the infomation like following:
    ```
    ...
    Install the project...
    -- Install configuration: "Release"
    -- Up-to-date: /usr/local/include/pinpoint_common
    -- Up-to-date: /usr/local/include/pinpoint_common/common.h
    -- Installing: /usr/local/lib64/libpinpoint_common_shared.so
    -- Installing: /usr/local/lib64/libpinpoint_common.a
    -- Installing: /usr/local/lib64/pkgconfig/pinpoint_common.pc
    ```
    ps: The real install path depends on you enviroment, for the information above, the install path is `/usr/local/lib64`

2. Export the pinpoint lib installing path to `LD_LIBRARY_PATH` `PKG_CONFIG_PATH` `LD_RUN_PATH`
```shell
export LD_LIBRARY_PATH=/path to pinpoint lib/:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/path to pinpoint lib/:/path to pinpoint lib/pkgconfig:$PKG_CONFIG_PATH
export LD_RUN_PATH=/path to pinpoint lib/:$LD_RUN_PATH
```
