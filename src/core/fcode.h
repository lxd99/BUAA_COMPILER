#ifndef  FCODE
#define FCODE
#define FDEBUG 0
void outI();
void genFCode();
struct insLis {
	vector<string> ins;
	void push(string s) {
		ins.push_back(s);
		if (FDEBUG == 1) printf("%s\n", s.c_str());
	}
	void outI() {
		for (auto i : ins) 
			printf("%s\n", i.c_str());
	}
};
#endif // ! FCODE

