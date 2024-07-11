##  How to use testapps(playground)

### Requirement

- [ ] [docker compose plugin](https://docs.docker.com/compose/install/linux/)
- [ ] pinpoint platform [Quick-start guide](https://pinpoint-apm.gitbook.io/pinpoint/getting-started/quickstart)
- [ ] map `dev-pinpoint` host to your pinpoint-collector address
    ```
    pinpoint@pinpoint:~$ cat /etc/hosts
    # 192.168.10.11 is the address of pinpoint-collector
    192.168.10.11 dev-pinpoint 
    ```
### Playground

```
$ git clone --recurse-submodules https://github.com/pinpoint-apm/pinpoint-c-agent.git 
$ cd pinpoint-c-agent && git checkout dev
$ cd testapps && docker compose build --build-arg PHP_VERSION=7.4 && docker compose up
$ ## testapp-fastapi
$ curl http://localhost:8186/docs#/
$ ## testapp-php yii2 framework
$ curl http://localhost:8185/index.php
$ ## testapp-flask
$ curl http://localhost:8184/
$ ## testapp-django
$ curl http://localhost:8188/admin
```

#### Tested List

Agent|Project|
|---|---|
PHP|Yii2|
||cachethq|
||flarum|
||phpmyadmin|
||wordpress|
||shopware|
||cli|
PY|django|
||fastapi|
||flask|