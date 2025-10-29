#include <iostream>
#include <vector>

void mergeSort(std::vector<int> &arr) {
    if (arr.size() <= 1) return;

    int mid = arr.size() / 2;
    std::vector<int> left(arr.begin(), arr.begin() + mid);
    std::vector<int> right(arr.begin() + mid, arr.end());

    mergeSort(left);
    mergeSort(right);

    int i = 0, j = 0, k = 0;
    while (i < (int)left.size() && j < (int)right.size()) {
        if (left[i] < right[j])
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];
    }
    while (i < (int)left.size()) arr[k++] = left[i++];
    while (j < (int)right.size()) arr[k++] = right[j++];
}

int main() {
    std::vector<int> arr = {38, 27, 43, 3, 9, 82, 10};
    mergeSort(arr);
    for (int i = 0; i < (int)arr.size(); i++)
        std::cout << arr[i] << " ";
    std::cout << "\n";
}
