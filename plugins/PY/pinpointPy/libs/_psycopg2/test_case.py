import psycopg2.extensions
import logging
from pinpointPy.libs._psycopg2 import monkey_patch
import unittest
from pinpointPy import PinTransaction
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_case(self):
        # reference from  https://www.psycopg.org/docs/usage.html
        import psycopg2
        conn = psycopg2.connect(
            dbname="test", user="test", password="pinpoint", host="postgres", port=5432)
        cur = conn.cursor()
        cur.execute(
            "CREATE TABLE test (id serial PRIMARY KEY, num integer, data varchar);")
        cur.execute("INSERT INTO test (num, data) VALUES (%s, %s)",
                    (100, "abc'def"))
        cur.execute("SELECT * FROM test;")
        resp = cur.fetchone()
        conn.commit()
        cur.close()
        conn.close()

        # import psycopg2

        # class LoggingCursor(psycopg2.extensions.cursor):
        #     def execute(self, sql, args=None):
        #         logger = logging.getLogger('sql_debug')
        #         logger.info(self.mogrify(sql, args))

        #         try:
        #             print(f"start sql: {sql} {self.connection}")
        #             psycopg2.extensions.cursor.execute(self, sql, args)
        #             print(f"end sql: {sql}")
        #         except Exception as exc:
        #             logger.error("%s: %s" % (exc.__class__.__name__, exc))
        #             raise

        # conn = psycopg2.connect(
        #     dbname="test", user="test", password="pinpoint", host="10.34.130.156", port=5432, cursor_factory=LoggingCursor)
        # cur = conn.cursor()
        # # cur.execute("INSERT INTO mytable VALUES (%s, %s, %s);",
        # # (10, 20, 30))
        # cur.execute("DROP TABLE test")
        # cur.execute(
        #     "CREATE TABLE test (id serial PRIMARY KEY, num integer, data varchar);")
        # cur.execute("INSERT INTO test (num, data) VALUES (%s, %s)",
        #             (100, "abc'def"))
        # cur.execute("SELECT * FROM test;")
        # resp = cur.fetchone()
        # conn.commit()
        # cur.close()
        # conn.close()


if __name__ == '__main__':
    unittest.main()
