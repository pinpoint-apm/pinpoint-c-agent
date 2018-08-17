## How to build and install pinpoint php agent

1. Checking phpize is in your PATH.
    If not, install phpize and export in your system PATH.(eg:export PATH=/path to phpize/:$PATH)
2. Specifying BOOST and THRIFT path  
    
    ```
    export WITH_BOOST_PATH='/usr/local'
    export WITH_THRIFT_PATH='/usr/local' 
    ```
3. Run ./Build.sh --enable-debug --always-make && make install
4. If **_3_** running successfully, agent had installed into php module.

## Q&A

1.  What's the difference between NGINX+PHP-FPM and APACHE+mod_php5(7).so when install pinpoint php agent.
  
    A: pinpoint php agent is an extension of php.