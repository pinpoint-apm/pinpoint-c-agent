import requests
from plugins.NextSpanPlugin import NextSpanPlugin


nextSpanHookSet = [
    ('requests','post'),
    ('requests','get')
]

for hook in nextSpanHookSet:
    new_request_post = NextSpanPlugin(hook[0], '')
    setattr(requests,hook[1], new_request_post(requests.post))