# With this Docker image, you can run PinPoint's PHP test app (https://github.com/pinpoint-apm) with a few light configurations. This image is using  pinpoint docker, pinpoint-collector-agent and pinpoint-php-agent.

https://github.com/pinpoint-apm/pinpoint-docker <br>
https://github.com/pinpoint-apm/pinpoint-c-agent/tree/master <br>
https://github.com/pinpoint-apm/pinpoint-c-agent/blob/master/DOC/PHP/Readme.md

## Setup (run the following Commands in Terminal):<br>
### Step 1: Get PinPoint Docker
```
git clone https://github.com/pinpoint-apm/pinpoint-docker.git
cd pinpoint-docker
docker-compose pull && docker-compose up -d
```
Now you can browse into the PinPoint web User Interface. Just type "127.0.0.1:8080" in Browser.
### Step 2: Get Pinpoint-Collector-Agent
```
git clone https://github.com/pinpoint-apm/pinpoint-c-agent.git
```
### Step 3: Run the Container and go in the container to do configuration to start the php-testapp
```
cd pinpoint-c-agent/testapps/PHP-Docker
```
then 
```
docker compose up -d && docker exec -it pinpoint-php-testapp bash 
```
or
```
docker-compose up -d && docker exec -it pinpoint-php-testapp bash 
```
### Step 4: This step is optional. Collector Agent (if you want to have a look or configure the environment variables, you can do it in "~/.bashrc", so for example ```cat ~/.bashrc``` ).
If you have followed the last steps as before without changing anything you can skip this step because then you do not need to change the environment variables.
However, if you change the variables and save, then you must also run<br>
```source ~/.bashrc```
<br>or<br> 
```exit``` and then <br>```docker exec -it pinpoint-php-testapp bash``` for the change to take effect.

### Step 5: Start the Collector Agent:
```
cd /pinpoint-c-agent/collector-agent/
./collector-agent &
```
The above ``&`` means that the collector agent is running in the background. Thus we can continue to work in this terminal.

### Step 6: Get Host IP of the container we are in:<br>
Run
<br>```hostname -I```<br>
and remember the IP which is output

### Step 7: Changing the IP for the pinpoint-php-testapp:
```
cd /pinpoint-c-agent/testapps/PHP/thinkphp/library/think/console/command
nano RunServer.php
``` 
Change in line 25 the IP "172.24.0.12" with the IP of Step 6. If it is the same u can let it.
save and exit by typing "Ctrl+X" and then type "y" and hit the "enter" button.

### Step 8: Finally start the pinpoint-php-Testapp:
```
cd /pinpoint-c-agent/testapps/PHP/
php think run
```
Done

### Now the pinpoint-php-Testapp is running and you can browse to if you enter in the browser "localhost:8000".
### And you can see it of course in the PinPoint web UI if you browse to "localhost:8080"

