from pinpointPy.libs._MySQLdb import monkey_patch
import unittest
from pinpointPy import PinTransaction
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_MySQLdb(self):
        import MySQLdb
        db = MySQLdb.connect(password="password",
                             database="employees",
                             user='root',
                             host='dev-mysql')
        c = db.cursor()

        sql = "select first_name from employees where `emp_no`=(%s)"
        c.execute(sql, (10003,))
        c.fetchone()

        db.close()


if __name__ == '__main__':
    unittest.main()
