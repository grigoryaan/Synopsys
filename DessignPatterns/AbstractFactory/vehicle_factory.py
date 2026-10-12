from abc import ABC, abstractmethod


class Car(ABC):
    @abstractmethod
    def drive(self):
        pass


class Motorcycle(ABC):
    @abstractmethod
    def ride(self):
        pass


class ToyotaCar(Car):
    def drive(self):
        print("Driving a Toyota car")


class ToyotaMotorcycle(Motorcycle):
    def ride(self):
        print("Riding a Toyota motorcycle")


class HondaCar(Car):
    def drive(self):
        print("Driving a Honda car")


class HondaMotorcycle(Motorcycle):
    def ride(self):
        print("Riding a Honda motorcycle")


class VehicleFactory(ABC):
    @abstractmethod
    def create_car(self):
        pass

    @abstractmethod
    def create_motorcycle(self):
        pass


class ToyotaFactory(VehicleFactory):
    def create_car(self):
        return ToyotaCar()

    def create_motorcycle(self):
        return ToyotaMotorcycle()


class HondaFactory(VehicleFactory):
    def create_car(self):
        return HondaCar()

    def create_motorcycle(self):
        return HondaMotorcycle()


def test_vehicles(factory):
    car = factory.create_car()
    motorcycle = factory.create_motorcycle()

    car.drive()
    motorcycle.ride()


print("Toyota factory:")
test_vehicles(ToyotaFactory())

print("---")

print("Honda factory:")
test_vehicles(HondaFactory())
