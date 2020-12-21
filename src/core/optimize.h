#include "mcode.h"
#ifndef  OPTIMIZE
#define OPTIMIZE 
#define ISARR(x) ((x).type==VARADD&&(x).cont.symb->form==ARRAYF)
#define INF 150000
#define GEN(src,p) do{ \
	if (dma.find(*mc.src) == dma.end())\
		genN(mc.src);\
	p = dma.find(*mc.src)->second;\
	if (mc.src->type==VARADD&&mc.src->cont.symb->form == ARRAYF&&\
	dma.find(*mc.src->off)==dma.end()) {\
		genN(mc.src->off);\
	}\
}while(0)
#define ISGENC(x) ((x).op==NEGOP||(x).op==ADDOP||(x).op==SUBOP \
	||(x).op==MULOP||(x).op==DIVOP||(x).op==READOP||(x).op==LOADOP||\
	(x).op==SLLOP||(x).op==SRLOP||(x).op==ASSIGNOP||((x).op==CALLOP&&(x).des)||(x).op==CASEOP)
#define HDFS(pl)do{\
	if (node.pl != -1 && valid[node.pl]) {\
		if (!vis[node.pl])\
			dfs(node.pl, vis, flag, symb);\
		if (flag[node.pl]) flag[id] = 1;\
	}\
}while(0)
#define ISUSE(src) (mc.src&&mc.src->type == VARADD && mc.src->cont.symb->form == ARRAYF && \
mc.src->off && mc.src->off->type == MVARADD)
struct BInf {
	int ismain = 0;
	int isfor = 0;
	int isend = 0;
};
struct Node {
	int id;
	int op = INIOP;
	int pl = -1, pr = -1;
	int repid= -1;
	Node(int mid, int mop = -1, int mpl = -1,int mpr=-1) {
		id = mid;
		op = mop;
		pl = mpl;
		pr = mpr;
	}
	Node() {

	}
	bool operator == (const Node& x) const{
		return op == x.op && pl == x.pl && pr == x.pr;
	}
	bool operator <(const Node& x) const{
		if (op != x.op) return op < x.op;
		else if (op == -1||op == READOP || op==CALLOP) {
			return id<x.id;
		}
		if (pl != x.pl) return pl < x.pl;
		return pr < x.pr;
		//return cont < x.cont;
	}
};
struct DAG {
	map<int, Node> nma;
	map<Data, int> dma;
	set<Node> nset;
	vector<Mcode> nlist;
	int id = 0,repid=0;
	Mcode movec(Data* srcl, Data* des) {
		static string op = "move";
		Mcode mcode = Mcode(MOVEOP, des, srcl);
		mcode.genS(op + " " + srcl->getS() + " to " + des->getS());
		return mcode;
	}
	void regenS(Mcode &mcode) {
		static map<int, string> jsma = {
			{JNEOP,"!="},{JEQOP,"=="},
			{JGEOP,">="},{JLTOP,"<"},
			{JGTOP,">"},{JLEOP,"<="}
		};
		Data* srcl = mcode.srcl, * srcr = mcode.srcr, * des = mcode.des;
		if (mcode.op == ADDOP)
			mcode.genS(des->getS() + "=" + srcl->getS() + "+" + srcr->getS());
		if (mcode.op == SUBOP)
			mcode.genS(des->getS() + "=" + srcl->getS() + "-" + srcr->getS());
		if (mcode.op == MULOP) \
			mcode.genS(des->getS() + "=" + srcl->getS() + "*" + srcr->getS());
		if (mcode.op == DIVOP) \
			mcode.genS(des->getS() + "=" + srcl->getS() + "/" + srcr->getS());
		if (mcode.op == NEGOP) \
			mcode.genS(des->getS() + "=" + "-" + srcl->getS()); \
			if (mcode.op == LOADOP) \
				mcode.genS(string("load") + " " + des->getS() + " from " + srcl->getS());
		if (mcode.op == SLLOP) \
			mcode.genS(des->getS() + " = " + srcl->getS() + " sll " + srcr->getS());
		if (mcode.op == SRLOP) \
			mcode.genS(des->getS() + " = " + srcl->getS() + " srl " + srcr->getS());
		if (mcode.op == PRINTOP)
			mcode.genS("print " + srcl->getS());
		if (jsma.find(mcode.op) != jsma.end()) {
			mcode.genS(srcl->getS() + " " + jsma[mcode.op] + " " + srcr->getS() + " goto " + des->getS());
		}
		if (mcode.op == CASEOP)
			mcode.genS(srcl->getS() + " != " + srcr->getS() + " goto " + des->getS());
		if (mcode.op == PUSHOP)
			mcode.genS("push " + srcl->getS());
		if (mcode.op == LOADOP)
			mcode.genS("load " + des->getS() + " from " + srcl->getS());
		if (mcode.op == RETOP) {
			if (srcl) mcode.genS("ret " + srcl->getS());
			else mcode.genS("ret null");
		}
		if (mcode.op == ASSIGNOP)
			mcode.genS(des->getS() + "=" + srcl->getS());
		if (mcode.op == CALLOP) {
			symbol* symb = srcl->cont.symb;
			if(symb->type != VTYPE)
			mcode.genS(des->getS() + "= " + "call " + srcl->getS());
			else 
				mcode.genS("call " + srcl->getS());
		}
			
	}
	int genN(Data *&data, int op = -1, int pl = -1, int pr = -1) {
		/*
		非ini结点:normal
		ini结点: 直接生成
		*/
		Node node = Node(id, op, pl, pr);
		auto it = nset.find(node);
		if (it != nset.end()) {
			if (data->type == MVARADD)
				data->cont.varid = it->repid;
			return -1;
		}
			
		if (data->type == MVARADD) {
			node.repid = repid++;
			if (op == -1) {
				Data *ndata = new  Data;
				*ndata = *data;
				ndata->cont.varid = node.repid;
				nlist.push_back(movec(data, ndata));
				data = ndata;

			}
			else {
				data->cont.varid = node.repid;
			}
		}		
		nma[id++] = node;
		nset.insert(node);
		dma[*data] = node.id;
		return node.id;
	}
	int findData(int id, Data& rdata) {
		//寻找附加于此id的data
		for (auto i : dma) {
			if (i.second == id) {
				Data mdata = i.first;
				rdata = mdata;
				return 1;
			}
		}
		return 0;
	}
	int findMNode(Mcode mc) {
		//寻找mc对应中间结点
		int pl=-1, pr=-1;
		if (mc.srcl && dma.find(*mc.srcl)!=dma.end()) 
			pl = dma[*mc.srcl];
		if (mc.srcr && dma.find(*mc.srcr) != dma.end())
			pr = dma[*mc.srcr];
		auto it = nset.find(Node(id,mc.op, pl, pr));
		if (it != nset.end()&&it->op!=-1) {
			return it->id;
		}
		return -1;		
	}
	int findGen(Mcode &mc) {
		int pl = -1, pr = -1;
		Data data;
		//GEN会生成INI结点
		/*printf("%s\n", mc.s.c_str());*/
		if (mc.srcl)
			GEN(srcl, pl);
		if (mc.srcr)
			GEN(srcr, pr);
		if (!mc.des || (mc.des->type!=MVARADD && mc.des->type!=VARADD)||
			genN(mc.des, mc.op, pl, pr)!=-1)
			return 0;
		
		return -1;	
	}
	void chLab(Data data,int pos,int id) {
		for (int i = pos; i < nlist.size(); i++) {
			Mcode mc = nlist[i];
			if (mc.op == MOVEOP) continue;
			if (mc.srcl && *mc.srcl == data) 
				mc.srcl->cont.varid = id;	
			if (mc.srcr && *mc.srcr == data)
				mc.srcr->cont.varid = id;
			regenS(mc);
			nlist[i] = mc;
		}
			
	}
	void reLabel() {
		vector<int> fir, las,flag;
		map<int, set<int>> gen, free;
		set<int> nlab;
		Mcode mc;
		for (int i = 0; i < nlist.size(); i++) {
			mc = nlist[i];
			if (mc.des && mc.des->type == MVARADD) {
				fir.push_back(i);
				las.push_back(-1);
				flag.push_back(-1);
			}
			if (mc.op == MOVEOP) continue;
			if (mc.srcl && mc.srcl->type == MVARADD)
				las[mc.srcl->cont.varid] = i;
			if (mc.srcr && mc.srcr->type == MVARADD)
				las[mc.srcr->cont.varid] = i;
			if (ISUSE(srcl)) 
				las[mc.srcl->off->cont.varid] = i;
			if(ISUSE(srcr))
				las[mc.srcr->off->cont.varid] = i;
			if (ISUSE(des))
				las[mc.des->off->cont.varid] = i;
		}

		for (int i = 0; i < repid; i++) {
			gen.emplace(fir[i], set<int>{ i });
			if (free.find(las[i]) == free.end())
				free.emplace(las[i],set<int>{i});
			else free[las[i]].insert(i);
			nlab.insert(i);
		}
		for (int i = 0; i < nlist.size(); i++) {
			mc = nlist[i];
			auto x = gen.find(i);
			auto y = free.find(i);
			if (x != gen.end()) {
				flag[mc.des->cont.varid]=*nlab.begin();
				chLab(*mc.des,i+1, *nlab.begin());
				mc.des->cont.varid = *nlab.begin();
				nlab.erase(nlab.begin());
			}
			if (y != free.end()) {
				for (auto i : y->second) {
					nlab.insert(flag[i]);
					flag[i] = -1;
				}
			}
			regenS(mc);
			nlist[i] = mc;
			/*printf("%s\n", mc.s.c_str());*/
		}
		/*printf("\n------------\n");*/
	}
	void genG(vector<Mcode>& mlist) {
		int len = mlist.size();
		Mcode mc;
		for (int i = 0; i < len; i++) {
			mc =  mlist[i];
			int isadd = 0;
			if (ISGENC(mc)) {
				if (mc.op == READOP) genN(mc.des,mc.op);
				else if (mc.op == CALLOP) {
					genN(mc.des, mc.op);
					for (auto it = dma.begin(); it != dma.end();) {
						if (it->first.type == VARADD)
							dma.erase(it++);
						else it++;
					}
				}
				else if (mc.op == ASSIGNOP) genN(mc.des, mc.op);
				else 
					isadd = findGen(mc);
				regenS(mc);
			/*	printf("[%s]\n", mc.s.c_str());*/
			}
			if (isadd >= 0) {
				regenS(mc);
				nlist.push_back(mc);
			}	
		}
		reLabel();
		mlist = nlist;
	}
};
struct Block {
	int id=-1;
	int belong = -1;
	int size = 0;
	BInf inf;
	vector<Mcode> cList;
	set <symbol*> vin, vout,vkill,vuse;
	set <symbol*> din, dout,dkill,dgen;
	void push(Mcode mcode) {
		cList.push_back(mcode);
		size++;
		if (mcode.op==DECLAREOP&&
			mcode.srcl->cont.symb->wd.ss == "main") {
			inf.ismain = 1;
		}
	}
	Mcode get(int id) {
		if (id < size) return cList[id];
		else return Mcode(ENDOP,0,0);
	}
	void out() {
		for (auto i : cList)
			printf("%s\n",i.s.c_str());
	}
	struct DAG dag;
};

struct  BlockTable {
	map<int, Block*> bma;
	int mid = -1;
	//vector<Block*> bList;
	int size = 0;
	void push(Block * blk) {
		blk->id = size++;
		bma.emplace(blk->id, blk);
		if (blk->inf.ismain)  mid = blk->id;
	}
	void genDAG() {
		int debugid = 0;
		for (auto i : bma) {
			Block* blk = i.second;
			blk->dag.genG(blk->cList);
			blk->size = blk->cList.size();
			debugid++;
		}
	}
	Block* get(int id) {
		if (id < size) return bma[id];
		else return 0;
	}
	void reGen() {
		symbol* symb =0;
		for (auto i : bma) {
			Block blk = *i.second;
			for (auto j : blk.cList) {
				if (j.op == DECLAREOP) {
					symb = j.srcl->cont.symb;
					symb->mVar = 0;
				}
				if (j.des && j.des->type == MVARADD && symb) {
					symb->mVar = max(symb->mVar, j.des->cont.varid);
				}
			}
		}
	}
	void out(){
		for (auto i : bma) {
			for (auto j : i.second->cList) {
				printf("%s\n", j.s.c_str());
			}
			printf("\n---------------\n");
		}
	}
};
struct numC {
	int d;
	int l, r, m;
};
void oGenI();
#endif // ! OPTIMIZE