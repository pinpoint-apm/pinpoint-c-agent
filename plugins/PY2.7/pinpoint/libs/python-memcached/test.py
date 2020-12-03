from memcache import Client

mc = Client(["127.0.0.1:11211"], debug=1)
mc.get("ok")