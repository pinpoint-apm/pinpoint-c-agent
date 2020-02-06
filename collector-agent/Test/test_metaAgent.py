import unittest

from Common.Logger import set_logger_level, logger_enable_console
from Events.GTimer import GTimer
from CollectorAgent.GrpcMeta import GrpcMeta
from gevent import time
class TestMetaAgent(unittest.TestCase):
    def setUp(self) -> None:
        set_logger_level("DEBUG")
        logger_enable_console()
        starttime = str(int(time.time() * 1000))
        self.agent_meta =[('agentid', 'test-id'),
                  ('applicationname', 'test-name'),
                  ('starttime',starttime)]

    def test_once_timer(self):
        meta = GrpcMeta('dev-pinpoint:9992',self.agent_meta)
        meta.start()
        time.sleep(1)
        meta.stop()
