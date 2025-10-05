int minimumSwaps(vector<int> arr) {
    int swaps = 0;
    int n = arr.size();
    int i = 0;
       while (i < n) {
        if (arr[i] != i + 1) {
            int true_index = arr[i] - 1; 
            swap(arr[i], arr[true_index]);
            swaps++;
        } else {
            i++;
        }
    }
    return swaps;

}
