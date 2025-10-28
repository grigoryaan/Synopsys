SinglyLinkedListNode* insertNodeAtPosition(SinglyLinkedListNode* llist, int data, int position) {
    SinglyLinkedListNode* newNode = new SinglyLinkedListNode(data);
    if (position == 0) {
        newNode->next = llist;
        llist = newNode;
        return llist;
    }
    SinglyLinkedListNode* current = llist;
    for (int i = 0; i < position - 1; i++) {
        if (current != nullptr)
            current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
    return llist;

}
