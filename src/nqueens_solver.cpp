#include <bits/stdc++.h>
#include <filesystem>
#include <thread>

using namespace std;
namespace fs = std::filesystem;

using ull = uint64_t;

static int N;
static ull ALL;

constexpr size_t FLUSH_SIZE = 1 << 20; // 1 MB

// ------------------------------------------------------------
// Ultra-fast DFS (hot path)
// ------------------------------------------------------------
inline void dfs(
    ull cols, ull d1, ull d2, int row,
    int pos[],
    string &buffer,
    ofstream &out,
    ull &count
) {
    if (cols == ALL) {
        ++count;

        // Write solution directly
        for (int i = 0; i < N; ++i) {
            buffer.append(to_string(pos[i]));
            buffer.push_back(i + 1 == N ? '\n' : ' ');
        }

        if (buffer.size() >= FLUSH_SIZE) {
            out.write(buffer.data(), buffer.size());
            buffer.clear();
        }
        return;
    }

    ull free = ALL & ~(cols | d1 | d2);
    while (free) {
        ull bit = free & -free;
        free ^= bit;

        int col = __builtin_ctzll(bit);
        pos[row] = col + 1;

        dfs(
            cols | bit,
            (d1 | bit) << 1,
            (d2 | bit) >> 1,
            row + 1,
            pos,
            buffer,
            out,
            count
        );
    }
}

// ------------------------------------------------------------
// Worker thread
// ------------------------------------------------------------
void worker(int first_col, const string &file) {
    int pos[64];
    string buffer;
    buffer.reserve(FLUSH_SIZE * 2);

    ull count = 0;

    ull bit = 1ULL << first_col;
    pos[0] = first_col + 1;

    ofstream out(file, ios::binary);

    dfs(bit, bit << 1, bit >> 1, 1, pos, buffer, out, count);

    if (!buffer.empty())
        out.write(buffer.data(), buffer.size());

    out.close();

    ofstream cnt(file + ".count");
    cnt << count;
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 2) {
        cerr << "Usage: nqueens_solver input.txt\n";
        return 1;
    }

    ifstream fin(argv[1]);
    fin >> N;
    fin.close();

    fs::create_directories("../output");

    string base = fs::path(argv[1]).stem().string();
    string output_file = "../output/" + base + "_output.txt";

    // ===== ONLY CHANGE IS HERE =====
    if (N == 2 || N == 3) {
        ofstream out(output_file, ios::binary);
        out << N << "\n";
        out << "No Solution\n";
         cout << "No Solution\n";
        out.close();
        return 0;
    }
    // ==============================

    ALL = (1ULL << N) - 1;

    auto start = chrono::high_resolution_clock::now();

    int half = N / 2;
    vector<thread> threads;
    vector<string> files;

    threads.reserve(half);
    files.reserve(half + 1);

    // First half columns
    for (int c = 0; c < half; ++c) {
        string f = "../output/" + base + "_t_" + to_string(c) + ".txt";
        files.push_back(f);
        threads.emplace_back(worker, c, f);
    }

    for (auto &t : threads) t.join();

    // Middle column if odd
    if (N & 1) {
        string f = "../output/" + base + "_t_mid.txt";
        files.push_back(f);
        worker(half, f);
    }

    // Merge outputs
    ull total = 0;
    ofstream out(output_file, ios::binary);
    out << N << "\n";

    for (auto &f : files) {
        ull c = 0;
        ifstream cnt(f + ".count");
        cnt >> c;
        total += c;
        cnt.close();

        ifstream in(f, ios::binary);
        out << in.rdbuf();
        in.close();

        fs::remove(f);
        fs::remove(f + ".count");
    }

    // Symmetry
    total *= 2;
    if (N & 1) {
        // middle column already counted once
    }

    out.seekp(0);
    out << N << "\n" << total << "\n";
    out.close();

    auto end = chrono::high_resolution_clock::now();
    double t = chrono::duration<double>(end - start).count();

    cout << "N = " << N << "\n";
    cout << "Solutions = " << total << "\n";
    cout << "Time = " << t << " sec\n";

    return 0;
}
