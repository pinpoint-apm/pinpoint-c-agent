#/bin/bash

(mysql -uroot -ppassword -hdev-mysql <db.sql)
(cd test_db && mysql -uroot -ppassword -hdev-mysql < employees.sql)
