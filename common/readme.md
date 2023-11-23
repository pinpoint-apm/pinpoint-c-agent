# pinpoint-common-library

## Command

```shell
$ cmake -DWITH_TEST_CASE=1 -DWITH_CODECOVERAGE=1 -DCMAKE_BUILD_TYPE=Debug  .. 
$ ./bin/TestCommon --gtest_filter=node.wakeTrace
```

## Changes

- 2023/11/23
  - support FetchContent #561

