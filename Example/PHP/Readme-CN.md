## 如何编写自己的插件

### 编写指南
 
1. 首先，您需要知道如何使用[ pinpoint-php-aop ☚](https://github.com/naver/pinpoint-php-aop) 以及它是怎样工作的。
2. 编写自己的XXXPlugin.php class。
   
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
   > 您可以在[[Example/PHP/Plugins](../../Example/PHP/Plugins)]中找到一些开箱即用的插件。当您添加clue(clues)的时候，不要使用大字符串或是包含一些特殊字符的字符串赋值(https://www.freeformatter.com/json-escape.html)
3. 在ClassName(XXXPlugins), onBefore, onEnd 或 onException前添加您所care的函数比如“///@hook:app\User::adduser”，如果您care所有情况的话，就调用before&after&around函数。

4. 删除 AOP_CACHE_DIR 目录下的 “__class_index_table” 文件。

5. 将您的插件文件复制到 source tree/plugins_dir，并将此目录添加到 composer .json{"autoload"} 中。
   
    ```
    "autoload": {
            "psr-4": {
                ......
                "Plugins\\": "your source tree/plugins_dir"
            }
        },
    ```
    
6. 更新您的autoload. $ composer update。
7. 现在，您可以使用pinpoint-php-agent了。
## 协议

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

## 示例

https://github.com/naver/pinpoint-c-agent/tree/v0.3.1/Example/PHP/Plugins

## API of Pinpoint_php_ext
[Goto pinpoint_ext_api ☚](https://github.com/naver/pinpoint-c-agent/blob/v0.3.1/src/PHP/pinpoint_php_api.php)