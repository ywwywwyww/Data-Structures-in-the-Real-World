#include <cstdio>
#include <string>
#include <iostream>
#include <unordered_set>
#include <cstdlib>
using namespace std;

unordered_set<string> s;

int ans[30000010];

int main(int argc, char **argv) {
	int k = atoi(argv[1]);
	freopen("result.ans", "w", stdout);
	ios::sync_with_stdio(false);
	int n = 0, ni;
	string op, str;
	int time;
	ios::sync_with_stdio(false);
	for (int j = 1; j <= k; j++) {
		char filename[100];
		sprintf(filename, "data%d.in", j);
		freopen(filename, "r", stdin);
		cin >> ni;
		n += ni;
		for (int i = 1; i <= ni; i++) {
			cin >> op >> str >> time;
			if (op[0] == 'i') {
				s.insert(str);
				ans[time] = -1;
			} else {
				ans[time] = s.count(str);
			}
		}
	}
	for (int i = 1; i <= n; i++) {
		if (ans[i] >= 0) {
			std::cout << ans[i] << '\n';
		}
	}
	std::cerr << s.size() << std::endl;
	return 0;
}

