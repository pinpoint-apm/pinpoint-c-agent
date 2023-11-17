from datetime import datetime, date, timedelta
import mysql.connector
from pinpointPy.libs._MysqlConnector import monkey_patch
import unittest
from pinpointPy import PinTransaction
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_str_url(self):
        import mysql.connector
        cnx = mysql.connector.connect(
            user='root',
            password='password',
            host='dev-mysql',
            database='employees')
        cnx.close()

    @PinTransaction("testcase", GenTestHeader())
    def test_query(self):
        # reference from https://dev.mysql.com/doc/connector-python/en/connector-python-tutorial-cursorbuffered.html
        from decimal import Decimal
        # Connect with the MySQL Server
        cnx = mysql.connector.connect(
            user='root',
            password='password',
            host='dev-mysql',
            database='employees')

        # Get two buffered cursors
        curA = cnx.cursor(buffered=True)
        curB = cnx.cursor(buffered=True)

        # Query to get employees who joined in a period defined by two dates
        query = (
            "SELECT s.emp_no, salary, from_date, to_date FROM employees AS e "
            "LEFT JOIN salaries AS s USING (emp_no) "
            "WHERE to_date = DATE('9999-01-01')"
            "AND e.hire_date BETWEEN DATE(%s) AND DATE(%s)")

        # UPDATE and INSERT statements for the old and new salary
        update_old_salary = (
            "UPDATE salaries SET to_date = %s "
            "WHERE emp_no = %s AND from_date = %s")
        insert_new_salary = (
            "INSERT INTO salaries (emp_no, from_date, to_date, salary) "
            "VALUES (%s, %s, %s, %s)")

        # Select the employees getting a raise
        curA.execute(query, (date(2000, 1, 1), date(2000, 12, 31)))
        print(curA)
        self.assertTrue(curA._pinpoint_)
        now = datetime.now()
        tomorrow = now + timedelta(1)
        # Iterate through the result of curA
        for (emp_no, salary, from_date, to_date) in curA:
            print(f'{emp_no} {salary} {from_date} {to_date}')
            # Update the old and insert the new salary
            new_salary = int(round(salary * Decimal('1.15')))
            curB.execute(update_old_salary, (tomorrow, emp_no, from_date))
            self.assertTrue(curB._pinpoint_)
            # curB.execute(insert_new_salary,
            #  (emp_no, tomorrow, date(9999, 1, 1,), new_salary))
            # Commit the changes
            cnx.commit()

        cnx.close()


if __name__ == '__main__':
    unittest.main()
