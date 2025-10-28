int findMergeNode(SinglyLinkedListNode* head1, SinglyLinkedListNode* head2) {
    SinglyLinkedListNode* p1 = head1;
    SinglyLinkedListNode* p2 = head2;
    while (p1 != p2)
    {
        if (p1 != nullptr)
            p1 = p1->next;
        else
            p1 = head2;

        if (p2 != nullptr)
            p2 = p2->next;
        else
            p2 = head1;

    }
    return p1->data;

}
