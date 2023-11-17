## Command

```sh
$ python manage.py migrate
$ python manage.py runserver 9090
```


### Create user before `/admin`

https://docs.djangoproject.com/en/4.1/topics/auth/default/

```
python manage.py createsuperuser --username=joe --email=joe@example.com
```