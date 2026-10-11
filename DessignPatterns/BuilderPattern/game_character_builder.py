class GameCharacter:
    def __init__(self, name, character_class, weapon, health):
        self.name = name
        self.character_class = character_class
        self.weapon = weapon
        self.health = health

    def show_info(self):
        print(f"Name: {self.name}")
        print(f"Class: {self.character_class}")
        print(f"Weapon: {self.weapon}")
        print(f"Health: {self.health}")
        print("-" * 25)


class GameCharacterBuilder:
    def __init__(self):
        self.name = "Unknown"
        self.character_class = "Warrior"
        self.weapon = "Basic Sword"
        self.health = 100

    def set_name(self, name):
        self.name = name
        return self

    def set_class(self, character_class):
        self.character_class = character_class
        return self

    def set_weapon(self, weapon):
        self.weapon = weapon
        return self

    def set_health(self, health):
        self.health = health
        return self

    def build(self):
        return GameCharacter(
            self.name,
            self.character_class,
            self.weapon,
            self.health
        )


character1 = (
    GameCharacterBuilder()
    .set_name("Aragorn")
    .set_class("Warrior")
    .set_weapon("Sword")
    .set_health(150)
    .build()
)

character2 = (
    GameCharacterBuilder()
    .set_name("Legolas")
    .set_class("Archer")
    .set_weapon("Bow")
    .set_health(100)
    .build()
)

character1.show_info()
character2.show_info()
