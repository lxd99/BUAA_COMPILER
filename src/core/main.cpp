﻿#include "header.h"
#include "symb.h"
extern vector<word> wordList;
FILE* fp;
extern word wd;
int main(){
	//fp = fopen("src/core/testfile.txt","r");
	//freopen("src/core/output.txt", "w", stdout);
	fp = fopen("testfile.txt", "r");
	freopen("error.txt", "w", stdout);
	getsym(fp);
	wd = next();
	program();
	return 0;
}
