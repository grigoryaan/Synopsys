class Laptop:
    def __init__(self, brand, processor, ram, operating_system):
        self.brand = brand
        self.processor = processor
        self.ram = ram
        self.operating_system = operating_system

    def show_info(self):
        print(f"Brand: {self.brand}")
        print(f"Processor: {self.processor}")
        print(f"RAM: {self.ram}")
        print(f"Operating System: {self.operating_system}")
        print("-" * 30)


class LaptopBuilder:
    def __init__(self):
        self.brand = "Unknown"
        self.processor = "Intel Core i5"
        self.ram = "8GB"
        self.operating_system = "Windows 11"

    def set_brand(self, brand):
        self.brand = brand
        return self

    def set_processor(self, processor):
        self.processor = processor
        return self

    def set_ram(self, ram):
        self.ram = ram
        return self

    def set_operating_system(self, operating_system):
        self.operating_system = operating_system
        return self

    def build(self):
        return Laptop(
            self.brand,
            self.processor,
            self.ram,
            self.operating_system
        )


laptop1 = (
    LaptopBuilder()
    .set_brand("Lenovo")
    .set_processor("Intel Core i7")
    .set_ram("16GB")
    .set_operating_system("Windows 11")
    .build()
)

laptop2 = (
    LaptopBuilder()
    .set_brand("Apple")
    .set_processor("Apple M4")
    .set_ram("24GB")
    .set_operating_system("macOS")
    .build()
)

laptop1.show_info()
laptop2.show_info()

