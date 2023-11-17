from pinpointPy.libs._PyMysql import monkey_patch
import unittest
from pinpointPy import PinTransaction
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_pymysql(self):
        # reference from  https://github.com/PyMySQL/PyMySQL/blob/main/docs/source/user/examples.rst#examples
        import pymysql.cursors

        # Connect to the database
        connection = pymysql.connect(host='dev-mysql',
                                     user='root',
                                     password='password',
                                     database='employees',
                                     charset='utf8mb4',
                                     cursorclass=pymysql.cursors.DictCursor)
        with connection:
            with connection.cursor() as cursor:
                # Create a new record
                sql = "select first_name from employees where `emp_no`=(%s)"
                cursor.execute(sql, ('10003'))

            # connection is not autocommit by default. So you must commit to save
            # your changes.
            connection.commit()

            with connection.cursor() as cursor:
                # Read a single record
                sql = "SELECT count(*) from employees  WHERE `gender`=%s"
                cursor.execute(sql, ('F'))
                result = cursor.fetchone()
                print(result)


if __name__ == '__main__':
    unittest.main()
