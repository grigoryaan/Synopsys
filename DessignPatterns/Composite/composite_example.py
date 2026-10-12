from abc import ABC, abstractmethod


class OrganizationComponent(ABC):
    @abstractmethod
    def show(self):
        pass


class Employee(OrganizationComponent):
    def __init__(self, name):
        self.name = name

    def show(self):
        print(f"Employee: {self.name}")


class Department(OrganizationComponent):
    def __init__(self, name):
        self.name = name
        self.components = []

    def add(self, component):
        self.components.append(component)

    def show(self):
        print(f"Department: {self.name}")
        for component in self.components:
            component.show()


employee1 = Employee("Anna")
employee2 = Employee("David")
employee3 = Employee("Alen")

it_department = Department("IT")
it_department.add(employee1)
it_department.add(employee2)

company = Department("Main Company")
company.add(it_department)
company.add(employee3)

company.show()
