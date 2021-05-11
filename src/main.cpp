
#include "kd_tree.hpp"
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <fstream>

using namespace std;

const dim_type POINT_D = 2;

struct point {

	float dim[POINT_D];

	point() {
	}

	point(float x, float y) {
		dim[0] = x;
		dim[1] = y;
	}
};

ostream& operator<<(ostream& os, const point& p) {
    os << std::setprecision(4) << "(" << p.dim[0] << ", " << p.dim[1] << ")";
    return os;
}

string to_string(point const& p) {
    std::ostringstream ss;
    ss << p;
    return ss.str();
}

struct dim_access {
	float operator()(const point &p, dim_type dim) const {
		return p.dim[dim];
	}
};

struct Distance {
	float operator()(const point &a, const point &b) const {
		float ab_x = b.dim[0] - a.dim[0]; 
		float ab_y = b.dim[1] - a.dim[1];
		return sqrt(ab_x * ab_x + ab_y * ab_y);
	}
};

float random_float() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

int main() {
	kd_tree<point, POINT_D, dim_access> tree;

	point p;

	p = point(7.0f, 2.0f);
	cout << "insert " << p << ": " << tree.insert(p) << "\n";
	cout << tree << endl;

	cout << "\n";
	p = point(5.0f, 4.0f);
	cout << "insert " << p << ": " << tree.insert(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    p = point(9.0f, 6.0f);
	cout << "insert " << p << ": " << tree.insert(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    p = point(2.0f, 3.0f);
	cout << "insert " << p << ": " << tree.insert(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    p = point(4.0f, 7.0f);
	cout << "insert " << p << ": " << tree.insert(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    p = point(8.0f, 1.0f);
	cout << "insert " << p << ": " << tree.insert(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    cout << "insert " << p << ": " << tree.insert(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    cout << "contains " << p << ": " << tree.contains(p) << "\n";

    cout << "\n";
    cout << "erase " << p << ": " << tree.erase(p) << "\n";
    cout << tree << endl;

    cout << "\n";
    cout << "contains " << p << ": " << tree.contains(p) << "\n";

    cout << "\n";
    for (dim_type i = 0; i < POINT_D; ++i) {
		cout << "min " << static_cast<int>(i) << ": " << *tree.min(i) << "\n";
	}

    cout << "\n";
    point a(2.0f, 3.0f);
	point b(9.0f, 6.0f);
	cout << "range " << a << ", " << b << ": ";
	vector<point const *> range_res;
	tree.range(range_res, a, b);
	for (vector<point const *>::size_type i = 0; i < range_res.size(); ++i) {
		cout << *range_res[i];
		if (i < range_res.size() - 1) {
			cout << ", ";
		}
	}
	cout << "\n";

    cout << "\n";
    p = point(1.0f, 1.0f);
	cout << "nerares neighbor " << p << ": " << *tree.nearest_neighbor<Distance>(p) << "\n";

    cout << "\n";
    cout << "clear\n";
	tree.clear();
    cout << tree << endl;

    cout << "\n";
    cout << "empty: " << tree.empty() << "\n";

    unsigned int n_points = 1000;
    vector<point> random_points;
    random_points.resize(n_points);
    {
        cout << "\n";

        string insert_filename("insert.dat");
        cout << "Inserting " << n_points << " random points. Saving results on " << insert_filename << "\n";
        ofstream file;
        file.open (insert_filename);
        if (!file.is_open()) {
            cout << "Could not open file " << insert_filename << endl;
            return 1;
        }
        for (unsigned int i = 0; i < n_points;) {
            random_points[i].dim[0] = random_float();
            random_points[i].dim[1] = random_float();
            auto time_start = chrono::steady_clock::now();
            bool res = tree.insert(random_points[i]);
            auto time_end = chrono::steady_clock::now();
            if (res) {
                file << i << "," << (chrono::duration_cast<chrono::nanoseconds>(time_end - time_start).count() / 1e+3) << "\n";
                ++i;
            }
        }
        file.close();
    }
    {
        cout << "\n";
        string insert_filename("contains.dat");
        cout << "Searching (contains) " << n_points << " random points. Saving results on " << insert_filename << "\n";
        ofstream file;
        file.open(insert_filename);
        if (!file.is_open()) {
            cout << "Could not open file " << insert_filename << endl;
            return 1;
        }
        for (unsigned int i = 0; i < n_points; ++i) {
            p = point(random_float(), random_float());
            auto time_start = chrono::steady_clock::now();
            tree.contains(random_points[i]);
            auto time_end = chrono::steady_clock::now();
            file << i << "," << (chrono::duration_cast<chrono::nanoseconds>(time_end - time_start).count() / 1e+3) << "\n";
        }
        file.close();
    }

    cout << flush;

    return 0;
}