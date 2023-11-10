class Accolade:
    def __init__(self, function):
        self.function = function

    def __call__(self, *args, **kwargs):
        # Adding Excellency before name
        # name = "Excellency " + name
        self.function(*args, **kwargs)
        # Saluting after the name
        # print("Thanks " + name + " for gracing the occasion")


@Accolade
def simple_function(name):
    print(name)


class Test:
    @Accolade
    def output(self):
        print('output')


simple_function('John McKinsey')

t = Test()
t.output()
