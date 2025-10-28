DoublyLinkedListNode* sortedInsert(DoublyLinkedListNode* llist, int data) {
    DoublyLinkedListNode* newNode = new DoublyLinkedListNode(data);

    if (!llist) return newNode;

    if (data <= llist->data) {
        newNode->next = llist;
        llist->prev = newNode;
        return newNode;
    }

    DoublyLinkedListNode* current = llist;
    while (current->next && current->data < data) {
        current = current->next;
    }

    if (current->data >= data) {
        newNode->next = current;
        newNode->prev = current->prev;
        if (current->prev)
            current->prev->next = newNode;
        current->prev = newNode;
    }
    else {
        current->next = newNode;
        newNode->prev = current;
    }

    return llist;
}
