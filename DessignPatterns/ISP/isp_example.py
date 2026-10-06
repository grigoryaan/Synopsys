from abc import ABC, abstractmethod

class Workable(ABC):
    @abstractmethod
    def work(self): pass

class Eatable(ABC):
    @abstractmethod
    def eat(self): pass

class Sleepable(ABC):
    @abstractmethod
    def sleep(self): pass

class Human(Workable, Eatable, Sleepable):  
    def work(self):
        print("Human working...")

    def eat(self):
        print("Human eating...")

    def sleep(self):
        print("Human sleeping...")

class Robot(Workable):  
    def work(self):
        print("Robot working...")

def make_work(worker: Workable):
    worker.work()

make_work(Human())  
make_work(Robot())  
