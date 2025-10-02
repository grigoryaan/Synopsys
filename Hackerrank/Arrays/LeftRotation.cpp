vector<int> rotLeft(vector<int> a, int d) {
int n = a.size();

    vector<int> result(n);

    for (int i = 0; i < n; i++) {
        result[i] = a[(i + d) % n];
    }

    return result;
}
