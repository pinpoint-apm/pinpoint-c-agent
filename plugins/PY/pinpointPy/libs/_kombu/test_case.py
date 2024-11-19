

from pinpointPy.libs._kombu import monkey_patch
import unittest
from pinpointPy import PinTransaction, Defines
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_kombu_amqp(self):
        # https://github.com/celery/kombu?tab=readme-ov-file#quick-overview
        from kombu import Connection, Exchange, Queue

        media_exchange = Exchange('media', 'direct', durable=True)
        video_queue = Queue(
            'video', exchange=media_exchange, routing_key='video')

        def process_media(body, message):
            print(body)
            message.ack()

        # connections
        with Connection('amqp://user:password@rabbit-mq:5672//') as conn:

            # produce
            producer = conn.Producer(serializer='json')
            producer.publish({'name': '/tmp/lolcat1.avi', 'size': 1301013},
                             exchange=media_exchange, routing_key='video',
                             declare=[video_queue])


if __name__ == '__main__':
    unittest.main()
