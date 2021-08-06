#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;

double a[100][10];
double run(int k) {
	char str[100];
	sprintf(str, "./pa3 %d", k);
	system(str);
	freopen("time.out", "r", stdin);
	double s;
	scanf("%lf",  &s);
	fclose(stdin);
	return s;
}
double test(int k) {
	int times = 5;
	double s = 0;
	for (int i = 1; i <= times; i++) {
		s += run(k);
	}
	return s / times;
}
int main() {
	system("cp ../pa3 .");
	system("cp ../BFdataNew/result.ans .");
	char str[100];
	for (int num_threads = 1; num_threads <= 32; num_threads *= 2) {
		sprintf(str, "cp ../BFdataNew/%d/data*.in .", num_threads);
		system(str);
		if (num_threads == 1) {
			a[num_threads][0] = test(-1);
			a[num_threads][1] = test(0);
		}
		a[num_threads][2] = test(num_threads);
		a[num_threads][3] = test(num_threads + 100);
		a[num_threads][4] = test(num_threads + 200);
	}
	system("rm data*.in");
	system("rm result.*");
	system("rm pa3");
	system("rm time.out");
	freopen("result.log", "w", stdout);
	for (int i = 1; i <= 32; i *= 2) {
		for (int j = 0; j < 5; j++) {
			printf("%.3f ", a[i][j]);
		}
		printf("\n");
	}
	return 0;
}

