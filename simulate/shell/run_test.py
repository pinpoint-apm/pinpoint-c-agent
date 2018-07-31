# Copyright 2018 NAVER Corp.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from php_simulate_test import PHPSimulateTest
import sys


def main(conf_path):
    php_test = PHPSimulateTest(conf_path)
    php_test.test()


def usage():
    print "python run_test.py conf_path"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        usage()
        exit(-1)

    main(sys.argv[1])
