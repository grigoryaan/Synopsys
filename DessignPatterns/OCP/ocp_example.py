from abc import ABC, abstractmethod

class NotificationChannel(ABC):

    @abstractmethod
    def send(self, message: str):
        pass

class EmailChannel(NotificationChannel):
    def send(self, message: str):
        print(f"Sending email: {message}")

class SMSChannel(NotificationChannel):
    def send(self, message: str):
        print(f"Sending SMS: {message}")

class PushChannel(NotificationChannel): # new channel 

    def send(self, message: str):
        print(f"Sending push: {message}")

class NotificationService:

    def __init__(self, channel: NotificationChannel):
        self.channel = channel

    def notify(self, message: str):
        self.channel.send(message)

service = NotificationService(EmailChannel())
service.notify("Hello!")

service = NotificationService(PushChannel())
service.notify("Hello!")
