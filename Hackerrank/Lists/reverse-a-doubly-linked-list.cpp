DoublyLinkedListNode* reverse(DoublyLinkedListNode* llist) {
    if (llist == nullptr) return nullptr;

    DoublyLinkedListNode* current = llist;
    DoublyLinkedListNode* temp = nullptr;

    while (current != nullptr) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;

        current = current->prev;
    }


    if (temp != nullptr)
        llist = temp->prev;

    return llist;
}
