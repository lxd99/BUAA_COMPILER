#define  _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include<string>
#include<cstdlib>
#include<iostream>
#include<vector>

using  namespace std;
void duck(int a, int b = 5) {
	printf("%d\n", a + b);
}
int _main() {
	vector<int> vec[3] = { {0,1} };
	//printf("%d %d %d\n", vec[0].size(), vec[1].size(), vec[2].size());
	duck(6);
	return 0;
}