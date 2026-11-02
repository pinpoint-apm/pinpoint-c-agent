"""Regression coverage for native argument handling and trace ownership."""

import unittest

import _pinpointPy as agent


class TestNativeRegression(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        agent.set_agent("tcp:127.0.0.1:1", -1, -1)

    def tearDown(self):
        agent.force_flush_trace(-1)

    def setUp(self):
        # Older tests may leave the thread-local ID referring to a closed tree.
        agent.force_flush_trace(-1)
        root = agent.start_trace(0)
        if root > 0:
            agent.force_flush_trace(-1, root)

    def test_default_trace_can_restart_after_force_flush(self):
        root = agent.start_trace()
        self.assertGreater(root, 0)
        agent.drop_trace(root)
        self.assertTrue(agent.force_flush_trace(-1))
        self.assertFalse(agent.trace_has_root(root))

        next_root = agent.start_trace()
        self.assertGreater(next_root, 0)
        agent.drop_trace(next_root)

    def test_explicit_root_flush_clears_its_thread_local_child(self):
        root = agent.start_trace()
        child = agent.start_trace()
        self.assertGreater(child, 0)
        agent.drop_trace(root)
        self.assertTrue(agent.force_flush_trace(-1, root))
        next_root = agent.start_trace()
        self.assertGreater(next_root, 0)
        agent.drop_trace(next_root)

    def test_flushing_another_tree_preserves_current_trace(self):
        current = agent.start_trace()
        other = agent.start_trace(0)
        agent.drop_trace(other)
        self.assertTrue(agent.force_flush_trace(-1, other))
        self.assertTrue(agent.is_root_trace(current))
        child = agent.start_trace()
        self.assertGreater(child, 0)
        self.assertEqual(current, agent.end_trace())
        agent.drop_trace(current)

    def test_invalid_flush_reports_failure(self):
        self.assertFalse(agent.force_flush_trace(-1, -2))

    def test_invalid_arguments_preserve_type_error(self):
        cases = [
            ("add_clue", (123, "value")),
            ("add_clues", ("key", None)),
            ("set_context_key", (123, "value")),
            ("get_context_key", (123,)),
            ("get_sequence_id", ("invalid",)),
            ("set_async_ctx", (1, 2, "invalid")),
            ("is_root_trace", ("invalid",)),
            ("trace_has_root", ("invalid",)),
            ("mark_as_error", ("message", "file", "invalid")),
            ("add_exception", (None,)),
        ]
        for name, args in cases:
            with self.subTest(method=name):
                with self.assertRaises(TypeError):
                    getattr(agent, name)(*args)


if __name__ == "__main__":
    unittest.main()
