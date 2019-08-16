## How to write your own plugins

### Tutorial

1. First,you need to know how to use [ php_simple_aop ☚](https://github.com/eeliu/php_simple_aop/blob/master/Readme.md) and how it works. 
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
   > You could find some out of box plugins in [[pinpoint_php_example/Plugins/](https://github.com/naver/pinpoint-c-agent/tree/dev/PHP/pinpoint_php_example/Plugins)].
    When you add clue(clues), DO NOT assignment with large string,or string included some special characters(https://www.freeformatter.com/json-escape.html)
3. Add "//@hook:app\User::adduser (which function you cared)" located right before ClassName(XXXPlugins),onBefore,onEnd or onException if you cared about it when called before&after&around,before,after or around.

4. Removed the "__class_index_table" file under AOP_CACHE_DIR.

5. Copy your plugins files into source tree/plugins_dir,and add this directory into composer.json{"autoload"}.
   
    ```
    "autoload": {
            "psr-4": {
                ......
                "Plugins\\": "your source tree/plugins_dir"
            }
        },
    ```
    
6. Update your autoload. $ composer update
7. Enjoy the pinpoint-php-agent
## Protocol

> Json -> Thrift

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
                           |
                           v
                  +----------------------+
                  |  pinpoint collector  |
                  +----------------------+

```

## Example

https://github.com/naver/pinpoint-c-agent/tree/dev/PHP/pinpoint_php_example/Plugins

## API of Pinpiont_php_ext
[Goto pinpoint_ext_api ☚](../pinpoint_php_ext/pinpoint_php_api.php)