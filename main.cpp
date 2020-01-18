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
    vector<bool> vis(n, 0);

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
                    vis[i] = vis[j] = true;
                } else {
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
    }

   if (findMinPoints) {
       for (auto it: lineCheck) {
           vis[it.se.fi] = vis[it.se.se] = true;
       }
   }


    pp pvt = {MAX_COORD + 1, MAX_COORD + 1};
    for (int i = 0; i < n; i++) {
        if (vis[i]) {
            hull.push_back(pts[i]);
            pvt = min(pvt, pts[i]);
        }
    }

    vector<pp> abv, blw, on;
    for (int i = 0; i < sz(hull); i++) {
        if (hull[i] == pvt) continue;
        if (hull[i].se > pvt.se) abv.push_back(hull[i]);
        else if (hull[i].se < pvt.se) blw.push_back(hull[i]);
        else on.push_back(hull[i]);
    }
    if (!sz(abv)) {
        abv = on;
    } else {
        for (auto it: on) {
            blw.push_back(it);
            on.clear();
        }
    }

    sort(begin(abv), end(abv), [pvt](pp a, pp b) {
        int dya = a.se - pvt.se;
        int dxa = a.fi - pvt.fi;
        int dyb = b.se - pvt.se;
        int dxb = b.fi - pvt.fi;
        int cross = (dya * dxb) - (dyb * dxa);
        if (cross == 0) { // titik a dan b segaris sama pvt
            int da = distance(pvt, a);
            int db = distance(pvt, b);
            return da < db;
        }
        return cross > 0;
    });
    sort(begin(blw), end(blw), [pvt](pp a, pp b) {
        int dya = a.se - pvt.se;
        int dxa = a.fi - pvt.fi;
        int dyb = b.se - pvt.se;
        int dxb = b.fi - pvt.fi;
        int cross = (dya * dxb) - (dyb * dxa);
        if (cross == 0) { // titik a dan b segaris sama pvt
            int da = distance(pvt, a);
            int db = distance(pvt, b);
            return da > db;
        }
        return cross > 0;
    });

    hull.clear();
    hull.push_back(pvt);
    for (auto it: abv) {
        hull.push_back(it);
    }
    for (auto it: blw) {
        hull.push_back(it);
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