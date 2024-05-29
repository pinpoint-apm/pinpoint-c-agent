#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------
from RedisControl import RedisControl
from DBControl import DBControl
import test_returns
import test_args
from test_exception import UserDefineException
import test_exception
from test_static_class_method import Method
import test_abstract
import test_private
import test_special
import test_mixin
from test_band import BandClass
from flask import Flask, request, Response
from flask import render_template

from datetime import datetime
import functools
import requests

import test_function
from app1 import user_function
import person
import student
import teacher
import doctor
import test_recursion
import test_generatior
import test_higher_order
import test_return
import test_lambda
import test_decorator
import test_partial
import test_band
from pinpointPy.Flask.PinPointMiddleWare import PinPointMiddleWare
from pinpointPy import set_agent, monkey_patch_for_pinpoint, use_thread_local_context
import logging
use_thread_local_context()
monkey_patch_for_pinpoint()

app = Flask(__name__)

set_agent("cd.dev.test.flask", "cd.dev.test.py",
          'tcp:dev-collector:10000', -1, 0, logging.DEBUG)

app.wsgi_app = PinPointMiddleWare(app, app.wsgi_app)


@app.route('/', methods=['GET', 'POST'])
def home():
    return render_template("index.html")


GREETING = "Hello World!"


@app.route('/test_builtin_func/<arg>', methods=['GET'])
def builtin_form(arg):
    data = abs(int(arg))
    return '''<h3>%d Absolute Value is %d</h3>''' % (int(arg), data)


@app.route('/test_package_func', methods=['GET'])
def date_form():
    dt = datetime.now()
    return '''<h3>Time: %s</h3>''' % dt


@app.route('/test_user_func1', methods=['GET'])
def test_func1_form():
    func1 = test_function.test_func1("hello", "world")
    return '''<h3>%s</h3>''' % func1


@app.route('/test_ut/<username>', methods=['GET'])
def test_ut(username):

    return f'{username}->{request.url_rule}\'s profile'


@app.route('/test_user_func2', methods=['GET'])
def test_func2_form():
    instance = test_function.TestUserFunc1("Evy", "99")
    func2 = instance.test_func2()
    return '''<h3>%s</h3>''' % func2


@app.route('/test_user_func3', methods=['GET'])
def test_func3_form():
    func3 = user_function.test_func3("hello", "world")
    return '''<h3>%s</h3>''' % func3


@app.route('/test_user_func4', methods=['GET'])
def test_func4_form():
    instance = user_function.TestUserFunc2("Evy", "99")
    func4 = instance.test_func4()
    return '''<h3>%s</h3>''' % func4


@app.route('/test_inherit_func', methods=['GET'])
def test_inherit_form():
    instance = student.Student()
    s = instance.eat()
    instance = teacher.Teacher()
    t = instance.eat()
    instance = doctor.Doctor()
    d1 = instance.eat()
    d2 = instance.other()
    return '''<h3>%s</h3>
              <h3>%s</h3>
              <h3>%s But %s</h3>''' % (s, t, d1, d2)


@app.route('/test_recursion', methods=['GET'])
def test_recursion_form():
    print(request.url_rule)
    r = test_recursion.fact(3)
    return '''<h3>%s</h3>''' % r


@app.route('/test_generatior', methods=['GET'])
def test_generator_form():
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
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_higher_order', methods=['GET'])
def test_higher_order_form():
    from pinpointPy.CommonPlugin import PinpointCommonPlugin

    @PinpointCommonPlugin(__name__)
    def f(x):
        return x * x

    l1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    # r = map(f, l1)
    r = map(test_higher_order.f, l1)
    h = "["
    for i in r:
        h = h + str(i) + ","
    h = h + "]"
    return '''<h3>%s</h3>''' % h


@app.route('/test_return_func', methods=['GET'])
def test_return_form():
    l1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    f = test_return.lazy_sum(*l1)
    h = f()
    return '''<h3>%d</h3>''' % h


@app.route('/test_lambda', methods=['GET'])
def test_lambda_form():
    def lambda_f(x): return x * x
    h1 = lambda_f(3)
    l = test_lambda.return_lambda(3)
    h2 = l()
    return '''<h3>%d</h3>
              <h3>%d</h3>''' % (h1, h2)


@app.route('/test_decorator', methods=['GET'])
def test_decorator_form():
    h1 = test_decorator.func_in_decorator(3)
    return '''<h3>%d</h3>''' % h1


@app.route('/test_partial', methods=['GET'])
def test_partial_form():
    func2 = functools.partial(test_partial.func1, b="Evy")
    h1 = func2("Hello ")
    h2 = func2("Hello ", b="Mer")
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_band', methods=['GET'])
def test_band_form():
    c1 = BandClass()
    from types import MethodType
    c1.band_func = MethodType(test_band.band_func, c1)
    h1 = c1.band_func(3)

    BandClass.func = test_band.band_func
    c2 = BandClass()
    h2 = c2.func(6)
    h3 = c1.func(9)
    return '''<h3>%s</h3>
              <h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2, h3)


@app.route('/test_mixin', methods=['GET'])
def test_mixin_form():
    i1 = test_mixin.Human()
    h1 = i1.human_func()
    h2 = i1.animal_func()
    h3 = i1.think_func()
    return '''<h3>%s,%s,%s</h3>''' % (h1, h2, h3)


@app.route('/test_special', methods=['GET'])
def test_special_form():
    i = test_special.Special("Evy", 99)
    h1 = i.common_func()
    return '''<h3>%s</h3>''' % h1


@app.route('/test_private', methods=['GET'])
def test_private_form():
    i = test_private.Private()
    h1 = i.common_func("Evy")
    return '''<h3>%s</h3>''' % h1


@app.route('/test_abstract', methods=['GET'])
def test_abstract_form():
    i = test_abstract.Txt()
    h1 = i.read()
    return '''<h3>%s</h3>''' % h1


@app.route('/test_staticmethod', methods=['GET'])
def test_staticmethod_form():
    h1 = Method.static_method("Hello", "Static")
    i1 = Method()
    h2 = i1.static_method("Hello", "InstanceStatic")
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_classmethod', methods=['GET'])
def test_classmethod_form():
    h1 = Method.class_method("Hello", "Class")
    i1 = Method()
    h2 = i1.class_method("Hello", "InstanceClass")
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_exception', methods=['GET'])
def test_exception_form():
    h1 = test_exception.test_userexception("Evy")
    return '''<h3>%s</h3>''' % h1


@app.route('/test_uncaught_exception', methods=['GET'])
def test_uncaught_exception_form():
    h1 = test_exception.test_userexception("Evy")
    return '''<h3>%s</h3>''' % h1


@app.route('/test_caught_exception', methods=['GET'])
def test_caught_exception_form():
    h1, h2 = "test", "test"
    try:
        h1 = test_exception.test_userexception("Evy")
    except UserDefineException as e:
        h2 = e.value
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_exception_in_recursion', methods=['GET'])
def test_exception_in_recursion_form():
    h1 = test_exception.test_exception_in_recursion(3)
    return '''<h3>%s</h3>''' % h1


@app.route('/test_exception_in_Chain', methods=['GET'])
def test_exception_in_Chain():

    from pinpointPy.CommonPlugin import PinpointCommonPlugin

    @PinpointCommonPlugin("call_exp_01")
    def call_exp_01():
        raise Exception("abc")

    @PinpointCommonPlugin("call_exp_02")
    def call_exp_02():
        raise Exception("abcd")

    @PinpointCommonPlugin("main")
    def main():
        try:
            call_exp_01()
        except Exception as e:
            pass
        try:
            call_exp_02()
        except Exception as e:
            raise e
    main()
    return '''<h3>success </h3>'''


@app.route('/test_arguments', methods=['GET'])
def test_arguments_form():
    i1 = Method()
    f = open("app.py", 'r')
    l1 = [None, 123, 3.1415, True, "abc", (123, "abc"), [456, "def"], {"a": 1, "b": 2}, set(
        "abc"), i1, str(UserDefineException("Evy")), f, GREETING, lambda x: x*x]
    l2 = []
    for i in l1:
        l2.append(test_args.test_args1(i))
    f.close()
    h1 = "<br>".join('%s' % str(id) for id in l2)
    h2 = test_args.test_args2("Hello", "Evy", "a", "b",
                              "c", a="A", b="B", c="C")
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_returns', methods=['GET'])
def test_returns_form():
    i1 = Method()
    f = open("app.py", 'r')
    l1 = [None, 123, 3.1415, True, "abc", (123, "abc"), [456, "def"], {"a": 1, "b": 2}, set(
        "abc"), i1, str(UserDefineException("Evy")), f, GREETING, lambda x: x*x]
    # l1 = [[456, "def"]]
    l2 = []
    for i in l1:
        l2.append(str(type(test_returns.test_returns1(i)))[8:-2])
    f.close()
    h1 = "<br>".join('%s' % str(id) for id in l2)
    h2 = test_returns.test_returns2(
        "Hello", "Evy", "a", "b", "c", a="A", b="B", c="C")
    return '''<h3>%s</h3>
              <h3>%s</h3>''' % (h1, h2)


@app.route('/test_mysql', methods=['GET'])
def test_mysql_form():
    mysql = DBControl("dev-mysql", "root", "password", "employees")
    mysql.con_db()
    h1 = mysql.db_select("select * from employees limit 128")
    mysql.db_close()
    return "<br>".join('%s' % str(id) for id in h1)


@app.route('/test_redis', methods=['GET'])
def test_redis_form():
    r = RedisControl("redis", "6379")
    r.connection()
    h1 = r.set("name", "Evy")
    r.delete("name")
    return '''<h3>%s</h3>''' % h1


@app.route('/call_remote', methods=['GET'])
def call_remote_form():
    remote = request.args['remote']
    h1 = requests.get(remote)
    return h1.content


@app.route('/signin', methods=['GET'])
def signin_form():
    return '''<form action="/signin" method="post">
              <p><input name="username"></p>
              <p><input name="password" type="password"></p>
              <p><button type="submit">Sign In</button></p>
              </form>'''


@app.route('/signin', methods=['POST'])
def signin():
    if request.form['username'] == 'admin' and request.form['password'] == 'password':
        return '<h3>Hello, admin!</h3>'
    return '<h3>Bad username or password.</h3>'


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
