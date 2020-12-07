#define  _CRT_SECURE_NO_WARNINGS
#include "gram.h"
#include "symb.h"
#include "fcode.h"
#include "mcode.h"
extern vector<word> wordList;
FILE* fp;
extern word wd;
int main(){
	fp = fopen("testfile.txt", "r");
	freopen("mips.txt", "w", stdout);
	getsym(fp);
	wd = next();
	program();
	genFCode();
	if(FDEBUG!=1)
	outI();
	return 0;
}
