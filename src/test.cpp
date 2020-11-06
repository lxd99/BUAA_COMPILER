#define  _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include<string>
#include<cstdlib>
#include<iostream>
#include<vector>

using  namespace std;
#define STRTINT(s,n) \
	do{\
		for(auto i : (s)) \
			(n) = (n) * 10 + i - '0';\
	}while(0);
void duck(int a,int b){}
int _main() {
	string ss = "123456";
	int n = 0;
	STRTINT(ss, n);
	printf("%d\n", n);
	return 0;
}