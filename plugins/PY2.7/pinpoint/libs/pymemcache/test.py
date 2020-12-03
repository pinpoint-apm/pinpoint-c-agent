from pymemcache.client.base import Client

client = Client('localhost')
client.set('some_key', 'some_value')
result = client.get('some_key')