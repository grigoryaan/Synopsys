from abc import ABC, abstractmethod

class Weapon(ABC):
    @abstractmethod
    def attack(self) -> str:
        pass

    @abstractmethod
    def damage(self) -> int:
        pass

class Sword(Weapon):
    def attack(self) -> str:
        return "Slash!"

    def damage(self) -> int:
        return 50

class Bow(Weapon):
    def attack(self) -> str:
        return "Twang"

    def damage(self) -> int:
        return 30

class Grenade(Weapon): 
    def attack(self) -> str:
        return "BOOM!"

    def damage(self) -> int:
        return 120

class Character:
    def __init__(self, name: str, weapon: Weapon):
        self.name = name
        self.weapon = weapon

    def fight(self):
        print(f"{self.name}: {self.weapon.attack()} (deals {self.weapon.damage()} dmg)")

Character("Knight", Sword()).fight()    # Knight: Slash! (deals 50 dmg)
Character("Archer", Bow()).fight()      # Archer: Twang! (deals 30 dmg)
Character("Soldier", Grenade()).fight() # Soldier: BOOM! (deals 120 dmg)
