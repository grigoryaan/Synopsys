from abc import ABC, abstractmethod

class Database(ABC): 
    @abstractmethod
    def save(self, data: str): pass

class MySQLDatabase(Database):
    def save(self, data: str):
        print(f"Saving '{data}' to MySQL...")

class PostgreSQLDatabase(Database):
    def save(self, data: str):
        print(f"Saving '{data}' to PostgreSQL...")

class MockDatabase(Database): 
    def save(self, data: str):
        print(f"Mock: pretending to save '{data}'")

class UserService:
    def __init__(self, db: Database):  
        self.db = db

    def create_user(self, name: str):
        self.db.save(name)

UserService(MySQLDatabase()).create_user("Alice")
UserService(PostgreSQLDatabase()).create_user("Bob")
UserService(MockDatabase()).create_user("Test User")
