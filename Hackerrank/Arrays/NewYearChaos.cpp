void minimumBribes(vector<int> q) {
    int bribes = 0;
    bool t = false;
    int n = q.size();

    for (int i = n - 1; i >= 0; i--) {
        if (q[i] - (i + 1) > 2) {
            t = true;
            break;
        }
        for (size_t j = max(0, q[i] - 2); j < i; j++) {
            if (q[j] > q[i]) {
                bribes++;
            }
        }
    }

    if (t) {
        cout << "Too chaotic" << endl;
    } else {
        cout << bribes << endl;
    }
}
