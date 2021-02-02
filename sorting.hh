/*
C++17 template implementations of sorting algorithms.
*/


#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;


template <typename T>
void print_vector(vector<T> s) {
    for_each(s.begin(), s.end(), [](const auto& e) {cout << e << ' ';});
    cout << endl;
}

template <typename T>
void bubble_sort_asc_sink_right(vector<T>& s) {
    /*
        Bubble sort. Ascending order.
        Each loop on i brings the largest number to the end.
    */
    cout << "Input: ";
    print_vector(s);
    auto N = s.size();
    bool complete = false;  // optimization flag
    for (int i = 0; i < N-1 && complete==false; ++i) {
        complete = true;
        for (int j = 0; j < N-1-i; ++j)
            if (s[j] > s[j+1]) {
                std::swap(s[j], s[j+1]);
                complete = false;
            }
        print_vector(s);
    }
}


template <typename T>
void bubble_sort_desc_float_right(vector<T>& s) {
    /*
        Bubble sort. Descending order.
        Each loop on i brings the smallest number to the end.
    */
    cout << "Input: ";
    print_vector(s);
    auto N = s.size();
    bool complete = false;  // optimization flag
    for (int i = 0; i < N-1 && complete==false; ++i) {
        complete = true;
        for (int j = 0; j < N-1-i; ++j)
            if (s[j] < s[j+1]) {
                std::swap(s[j], s[j+1]);
                complete = false;
            }
        print_vector(s);
    }
}


template <typename T>
void bubble_sort_asc_float_left(vector<T>& s) {
    /*
        Bubble sort. Ascending order.
        Each loop on i brings the smallest number to the beginning.
    */
    cout << "Input: ";
    print_vector(s);
    auto N = s.size();
    bool complete = false;  // optimization flag
    for (int i = N-1; i > 0 && complete==false; --i) {
        complete = true;
        for (int j = N-1; j > N-1-i; --j)
            if (s[j] < s[j-1]) {
                std::swap(s[j], s[j-1]);
                complete = false;
            }
        print_vector(s);
    }
}

template <typename T>
void bubble_sort_desc_sink_left(vector<T>& s) {
    /*
        Bubble sort. Decending order.
        Each loop on i brings the largest number to the beginning.
    */
    cout << "Input: ";
    print_vector(s);
    auto N = s.size();
    bool complete = false;  // optimization flag
    for (int i = N-1; i > 0 && complete==false; --i) {
        complete = true;
        for (int j = N-1; j > N-1-i; --j)
            if (s[j] > s[j-1]) {
                std::swap(s[j], s[j-1]);
                complete = false;
            }
        print_vector(s);
    }
}

template <typename T>
void insertion_sort(vector<T>& s) {
/*
    CRLS 3ed Insertion-Sort.
*/
    cout << "Input: ";
    print_vector(s);
    if (s.size() < 2) return;
    for (int j = 1; j < s.size(); ++j) {
        auto key = s[j];
        // Insert s[j] into the sorted sequence s[0..j-1]
        int i = j - 1;
        while (i >= 0 && s[i] > key) {
            s[i+1] = s[i];
            --i;
        }
        s[i+1] = key;
        print_vector(s);
    }
}

template <typename T>
void merge(vector<T>& s, int p, int q, int r) {
    /*
    ** CRLS 3ed Merge(A, p, q, r). 2.3, p31.
    ** q, r is different and 1-pass the two setment ends.
    */
    // copy out two segments
    vector<T> L, R;
    for (int i = p; i < q; ++i)
        L.push_back(s[i]);
    for (int i = q; i < r; ++i)
        R.push_back(s[i]);

    // merge back
    auto n1 = q - p;
    auto n2 = r - q;
    int i = 0;
    int j = 0;
    for (int k = p; k < r; ++k) {
        if (j == n2 || (i < n1 && L[i] <= R[j])) {
            s[k] = L[i];
            ++i;
        }
        else {
            s[k] = R[j];
            ++j;
        }
    }
    
}

template <typename T>
void merge_sort(vector<T>& s, int p, int r) {
    /*
    ** CRLS 3ed MERGE-SORT(A, p, r). 2.3, p34.
    ** r is different and 1-pass to the end.
     */
    if (p >= r - 1) { // base case
        cout << p << ", " << r << ": ";
        print_vector(s);
        return;
    }

    auto q = (p + r) / 2;  // divide
    merge_sort(s, p, q);  // conquer 1
    merge_sort(s, q, r);  // conquer 2
    merge(s, p, q, r);
    cout << p << ", " << r << ": ";
    print_vector(s);
}
