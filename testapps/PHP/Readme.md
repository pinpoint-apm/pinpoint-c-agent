## How to write your own plugins 

### Tutorial

1. First, you need to know how to use [ pinpoint-php-aop ☚](https://github.com/pinpoint-apm/pinpoint-php-aop) and how it works. 
2. Write your own XXXPlugin.php class.
   
   ```php

    class XXXPlugins extends Candy
    {
        public function onBefore(){
            pinpoint_add_clue("stp",PHP_METHOD);
            pinpoint_add_clues(PHP_ARGS,print_r($this->args,true));
        }

        public function onEnd(&$ret){
            pinpoint_add_clues(PHP_RETURN,"ture");
        }

        public function onException($e){
            pinpoint_add_clue("EXP",$e->getMessage());
        }
    }
   ```
   > You could find some out of box plugins in [[Example/PHP/Plugins](../../Example/PHP/Plugins)].
    When you add clue(clues), DO NOT assignment with large string or string included some special characters(https://www.freeformatter.com/json-escape.html)
3. Add the function that you care about such as "///@hook:app\User::adduser" before ClassName(XXXPlugins), onBefore, onEnd or onException. If you care about all cases, please call before&after&around function.

4. Remove the "__class_index_table" file under AOP_CACHE_DIR.

5. Copy your plugins files into source tree/plugins_dir, and add this directory into composer.json{"autoload"}.
   
    ```
    "autoload": {
            "psr-4": {
                ......
                "Plugins\\": "your source tree/plugins_dir"
            }
        },
    ```
    
6. Update your autoload. $ composer update.
7. Enjoy the pinpoint-php-aop.
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

## Example

https://github.com/pinpoint-apm/pinpoint-c-agent/tree/v0.3.1/Example/PHP/Plugins

## API of Pinpoint_php_ext
[Goto pinpoint_ext_api ☚](https://github.com/pinpoint-apm/pinpoint-c-agent/blob/v0.3.1/src/PHP/pinpoint_php_api.php)