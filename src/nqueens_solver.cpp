#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int N;
ll ALL;

/* Ultra-fast DFS using bitmasking */
ll dfs(ll cols, ll d1, ll d2) {
    if (cols == ALL)
        return 1;

    ll count = 0;
    ll free = ALL & ~(cols | d1 | d2);

    while (free) {
        ll bit = free & -free;
        free -= bit;
        count += dfs(
            cols | bit,
            (d1 | bit) << 1,
            (d2 | bit) >> 1
        );
    }
    return count;
}

/* Main solver with symmetry pruning */
ll solve_n_queens_ultra(int n) {
    N = n;
    ALL = (1LL << n) - 1;

    ll total = 0;
    int half = n >> 1;

    // Symmetry pruning (first row)
    for (int i = 0; i < half; i++) {
        ll bit = 1LL << i;
        total += dfs(bit, bit << 1, bit >> 1);
    }

    total <<= 1; // multiply by 2

    // Middle column for odd N
    if (n & 1) {
        ll bit = 1LL << half;
        total += dfs(bit, bit << 1, bit >> 1);
    }

    return total;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    string input_name;

    if (argc == 2) {
        ifstream fin(argv[1]);
        if (!fin) {
            cerr << "Input file not found\n";
            return 1;
        }
        fin >> n;
        fin.close();

        input_name = filesystem::path(argv[1]).filename().string();
    } else {
        cout << "Enter N: ";
        cin >> n;
        input_name = "manual_input_N" + to_string(n) + ".txt";
    }

    filesystem::create_directory("output");
    string out_path = "output/" +
        input_name.substr(0, input_name.find(".")) + "_output.txt";

    auto start = chrono::high_resolution_clock::now();
    ll solutions = solve_n_queens_ultra(n);
    auto end = chrono::high_resolution_clock::now();

    double elapsed =
        chrono::duration<double>(end - start).count();

    ofstream fout(out_path);
    fout << n << "\n" << solutions << "\n";
    fout.close();

    cout << "\n OPTIMIZED N-QUEENS (C++)\n";
    cout << "N = " << n << "\n";
    cout << "Solutions = " << solutions << "\n";
    cout << "Time = " << fixed << setprecision(6) << elapsed << " seconds\n";
    cout << "Output = " << out_path << "\n";

    return 0;
}
