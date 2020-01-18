#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include "dependencies/matplotlibcpp.h"

namespace plt = matplotlibcpp;
using namespace std;

#define fi first
#define se second
#define sz(a) (int)a.size()

using pp = pair<int, int>;
using LINE = pair<pp, int>;

const int MAX_COORD = 10;

int n;
vector<pp> pts;
vector<pp> hull;
LINE refLine;

void generatePoints() {
    auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 eng;
    eng.seed(seed);
    uniform_int_distribution<int> range(0, MAX_COORD);
    map<pp, bool> vis;
    for (int i = 0; i < n; i++) {
        pp cur = {range(eng), range(eng)};
        while (vis[cur]) cur = {range(eng), range(eng)};
        vis[cur] = true;
        pts.push_back(cur);
    }
}

void inputPoints() {
    cout << '\n' << "Masukkan " << n << " titik yang berbeda." << '\n';
    cout << "Koordinat harus merupakan bilangan bulat (integer)" << '\n';

    for (int i = 0; i < n; i++) {
        pp p;
        cout << "Titik " << i + 1 << ": ";
        cin >> p.fi >> p.se;
        pts.push_back(p);
    }
}

void showPoints() {
    for (int i = 0; i < n; i++) {
        cout << i + 1 << ". " << "(" << pts[i].fi << ", " << pts[i].se << ")" << '\n';
    }
}

void showHull() {
    for (int i = 0; i < sz(hull); i++) {
        cout << i + 1 << ". " << "(" << hull[i].fi << ", " << hull[i].se << ")" << '\n';
    }
}

LINE makeLine(pp p1, pp p2) {
    // buat garis dari p1 ke p2
    // bentuk ax + by = c
    int a = p2.se - p1.se;
    int b = p1.fi - p2.fi;
    int c = a * p1.fi + b * p1.se;
    int gc = __gcd(a, b);
    gc = __gcd(gc, c);
    if (gc != 0) {
        a /= gc;
        b /= gc;
        c /= gc;
    }
    // agar persamaan memiliki bentuk paling sederhana
    return {{a, b}, c};
}

int sideLine(pp p) {
    // sisi point p terhadap garis
    int cc = refLine.fi.fi * p.fi + refLine.fi.se * p.se;
    int c = refLine.se;
    if (cc == c) return 0;
    else if (cc > c) return 1;
    else return -1;
}

int distance(pp a, pp b) {
    int ret = pow(a.fi - b.fi, 2) + pow(a.se - b.se, 2);
    return ret;
}

void findHull(bool findMinPoints) {
    map<LINE, pair<int, int>> lineCheck; // nyimpan dua titik terjauh untuk sebuah garis
    vector<vector<int>> adj(n);

    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            refLine = makeLine(pts[i], pts[j]);
            bool left = false, right = false;
            bool can = true;
            for (int k = 0; k < n && can; k++) {
                if (k == i || k == j) continue;
                int side = sideLine(pts[k]);
                if (side < 0) left = true;
                if (side > 0) right = true;
                if (left && right) {
                    can = false;
                }
            }
            if (can) {
                if (!findMinPoints) {
                    adj[i].push_back(j);
                    adj[j].push_back(i);
                }
                
                if (lineCheck.count(refLine) == 0) {
                    lineCheck[refLine] = {i, j};
                } else {
                    int cur = distance(pts[i], pts[j]);
                    pair<int, int> store = lineCheck[refLine];
                    int tm = distance(pts[store.fi], pts[store.se]);
                    if (cur > tm) {
                        lineCheck[refLine] = {i, j};
                    }
                }

            }
        }
    }

    // buat adjacency list
    int cur = -1; // indeks urutan pertama titik di hull, cari yang paling kiri
    if (findMinPoints) {
        for (auto it: lineCheck) {
            if (cur == -1) cur = it.se.fi;
            if (pts[it.se.fi] < pts[cur]) cur = it.se.fi;
            if (pts[it.se.se] < pts[cur]) cur = it.se.se;

            adj[it.se.fi].push_back(it.se.se);
            adj[it.se.se].push_back(it.se.fi);
        }
    } else {
        vector<vector<int>> tadj(n);
        for (auto it: lineCheck) {
            LINE curLine = it.fi;
            int u = it.se.fi;
            vector<int> tm;
            tm.push_back(u);
            for (auto v: adj[u]) {
                if (makeLine(pts[u], pts[v]) == curLine) tm.push_back(v);
            }
            sort(begin(tm), end(tm), [u](int a, int b) {
                return distance(pts[u], pts[a]) < distance(pts[u], pts[b]);
            });
            for (int i = 0; i < sz(tm) - 1; i++) {
                if (cur == -1) cur = tm[i];
                if (pts[tm[i]] < pts[cur]) cur = tm[i];
                if (pts[tm[i + 1]] < pts[cur]) cur = tm[i + 1];

                tadj[tm[i]].push_back(tm[i + 1]);
                tadj[tm[i + 1]].push_back(tm[i]);
            }
        }
        adj = tadj;
    }

    bool end = false;
    vector<bool> vis(n, 0);
    if (sz(adj[cur]) == 2) {
        pp a = pts[adj[cur][0]], b = pts[adj[cur][1]];
        pp ref = pts[cur];
        int dxa = ref.fi - a.fi;
        int dya = ref.se - a.se;
        int dxb = ref.fi - b.fi;
        int dyb = ref.se - b.se;
        if (dya * dxb < dyb * dxa) swap(adj[cur][0], adj[cur][1]); // proses gradien yang lebih tinggi biar urutan clockwise
    }

    while (!end) { // proses urutan
        hull.push_back(pts[cur]);
        vis[cur] = true;
        end = true;
        for (int i = 0; i < sz(adj[cur]) && end; i++) {
            if (!vis[adj[cur][i]]) {
                cur = adj[cur][i];
                end = false;
            }
        }
    }
}


vector<int> x, y;
void split(vector<pp> v) {
    // buat matplotlib
    x.clear();
    y.clear();
    for (int i = 0; i < sz(v); i++) {
        x.push_back(v[i].fi);
        y.push_back(v[i].se);
    }
}

int main() {
    cout << "========== TUCIL STIMA 1 ==========" << '\n';
    cout << "===== BRUTE-FORCE CONVEX HULL =====" << '\n';
    cout << "==========  Michel Fang  ==========" << '\n';
    cout << "==========    13518137   ==========" << '\n';

    cout << '\n' << "Masukkan N (banyaknya titik): ";
    cin >> n;

    cout << "1. Bangkitkan titik secara acak" << '\n';
    cout << "2. Masukkan titik sendiri" << '\n'; // buat debug/hardcode
    int opt;
    cout << "Masukkan pilihan: ";
    cin >> opt;
    if (opt == 1) {
        generatePoints();
    } else {
        inputPoints();
    }

    cout << '\n' << "TITIK-TITIK YANG DIGUNAKAN:" << '\n';
    showPoints();

    bool findMinPoints = false;
    cout << '\n' << "Cari Convex Hull dengan titik minimal?" << '\n';
    cout << " (Y) : Jika ada 3 titik (atau lebih) dalam satu garis, maka titik yang diambil sebagai himpunan convex hull adalah 2 titik terjauh" << '\n';
    cout << " (N) : Jika ada 3 titik (atau lebih) dalam satu garis di convex hull, ambil semuanya" << '\n';
    cout << "Masukkan Pilihan (Y/N): ";
    char c;
    cin >> c;
    findMinPoints = (c == 'Y');

    // time start
    clock_t start = clock();
    findHull(findMinPoints);
    double timeTaken = (clock()-start)*1./CLOCKS_PER_SEC;
    // time end

    cout << '\n' << "HULL:" << '\n';
    showHull();

    // biar lucu
    // plt::xkcd();

    // grid
    plt::grid(true);

    // [color][marker][line]
    split(pts);
    plt::named_plot("Points", x, y, "ko");

    // plot garis
    hull.push_back(hull[0]);
    for (int i = 0; i < sz(hull) - 1; i++) {
        vector<int> tx = {hull[i].fi, hull[i + 1].fi}, ty = {hull[i].se, hull[i + 1].se};
        plt::plot(tx, ty, "b,:");
        string s = "P";
        s += to_string(i + 1);
        plt::text(hull[i].fi, hull[i].se, s);
    }

    split(hull);
    plt::named_plot("Hull", x, y, "bD");

    string graphTitle = "N = " + to_string(n) + ", " + "Waktu = " + to_string(timeTaken) + " s";
    plt::title(graphTitle);
    plt::legend();
    plt::show();
    return 0;
}