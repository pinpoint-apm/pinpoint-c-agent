import requests
from plugins.NextSpanPlugin import NextSpanPlugin


HookSet = [
    ('requests','post', requests, requests.post),
    ('requests','get', requests, requests.get)
]

for hook in HookSet:
    new_requests = NextSpanPlugin(hook[0], '')
    setattr(hook[2],hook[1], new_requests(hook[3]))
