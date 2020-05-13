from django.shortcuts import render
from django.http import HttpResponse
from django.template import loader


from datetime import datetime
import functools
import requests


from TestDemo import test_function
from TestDemo.app1 import user_function
from TestDemo import person, student, teacher, doctor
from TestDemo import test_recursion
from TestDemo import test_generatior
from TestDemo import test_higherorder
from TestDemo import test_return_func
from TestDemo import test_lambda_func
from TestDemo import test_decorator_func
from TestDemo import test_partial_func
from TestDemo import test_band
from TestDemo.test_band import BandClass
from TestDemo import test_mixin
from TestDemo import test_special
from TestDemo import test_private
from TestDemo import test_abstract
from TestDemo.test_static_class_method import Method
from TestDemo import test_exception
from TestDemo.test_exception import UserDefineException
from TestDemo import test_args
from TestDemo import test_returns
from TestDemo.DBControl import DBContrl


from plugins.PinpointCommonPlugin import PinpointCommonPlugin

GREETING = "Hello World!"


# Create your views here.
def index(request):
    template = loader.get_template('TestDemo/index.html')
    return HttpResponse(template.render(request=request))

def builtin(request, arg):
    data = abs(int(arg))
    return HttpResponse('''<h3>%d Absolute Value is %d</h3>''' % (int(arg), data))


def date(request):
    dt = datetime.now()
    return HttpResponse('''<h3>Time: %s</h3>''' % dt)


def test_func1(request):
    func1 = test_function.test_func1("hello", "world")
    return HttpResponse('''<h3>%s</h3>''' % func1)


def test_func2(request):
    instance = test_function.TestUserFunc1("Evy", "99")
    func2 = instance.test_func2()
    return HttpResponse('''<h3>%s</h3>''' % func2)


def test_func3(request):
    func3 = user_function.test_func3("hello", "world")
    return HttpResponse('''<h3>%s</h3>''' % func3)


def test_func4(request):
    instance = user_function.TestUserFunc2("Evy", "99")
    func4 = instance.test_func4()
    return HttpResponse('''<h3>%s</h3>''' % func4)


def test_inherit(request):
    instance = student.Student()
    s = instance.eat()
    instance = teacher.Teacher()
    t = instance.eat()
    instance = doctor.Doctor()
    d1 = instance.eat()
    d2 = instance.other()
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>
              <h3>%s But %s</h3>''' % (s, t, d1, d2))


def test_recursion_func(request):
    r = test_recursion.fact(3)
    return HttpResponse('''<h3>%s</h3>''' % r)


def test_generator(request):
    g1 = test_generatior.fib(6)
    g2 = test_generatior.fib(6)
    h1, h2 = "", ""

    for i in g1:
        h1 = h1 + str(i) + " "

    while True:
        try:
            h2 = h2 + str(next(g2)) + " "
        except StopIteration as e:
            h2 = h2 + e.value
            break
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_higher_order(request):
    def f(x):
        return x * x

    l1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    # r = map(f, l1)
    r = map(test_higherorder.f, l1)
    h = "["
    for i in r:
        h = h + str(i) + ","
    h = h + "]"
    return HttpResponse('''<h3>%s</h3>''' % h)


def test_return(request):
    l1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    f = test_return_func.lazy_sum(*l1)
    h = f()
    return HttpResponse('''<h3>%d</h3>''' % h)


def test_lambda(request):
    lambda_f = lambda x: x * x
    h1 = lambda_f(3)
    l = test_lambda_func.return_lambda(3)
    h2 = l()
    return HttpResponse('''<h3>%d</h3>
              <h3>%d</h3>''' % (h1, h2))


def test_decorator(request):
    h1 = test_decorator_func.func_in_decorator(3)
    return HttpResponse('''<h3>%d</h3>''' % h1)


def test_partial(request):
    func2 = functools.partial(test_partial_func.func1, b="Evy")
    h1 = func2("Hello ")
    h2 = func2("Hello ", b="Mer")
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_band_func(request):
    c1 = BandClass()
    from types import MethodType
    c1.band_func = MethodType(test_band.band_func, c1)
    h1 = c1.band_func(3)

    BandClass.func = test_band.band_func
    c2 = BandClass()
    h2 = c2.func(6)
    h3 = c1.func(9)
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2, h3))



def test_mixin_func(request):
    i1 = test_mixin.Human()
    h1 = i1.human_func()
    h2 = i1.animal_func()
    h3 = i1.think_func()
    return HttpResponse('''<h3>%s,%s,%s</h3>''' % (h1, h2, h3))


def test_special_func(request):
    i = test_special.Special("Evy", 99)
    h1 = i.common_func()
    return HttpResponse('''<h3>%s</h3>''' % h1)


def test_private_func(request):
    i = test_private.Private()
    h1 = i.common_func("Evy")
    return HttpResponse('''<h3>%s</h3>''' % h1)


def test_abstract_func(request):
    i = test_abstract.Txt()
    h1 = i.read()
    return HttpResponse('''<h3>%s</h3>''' % h1)


def test_staticmethod(request):
    h1 = Method.static_method("Hello", "Static")
    i1 = Method()
    h2 = i1.static_method("Hello", "InstanceStatic")
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_classmethod(request):
    h1 = Method.class_method("Hello", "Class")
    i1 = Method()
    h2 = i1.class_method("Hello", "InstanceClass")
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_exception_func(request):
    # h1 = test_exception.test_syntaxerror("Hello")
    # h1 = test_exception.test_nameerror("Hello")
    # h1 = test_exception.test_ZeroDivisionError(0)
    h1 = test_exception.test_userexception("Evy")
    return HttpResponse('''<h3>%s</h3>''' % h1)


def test_uncaught_exception(request):
    h1 = test_exception.test_userexception("Evy")
    return HttpResponse('''<h3>%s</h3>''' % h1)


def test_caught_exception(request):
    h1, h2 = "test", "test"
    try:
        h1 = test_exception.test_userexception("Evy")
    except UserDefineException as e:
        h2 = e.value
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_exception_in_recursion(request):
    h1 = test_exception.test_exception_in_recursion(3)
    return HttpResponse('''<h3>%s</h3>''' % h1)


def test_arguments(request):
    i1 = Method()
    f = open("TestDemo/views.py", 'r')
    l1 = [None, 123, 3.1415, True, "abc", (123, "abc"), [456, "def"], {"a":1, "b":2}, set("abc"), i1, str(UserDefineException("Evy")), f, GREETING, lambda x:x*x]
    l2 = []
    for i in l1:
        l2.append(test_args.test_args1(i))
    f.close()
    h1 = "<br>".join('%s' %str(id) for id in l2)
    h2 = test_args.test_args2("Hello", "Evy", "a", "b", "c", a="A", b="B", c="C")
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_returns_func(request):
    i1 = Method()
    f = open("TestDemo/views.py", 'r')
    l1 = [None, 123, 3.1415, True, "abc", (123, "abc"), [456, "def"], {"a":1, "b":2}, set("abc"), i1, str(UserDefineException("Evy")), f, GREETING, lambda x:x*x]
    # l1 = [[456, "def"]]
    l2 = []
    for i in l1:
        l2.append(str(type(test_returns.test_returns1(i)))[8:-2])
    f.close()
    h1 = "<br>".join('%s' %str(id) for id in l2)
    h2 = test_returns.test_returns2("Hello", "Evy", "a", "b", "c", a="A", b="B", c="C")
    return HttpResponse('''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2))


def test_mysql(request):
    mysql = DBContrl("localhost", "", "", "DjangoTest")
    mysql.con_db()
    h1 = mysql.db_select("select * from polls_question;")
    mysql.db_close()
    return HttpResponse("<br>".join('%s' %str(id) for id in h1))


def test_redis(request):
    mysql = DBContrl("localhost", "root", "root", "DBTest")
    mysql.con_db()
    h1 = mysql.db_select("select * from user;")
    mysql.db_close()
    return HttpResponse("<br>".join(h1))


def call_remote(request):
    remote = request.GET['remote']
    h1 = requests.get(remote)
    return HttpResponse(h1.content)
