from django.http import HttpResponse
from django.http.request import HttpRequest
import requests

def index(request):
    return HttpResponse("Hello, world. You're at the polls index.")


def get_products(request, pk):
    assert isinstance(request, HttpRequest)
    return HttpResponse(f"product: {pk}")

def get_url(request):
    h1 =requests.get("http://testapp-php/")
    return HttpResponse(h1.content)