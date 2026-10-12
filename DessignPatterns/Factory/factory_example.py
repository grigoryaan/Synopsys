from abc import ABC, abstractmethod


class Document(ABC):
    @abstractmethod
    def create(self):
        pass


class PDFDocument(Document):
    def create(self):
        print("Creating PDF document")


class WordDocument(Document):
    def create(self):
        print("Creating Word document")


class ExcelDocument(Document):
    def create(self):
        print("Creating Excel document")


class DocumentFactory(ABC):
    @abstractmethod
    def create_document(self):
        pass


class PDFFactory(DocumentFactory):
    def create_document(self):
        return PDFDocument()


class WordFactory(DocumentFactory):
    def create_document(self):
        return WordDocument()


class ExcelFactory(DocumentFactory):
    def create_document(self):
        return ExcelDocument()


def process_document(factory):
    document = factory.create_document()
    document.create()


process_document(PDFFactory())
process_document(WordFactory())
process_document(ExcelFactory())
