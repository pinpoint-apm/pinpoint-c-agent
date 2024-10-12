cmake_minimum_required(VERSION 3.15)
project(libevent_c_http)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
set(EVENT__DISABLE_OPENSSL ON CACHE INTERNAL "")
set(EVENT__DISABLE_MBEDTLS ON CACHE INTERNAL "")
set(EVENT__DISABLE_TESTS ON CACHE INTERNAL "")
set(EVENT__DISABLE_SAMPLES ON CACHE INTERNAL "")
set(EVENT__DISABLE_BENCHMARK ON CACHE INTERNAL "")

FetchContent_Declare(
  pinpoint 
  # URL https://github.com/pinpoint-apm/pinpoint-c-agent/releases/download/v0.7.0/pinpoint-common.tar.gz
  URL https://github.com/eeliu/pinpoint-c-agent/releases/download/v0.7.1/pinpoint-common.tar.gz
)

FetchContent_Declare(
  libevent
  URL https://github.com/libevent/libevent/releases/download/release-2.2.1-alpha/libevent-2.2.1-alpha-dev.tar.gz
)

FetchContent_MakeAvailable(libevent pinpoint)


add_executable(server server.c pp_ev_http.c)

target_link_libraries(server PRIVATE event event_core  pinpoint_common_static  pthread rt)