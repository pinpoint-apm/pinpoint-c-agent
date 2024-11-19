

from pinpointPy.libs._rabbitmq import monkey_patch
import unittest
from pinpointPy import PinTransaction, Defines
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_amqp(self):
        import amqp
        with amqp.Connection(
            'rabbit-mq', exchange='test_exchange', userid='user', password='password',
            confirm_publish=True,
        ) as c:
            ch = c.channel()
            ch.basic_publish(amqp.Message('Hello World'),
                             routing_key='test', exchange="")


if __name__ == '__main__':
    unittest.main()
