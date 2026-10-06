def singleton(cls):
    instances = {}
    def get_instance(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]
    return get_instance

@singleton
class DatabaseConnection:
    def __init__(self):
        self.connected = True
        print("Connecting to DB...")  

db1 = DatabaseConnection() 
db2 = DatabaseConnection() 

print(db1 is db2)
