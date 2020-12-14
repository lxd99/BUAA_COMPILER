#define  _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include<string>
#include<cstdlib>
#include<iostream>
#include<vector>
#define INIT -5
using  namespace std;
#define STRTINT(s,n) \
	do{\
		for(auto i : (s)) \
			(n) = (n) * 10 + i - '0';\
	}while(0);
void duck(int a,int b){}
int _main() {
	string a[2] = { "sdf","sdfsd" };
	string b[2];
	a[0] = "dsdsf";
	b[1] = a[0];
	printf("%s", b[1].c_str());
	return 0;
}