# welcome to use pinpoint-php
## How to write your own plugins 

### Tutorial

1. First, you need to know how to use [ pinpoint-php-aop ☚](https://github.com/pinpoint-apm/pinpoint-php-aop) and how it works. 
2. Write your own XXXPlugin.php class.
   
   ```php
    use Pinpoint\Plugins\Common\PinTrace;
    class XXXPlugins extends PinTrace
    {
        public function onBefore(){
            pinpoint_add_clue("stp",PHP_METHOD);
            pinpoint_add_clues(PHP_ARGS,"fetch data from $this->args");
        }

        public function onEnd(&$ret){
            pinpoint_add_clues(PHP_RETURN,"parse data from $ret");
        }

        public function onException($e){
            pinpoint_add_clue("EXP",$e->getMessage());
        }
    }
   ```
   > You could find some out of box plugins in [[Plugins Example](../../testapp/PHP/Plugins/AutoGen/app)].
    When you add clue(clues), DO NOT assignment with large string or string included some special characters(https://www.freeformatter.com/json-escape.html)
3. Add the function that you care about such as "///@hook:app\User::adduser" before ClassName(XXXPlugins), onBefore, onEnd or onException. If you care about all cases, please call before&after&around function.

4. Inject plugin into your project: 
    $ cd your_project_folder
    $ mkdir Plugins/AutoGen
    $ cp your_xxxplugins.php Plugins/AutoGen
    
5. Enable autoloading Plugins/ into composer.json 
   
    ```
    "autoload": {
            "psr-4": {
                ......
                # location for your Plugins
                "Plugins\\": "Plugins" 
            }
        },
    ```
    
6. Update your autoload. $composer update.

## Protocol

> Json -> Thrift/GRPC

```
                   +------------------+
                   |  php-fpm         |
                   |  pinpoint_php.so |
                   +------------------+
                           |
                json       |
            unix:socket    |
                           v
                    +----------------+
                    |collector-agent |
                    |                |
                    +----------------+
                           |
                           |    thrift TCP&UDP
                           |    GRPC
                           v
                  +----------------------+
                  |  pinpoint collector  |
                  +----------------------+

```

## API of Pinpoint_php_ext
[Goto pinpoint_ext_api ☚](../../src/PHP/pinpoint_php_api.php)


# 欢迎使用pinpoint-php 监控的你PHP应用