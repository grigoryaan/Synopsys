class User:

    def __init__(self, name: str, email: str):
        self.name = name
        self.email = email

    def get_user_info(self) -> str:
        return f"{self.name} ({self.email})"

class UserRepository:

    def save(self, user: User):
        print(f"Saving {user.name} to database...")

    def find_by_email(self, email: str) -> User | None:
        print(f"Looking up user by {email}...")
        return None

class EmailService:

    def send_welcome_email(self, user: User):
        print(f"Sending welcome email to {user.email}...")

user = User("Alice", "alice@example.com")
repo = UserRepository()
mailer = EmailService()

repo.save(user)
mailer.send_welcome_email(user)
