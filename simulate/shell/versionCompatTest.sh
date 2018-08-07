#!/usr/bin/env bash
# Copyright 2018 NAVER Corp.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

BIN_PATH_PREFIX=/home/Newfile/var/
TMP_PATH=/home/Newfile/tmp/
APACHE_BIN_PATH=/home/jenkins/tools/httpd-2.2.34-bin/
ROOT_PATH=$ROOT
RESULT_HTML=$HTML_PATH"("`date "+%Y-%m-%d"`").html"

OS_INFO=centos-`lsb_release -a | awk -F $"[:.\t ]" ' $1 == "Release"{print $3}'`.x

EXTENSION_ARRAY=(
  /home/jenkins/tools/phpredis/
  )


function getAbsolutePath(){
    _curPath=$(pwd)
    cd $1
    echo $(pwd)
    cd $_curPath
}



installExtension(){
  for i in ${EXTENSION_ARRAY[@]}; do
      cd $i
      phpize
    ./configure $JENKINS_DEFINE_CONFIG && make -j$CPUNUM -B 
    if [[ $? -ne 0 ]]; then
         return 1
    fi 
    make install
  done
}

function installNodebugPHPFromTar(){

    echo "tar path:" $1
    echo "file name: " $(basename $1)
    echo "intall path: " ${BIN_PATH_PREFIX}$(basename $1)
    tarFolderName=`tar -xvf $1 -C $TMP_PATH | tail -n 1 | awk -F'/' '{print $1}'`
    echo "php versionID:" $tarFolderName
    ## load install result 
    ## rewrite sys env 
    DST_DIR=${BIN_PATH_PREFIX}/${tarFolderName}-$OS_INFO-release
    
    export PHP_SOURCE_PATH=${DST_DIR}
    export PATH=${DST_DIR}/bin:$JENKINS_PATH


    if [ ! -d $DST_DIR ]; then
        cd $TMP_PATH/${tarFolderName}
        ## call configure 
        cmd="./configure --prefix=${DST_DIR}/  --with-apxs2=${APACHE_BIN_PATH}/bin/apxs         --with-libxml-dir         --with-gd         --with-jpeg-dir         --with-curl         --with-png-dir         --with-freetype-dir         --with-iconv-dir         --with-bz2         --with-pdo-mysql         --enable-mysqlnd    --with-mysqli     --enable-soap         --enable-gd-native-ttf         --enable-ftp         --enable-mbstring         --enable-sockets         --enable-exif   --with-zlib-dir=/home/jenkins/tools/zlib-1.2.11"

        phpVersion=${tarFolderName%.*}
        if [[ $phpVersion == "php-5.3" ]]; then
           #cmd=$cmd"    --enable-fpm ${JENKINS_DEFINE_CONFIG} "
           echo $phpVersion
        elif [[  $phpVersion == "php-5.4" ]];then
            cmd=$cmd"    --enable-fpm"
            #export CC=distcc
            #export CXX=distcc
        else 
            cmd=$cmd"    --enable-fpm ${JENKINS_DEFINE_CONFIG} "
        fi
        eval $cmd
        make -j$CPUNUM -B && make install
        cp sapi/fpm/php-fpm ${DST_DIR}/bin/ || echo "php-fpm not find"
        cp .libs/libphp*.so ${DST_DIR}/bin/ 
    fi
  

    rm ${APACHE_BIN_PATH}/modules/libphp*.so -rf
    cp ${DST_DIR}/bin/libphp*.so ${APACHE_BIN_PATH}/modules/ || return 1

    cp  ../conf/httpd.conf.base ../conf/httpd.conf
    cp  ../conf/fastcgi_params ../tmp/
    var=`ls ${DST_DIR}/bin/libphp*.so`

    if [[ ${var##*/} = "libphp5.so" ]]; then
        sed -i "s/INCLUDE_PHPMODULE/LoadModule php5_module  modules\/libphp5.so/g" ../conf/httpd.conf
    elif [[ ${var##*/} = "libphp7.so" ]]; then
        #statements
        sed -i "s/INCLUDE_PHPMODULE/LoadModule php7_module  modules\/libphp7.so/g" ../conf/httpd.conf
    else
        return 1
    fi

    return 0
}


function installPHPFromTar(){
    echo "tar path:" $1
    echo "file name: " $(basename $1)
    echo "intall path: " ${BIN_PATH_PREFIX}$(basename $1)
    tarFolderName=`tar -xvf $1 -C $TMP_PATH | tail -n 1 | awk -F'/' '{print $1}'`
    echo "php versionID:" $tarFolderName
    ## load install result 
    ## rewrite sys env 
    DST_DIR=${BIN_PATH_PREFIX}/${tarFolderName}-$OS_INFO
    
    export PHP_SOURCE_PATH=${DST_DIR}
    export PATH=$DST_DIR/bin:$JENKINS_PATH



    if [ ! -d ${DST_DIR} ]; then
        cd $TMP_PATH/$tarFolderName
        ## call configure 
        cmd="./configure --prefix=${DST_DIR}/  --with-apxs2=${APACHE_BIN_PATH}/bin/apxs         --with-libxml-dir         --with-gd         --with-jpeg-dir         --with-curl         --with-png-dir         --with-freetype-dir         --with-iconv-dir         --with-bz2         --with-pdo-mysql         --enable-mysqlnd    --with-mysqli     --enable-soap         --enable-gd-native-ttf         --enable-ftp         --enable-mbstring         --enable-sockets         --enable-exif         --enable-debug         --with-zlib-dir=/home/jenkins/tools/zlib-1.2.11"

        phpVersion=${tarFolderName%.*}
        if [[ $phpVersion == "php-5.3" ]]; then
           #cmd=$cmd"    --enable-fpm ${JENKINS_DEFINE_CONFIG} "
           echo $phpVersion
        elif [[  $phpVersion == "php-5.4" ]];then
            cmd=$cmd"    --enable-fpm"
            #export CC=distcc
            #export CXX=distcc
        else 
            cmd=$cmd"    --enable-fpm ${JENKINS_DEFINE_CONFIG} "
        fi
        eval $cmd
        make -j$CPUNUM -B && make install
        cp sapi/fpm/php-fpm ${DST_DIR}/bin/ || echo "php-fpm not find"
        cp .libs/libphp*.so ${DST_DIR}/bin/ 
    fi
  
    cd $TMP_PATH/$tarFolderName
    rm ${APACHE_BIN_PATH}/modules/libphp*.so -rf
    cp ${DST_DIR}/bin/libphp*.so ${APACHE_BIN_PATH}/modules/ || return 1
    CONF_PATH=$ROOT_PATH/simulate/conf
    cp  $CONF_PATH/httpd.conf.base $CONF_PATH/httpd.conf 
    cp  $CONF_PATH/fastcgi_params $ROOT_PATH/simulate/tmp/

    var=`ls ${DST_DIR}/bin/libphp*.so`
    if [[ ${var##*/} = "libphp5.so" ]]; then
        sed -i "s/INCLUDE_PHPMODULE/LoadModule php5_module  modules\/libphp5.so/g" $CONF_PATH/httpd.conf
    elif [[ ${var##*/} = "libphp7.so" ]]; then
        #statements
        sed -i "s/INCLUDE_PHPMODULE/LoadModule php7_module  modules\/libphp7.so/g" $CONF_PATH/httpd.conf
    else
        return 1
    fi

    return 0
}

# $1 file
function sendNoticeMail(){
    if [ -z $PINPOINT_MAIL_USER ];then
      echo "no user register no mail will send"
    else
      echo "send mail to " $PINPOINT_MAIL_USER
      mail -s "[PP-TEST]""${2} ${3} " $PINPOINT_MAIL_USER < $1
    fi
}


function releaseThisVersion(){

    version=`echo ${PHP_SOURCE_PATH##*/}|sed s/[[:space:]]//g`
    ## check  if  php's version is  shell env proivided 
    if [[ ! ${version} =~ `php -v | head -n 1 | awk -F" " '{ print $2}'` ]]
    then
        echo $version " not include " `php -v | head -n 1 | awk '{print $2}'`
        return 
    fi
    mkdir -p ../log/

    ## call auto test 
    echo "export PHP_SOURCE_PATH="$PHP_SOURCE_PATH >>../log/${version}_config.log
    echo "export PATH="$PATH >>../log/${version}_config.log
    cd $ROOT_PATH/pinpoint_php
    ./Build.sh --enable-release && make install
    mkdir -p ../Release 
    cp .libs/pinpoint.so ../Release/${version}-pinpoint.so

}

function testThisVersionPHP(){
    export >${PHP_SOURCE_PATH}.log
    version=`echo ${PHP_SOURCE_PATH##*/}|sed s/[[:space:]]//g`
    ## check  if  php's version is  shell env proivided 
    if [[ ! ${version} =~ `php -v | head -n 1 | awk -F" " '{ print $2}'` ]]
    then
        echo $version " not include " `php -v | head -n 1 | awk '{print $2}'`
        return 
    fi

    cd $ROOT_PATH/simulate/shell/
    mkdir -p ../log/

    ## call auto test 
    echo "export PHP_SOURCE_PATH="$PHP_SOURCE_PATH >>../log/${version}_config.log
    echo "export PATH="$PATH >>../log/${version}_config.log

    export PINPOINT_CXXFLAG='-DTEST_SIMULATE'
    export PINPOINT_CFLAG='-DTEST_SIMULATE'
   
    echo "run python run_test.py" && python run_test.py ../conf/php_test_conf_ci.json >>../log/${version}_result.log 2>&1

    cat ../log/${version}_config.log > ./mail.txt
    echo "---------------------------------" >>./mail.txt
    tail -n 50 ../log/${version}_result.log | tac >> ./mail.txt
    resultStr=`tail -n 1 ../log/${version}_result.log`

    ## check apache error log
    leaks_count=`grep -r "memory leaks" ../log/ | wc -l`
    Load_count=`grep -r "Cannot load" ../log/ | wc -l`
    resultStr=$resultStr" leaks_count="$leaks_count" Load_count="$Load_count

    # generate gcovr result
    if [ "$TEST_NEED_GCOV" = "YES" ]; then
        echo "run gcovr .................";
        cd $ROOT_PATH && gcovr -e test* -e pinpoint_common.thrift.gen-cpp -e simulate.collector -r $ROOT_PATH --xml >${version}_coverage.xml && mv ${version}_coverage.xml  $ROOT_PATH/CI/target/coverage-results/
    fi

    ## close php-fpm
    echo "pkill php-fpm" && pkill php-fpm

    ## collect the last word apps said

    echo "delete versin file" &&  rm -rf $ROOT_PATH/CI/$version
    echo "mkdir -p " && mkdir -p $ROOT_PATH/CI/$version
    Core_count=`find ../ -name core.* | wc -l`
    AgentLog_count=`wc -l ../log/agent/pinpoint_log.txt | awk '{print $1}'`
    resultStr=$resultStr" Core_count="$Core_count" AgentLog_count="$AgentLog_count
    find ../ -name core.* | xargs -I '{}' mv '{}' $ROOT_PATH/CI/$version/

    cp -rf ../log $ROOT_PATH/CI/$version/ 
    
    rm  ../log/* -rf 
    cp -rf ../testcase $ROOT_PATH/CI/$version/
    cp -rf ../tmp $ROOT_PATH/CI/$version/

    ## disabled as so shell exsit in php extension
    # mv ../../pinpoint_php/.libs/pinpoint.so $CUR_DIR/CI/$version/
    ## clean all the stuff leave behind

    ## send mail 
    echo $resultStr >>./mail.txt
    sendNoticeMail ./mail.txt ${version} $resultStr
    echo  "<tr><td>"  `date`  "</td> <td>" ${version} "</td> <td>" ${resultStr}  "</td> </tr>" >>$ROOT_PATH/DataFile.txt
}

function startTestAllPHP(){
    echo "path: " $1
    _curdir=$(pwd)
    for var in `ls $1/$2`; do
        echo $var
        # installPHPFromTar $var
        $4 $var
        if [ $? -eq 1 ] 
        then
            echo  "<tr><td>"  `date`  "</td> <td>" $var "</td> <td>" install failed  "</td> </tr>" >>$CUR_DIR/DataFile.txt
            continue
        fi

        installExtension

        if [ $? -ne 0 ]
        then
             echo  "<tr><td>"  `date`  "</td> <td>" $var "</td> <td>" wtf : extension instlal failed  "</td> </tr>" >>$CUR_DIR/DataFile.txt
            continue 
        fi

        cd $_curdir
        $3 
    done
}

function init_env(){
    if [ -z '$JENKINS_DEFINE_CONFIG' ];then
        export CPUNUM=`cat /proc/cpuinfo |grep "processor"|wc -l`
    else
        export CPUNUM=$DISTCC_NODE_COUNT
    fi
    echo "Build node: "${CPUNUM}

    if [ -z $WORKSPACE ];then
       export  CUR_DIR=`getAbsolutePath ../../`
    else
       export  CUR_DIR=$WORKSPACE
    fi
}

function cleanup_last_build(){
    cd $ROOT_PATH/pinpoint_php
   ./Build.sh clean
}

function txtToHtml(){
    ## $1
    ## $2
    echo "<!DOCTYPE html><html><head><style>table, th, td {    border: 1px solid black;}</style></head><body><table>  <tr><th>Time </th><th>PHP Version</th><th>Test Result </th></tr>" > $RESULT_HTML
        
    cat $ROOT_PATH/DataFile.txt >> $RESULT_HTML
        
    echo "</table></body></html>" >> $RESULT_HTML
    
}


function main(){

    init_env && cd $ROOT_PATH

    cleanup_last_build && cd $ROOT_PATH

    if [ -z $PHP_PATH ]; then
        startTestAllPHP  /home/jenkins/tools/PHPSource/ $1 $2 $3
    else
        startTestAllPHP  $PHP_PATH $1 $2 $3
    fi

    ## collect test result 
    echo ".... collect test result ...."
    txtToHtml 

}


## I'M MAIN 
if [[ $1 = "run" ]]; then
  main $2 testThisVersionPHP installPHPFromTar
elif [[ $1 = "clean" ]]; then
  rm ../../*.xml
  rm ../../CI/php-* -rf
elif [[ $1 = "release" ]]; then
  # WORK_PATH=$(pwd)
  main $2 releaseThisVersion  installNodebugPHPFromTar
  main $2 releaseThisVersion  installPHPFromTar
else
  echo "run"
  echo "clean"
  echo "release"
fi


#LoadModule php7_module        modules/libphp7.so
#LoadModule php5_module        modules/libphp5.so
