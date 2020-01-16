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

using pp = pair<double, double>;

const double MAX_COORD = 10;

int n;
vector<pp> points;
vector<pp> hull;
pair<pp, double> refLine;

void generatePoints() {
    auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 eng;
    eng.seed(seed);
    uniform_real_distribution<double> range(-MAX_COORD, MAX_COORD);
    for (double i = 0; i < n; i++) {
        points.push_back({range(eng), range(eng)});
    }
}

void inputPoints() {
    for (int i = 0; i < n; i++) {
        pp p;
        cout << "Titik " << i + 1 << ": ";
        cin >> p.fi >> p.se;
        points.push_back(p);
    }
}

void showPoints() {
    for (int i = 0; i < n; i++) {
        cout << i + 1 << ". " << "(" << points[i].fi << ", " << points[i].se << ")" << '\n';
    }
}

void showHull() {
    for (int i = 0; i < sz(hull); i++) {
        cout << i + 1 << ". " << "(" << hull[i].fi << ", " << hull[i].se << ")" << '\n';
    }
}
void makerefLine(pp p1, pp p2) {
    // refLine p1 to p2
    double a = p2.se - p1.se;
    double b = p1.fi - p2.fi;
    double c = a * p1.fi + b * p1.se;
    refLine = {{a, b}, c};
}

int sideLine(pp p) {
    double cc = refLine.fi.fi * p.fi + refLine.fi.se * p.se;
    double c = refLine.se;
    if (cc == c) return 0;
    else if (cc > c) return 1;
    else return -1;
}

double distance(pp a, pp b) {
    double ret = pow(a.fi - b.fi, 2) + pow(a.se - b.se, 2);
    ret = sqrt(ret);
    return ret;
}

void findHull() {
    if (n <= 2) {
        hull = points;
        return;
    }
    
    map<pair<pp, double>, pair<int, int>> lineCheck; // ensure to take two furthest points for a line equation
    vector<vector<int>> adj(n);
    for (double i = 0; i < n; i++) {
        for (double j = i + 1; j < n; j++) {
            makerefLine(points[i], points[j]);
            bool left = false, right = false;
            bool can = true;
            for (double k = 0; k < n && can; k++) {
                if (k == i || k == j) continue;
                double side = sideLine(points[k]);
                if (side < 0) left = true;
                if (side > 0) right = true;
                if (left && right) {
                    can = false;
                }
            }
            if (can) {
                if (lineCheck.count(refLine) == 0) {
                    lineCheck[refLine] = {i, j};
                } else {
                    double cur = distance(points[i], points[j]);
                    pair<int, int> store = lineCheck[refLine];
                    double tm = distance(points[store.fi], points[store.se]);
                    if (cur > tm) {
                        lineCheck[refLine] = {i, j};
                    }
                }
            }
        }
    }

    // get order of the hull
    int cur = -1;
    for (auto it: lineCheck) {
        if (cur == -1) cur = it.se.fi;
        adj[it.se.fi].push_back(it.se.se);
        adj[it.se.se].push_back(it.se.fi);
    }

    vector<bool> vis(n, 0);
    bool end = false;
    while (!end) {
        hull.push_back(points[cur]);
        vis[cur] = true;
        end = true;
        for (int i = 0; i < 2 && end; i++) {
            if (!vis[adj[cur][i]]) {
                cur = adj[cur][i];
                end = false;
            }
        }
    }
}

vector<double> x, y;

void split(vector<pp> v) {
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
    cout << "Masukkan N (banyaknya titik): ";
    cin >> n;

    cout << "1. Bangkitkan titik secara acak" << '\n';
    cout << "2. Masukkan titik sendiri" << '\n';
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

    // time start
    clock_t start = clock();
    findHull();
    double timeTaken = (clock()-start)*1./CLOCKS_PER_SEC;
    // time end

    cout << '\n' << "HULL:" << '\n';
    showHull();

    // [color][marker][line]
    split(points);
    plt::named_plot("Points", x, y, "bo");

    split(hull);
    plt::named_plot("Hull", x, y, "gD");

    hull.push_back(hull[0]);
    for (int i = 0; i < sz(hull) - 1; i++) {
        // cout << i + 1 << " " << grad(hull[0], hull[i]) << '\n';
        vector<double> tx = {hull[i].fi, hull[i + 1].fi}, ty = {hull[i].se, hull[i + 1].se};
        plt::plot(tx, ty, "g.--");
        string s = "P";
        s += to_string(i + 1);
        plt::text(hull[i].fi, hull[i].se, s);
    }

    string graphTitle = "N = " + to_string(n) + ", " + "Time taken = " + to_string(timeTaken) + " s";
    plt::title(graphTitle);
    plt::legend();

    plt::show();
    return 0;
}