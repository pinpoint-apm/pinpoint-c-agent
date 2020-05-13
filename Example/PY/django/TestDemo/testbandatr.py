
class Person(object):
    def __init__(self ,newName ,newAge):
        self.name = newName
        self.age = newAge

laowang = Person("laowang" ,20)
print(laowang.name)
print(laowang.age)
laowang.addr = "test"
print(laowang .addr)

evy = Person("Evy", 30)
print(evy.addr)