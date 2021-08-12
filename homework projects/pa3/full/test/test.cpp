#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>

void check(int result[2][2]) {
	result[0][0] = result[0][1] = result[1][0] = result[1][1] = 0;
	std::ifstream f_out("result.out");
	std::ifstream f_ans("result.ans");
	int gt, ours;
	while(f_ans >> gt) {
		f_out >> ours;
		result[gt][ours]++;
	}
}
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
void test(int k, double &time, double &fpr, double &fnr) {
	int times = 3;
	double s = 0;
	fpr = 0;
	fnr = 0;
	int result[2][2];
	for (int i = 1; i <= times; i++) {
		s += run(k);
		check(result);
		fpr = std::max(fpr, (double)result[0][1] / (result[0][1] + result[0][0]));
		fnr = std::max(fnr, (double)result[1][0] / (result[1][0] + result[1][1]));
	}
	time = s / times;
}

std::ofstream f_time("time.log");
std::ofstream f_fpr("fpr.log");
std::ofstream f_fnr("fnr.log");

void test(const char* name) {
	fprintf(stderr, "now testing %s\n", name);
	char cmd[100];
	sprintf(cmd, "cp ../flags/%s ../flags.h", name);
	system(cmd);
	system("(cd .. && make)");
	//system("make");
	//system("cd test");
	system("cp ../pa3 .");
	f_time << name << ' ';
	f_fpr << name << ' ';
	f_fnr << name << ' ';
	for (int num_threads = 1; num_threads <= 64; num_threads *= 2) {
		double time, fpr, fnr;
		test(num_threads, time, fpr, fnr);
		f_time << time << ' ';
		f_fpr << fpr << ' ';
		f_fnr << fnr << ' ';
		fprintf(stderr,"%.10f %.10f %.10f\n", time, fpr, fnr);
	}
	f_time << '\n';
	f_fpr << '\n';
	f_fnr << '\n';
}

int main() {
	system("cp ../BFdataNew/result.ans .");
	system("cp ../BFdataNew/1/data1.in data.in");
	system("cp ../flags.h backup");

	test("STD");
	test("STD_ATOMIC");
	test("BLO");
	test("BLO_ATOMIC");
	test("PAT");
	test("PAT_ATOMIC");
	test("PAT_MUTEX");

	system("rm data.in");
	system("rm result.*");
	system("rm pa3");
	system("rm time.out");
	system("mv backup ../flags.h");
	return 0;
}

