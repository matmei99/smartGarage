class Status:

    def __init__(self):
        self.openGarage = True
        self.value = 3

    def to_json(self):
        return self.__dict__
