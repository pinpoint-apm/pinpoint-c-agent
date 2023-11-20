from django.http import HttpResponse
from django.http.request import HttpRequest


def index(request):
    return HttpResponse("Hello, world. You're at the polls index.")


def get_products(request, pk):
    assert isinstance(request, HttpRequest)
    return HttpResponse(f"product: {pk}")
