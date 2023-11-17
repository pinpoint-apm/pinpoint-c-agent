from pinpointPy.libs._sqlalchemy import monkey_patch
import unittest
from pinpointPy import PinTransaction, Defines
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_sqlalchemy(self):
        import sqlalchemy as db
        from sqlalchemy import text

        engine = db.create_engine(
            'mysql+pymysql://root:password@dev-mysql/employees')
        connection = engine.connect()
        result = connection.execute(text("select * from employees limit 10"))
        for row in result:
            print(f"gender:{row.gender}")

        result = connection.execute(text("select * from employees limit 13"))
        for row in result:
            print(f"gender:{row.gender}")
        connection.close()


if __name__ == '__main__':
    unittest.main()
