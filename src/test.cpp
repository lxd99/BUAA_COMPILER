#include<cstdio>
#include<string>
#include<cstdlib>

using  namespace std;
void duck(string ss) {
	printf("%s\n", ss.c_str());
}
int _main() {
	string a = "4646";
	duck(a);
	duck("123");
	if (a >= "4645") printf("yes\n");
	else printf("no\n");
	return 0;
}