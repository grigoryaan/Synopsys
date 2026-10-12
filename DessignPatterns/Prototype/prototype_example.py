import copy


class Character:
    def __init__(self, name, health, weapon):
        self.name = name
        self.health = health
        self.weapon = weapon

    def clone(self):
        return copy.deepcopy(self)

    def show(self):
        print(f"Name: {self.name}, Health: {self.health}, Weapon: {self.weapon}")


original = Character("Warrior", 100, "Sword")

character1 = original.clone()
character1.name = "Knight"

character2 = original.clone()
character2.name = "Archer"
character2.weapon = "Bow"

original.show()
character1.show()
character2.show()
