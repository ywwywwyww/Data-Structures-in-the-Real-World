#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
using namespace std;
int main() {
	freopen("vector.in", "w", stdout);
	int n = 2000;
	int m = 1000;
	int k = 1000;
	printf("100000000\n%d\n%d\n", n, m);
	srand(time(0));
	for (int i = 0; i < m; i++) {
		printf("%d\n", rand());
	}
	printf("%d\n", k);
	for (int i = 0; i < k; i++) {
		if (i == 0) {
			printf("3\n");
		} else if (rand() % 3 == 0) {
			printf("0 %d\n", rand());
		} else if (rand() % 2 == 0) {
			printf("1 %d %d\n", rand() % m, rand());
		} else {
			int u, v;
			do {
				u = rand() % m;
				v = rand() % m;
			}
			while(u == v);
			printf("2 %d %d\n", u, v);
		}
	}
	return 0;
}
