from django.http import HttpResponse
from django.http.request import HttpRequest
import requests
import psycopg2


def index(request):
    return HttpResponse("Hello, world. You're at the polls index.")


def get_products(request, pk):
    assert isinstance(request, HttpRequest)
    return HttpResponse(f"product: {pk}")


def get_url(request):
    h1 = requests.get("http://testapp-php/")
    return HttpResponse(h1.content)


def get_postgres(request) -> HttpResponse:
    # ref to https://www.psycopg.org/docs/usage.html
    conn = psycopg2.connect(dbname="test", user="test",
                            password="pinpoint", host="postgres", port=5432)
    cur = conn.cursor()
    cur.execute("DROP TABLE IF EXISTS  test")
    cur.execute(
        "CREATE TABLE test (id serial PRIMARY KEY, num integer, data varchar);")
    cur.execute("INSERT INTO test (num, data) VALUES (%s, %s)",
                (100, "abc'def"))
    cur.execute("SELECT * FROM test;")
    resp = cur.fetchone()
    conn.commit()
    cur.close()
    conn.close()
    return HttpResponse(resp)
