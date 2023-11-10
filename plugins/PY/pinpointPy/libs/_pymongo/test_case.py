from pinpointPy.libs._pymongo import monkey_patch
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
        # reference from  https://pymongo.readthedocs.io/en/stable/tutorial.html#
        from pymongo import MongoClient
        import datetime
        client = MongoClient("mongodb://mongodb:27017/")

        db_name = 'pinpoint-test'
        db = client[db_name]
        posts = db.posts
        post = {
            "author": "Mike",
            "text": "My first blog post!",
            "tags": ["mongodb", "python", "pymongo"],
            "date": datetime.datetime.now(tz=datetime.timezone.utc),
        }
        post_id = posts.insert_one(post).inserted_id
        print(post_id)
        posts.find_one()
        posts.find_one({"author": "Mike"})
        posts.find_one({"author": "Eliot"})
        new_posts = [
            {
                "author": "Mike",
                "text": "Another post!",
                "tags": ["bulk", "insert"],
                "date": datetime.datetime(2009, 11, 12, 11, 14),
            },
            {
                "author": "Eliot",
                "title": "MongoDB is fun",
                "text": "and pretty easy too!",
                "date": datetime.datetime(2009, 11, 10, 10, 45),
            },
        ]
        ret = posts.insert_many(new_posts)
        print(ret)
        client.close()


if __name__ == '__main__':
    unittest.main()
