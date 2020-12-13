#ifndef  OPTIMIZE
#include "mcode.h"
#define OPTIMIZE
#define ISARR(x) ((x).type==VARADD&&(x).cont.symb->form==ARRAYF)
#define INF 150000
struct BInf {
	int ismain = 0;
	int isfor = 0;
	int isend = 0;
};
struct Block {
	int id=-1;
	int belong = -1;
	int size = 0;
	BInf inf;
	vector<Mcode> cList;
	set <symbol*> vin, vout,vkill,vuse;
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
	struct DAG dag;
};

struct  BlockTable {
	map<int, Block*> bma;
	int mid = -1;
	//vector<Block*> bList;
	int size = 0;
	void push(Block * blk) {
		blk->id = size;
		bma.emplace(size++, blk);
		if (blk->inf.ismain)  mid = blk->id;
	}
	void genDAG() {
		for (auto i : bma) {
			Block* blk = i.second;
			blk->dag.genG(blk->cList);
		}
	}
	Block* get(int id) {
		if (id < size) return bma[id];
		else return 0;
	}
};
struct Node {
	int id;
	int op = INIOP;
	int pl = -1, pr = -1;
	Node(int mid,int mop=-1,int mpl=-1,int mpr=-1) {
		id = mid;
		op = mop;
		pl = mpl;
		pr = mpr;
	}
	Node() {

	}
	bool operator == (const Node& x) {
		return op == x.op && pl == x.pl && pr == x.pr;
	}
	bool operator <(const Node& x) {
		if (op != x.op) return op < x.op;
		if (pl != x.pl) return pl < x.pl;
		if (pr != x.pr) return pr < x.pr;
	}
};
struct numC {
	int d;
	int l, r, m;
};
struct DAG {
#define GEN(srcl,pl,pol) do{ \
	data = *mc.srcl;\
	if (dma.find(data) == dma.end())\
		genN(pos, data);\
	pl = dma.find(data)->second;\
	if (data.cont.symb->form == ARRAYF) {\
		data = *mc.srcl->off;\
		genN(pos, data);\
		pol = dma.find(data)->second;\
	}\
}while(0)

#define HDFS(pl)do{\
	if (node.pl != -1 && valid[node.pl]) {\
		if (!vis[node.pl])\
			dfs(node.pl, vis, flag, symb);\
		if (flag[node.pl]) flag[id] = 1;\
	}\
}while(0)
	map<int,Node> nma;
	map<Data, int> dma;
	set<Node> nset;
	int id=0;
	Mcode movec(Data* srcl, Data* des) {
		static string op = "move";
		Mcode mcode = Mcode(MOVEOP, des, srcl);
		mcode.genS(op + " " + srcl->getS() + " to " + des->getS());
		return mcode;
	}
	Node genN(int pos,Data data,int op=-1,int pl=-1,int pr=-1,int pol=-1,int por=-1) {
		/*无条件生成新结点*/
		Node node = Node(id,op,pl,pr);
		auto it = nset.find(node);
		if (it != nset.end()) return *it;
		nma[id++] = node;
		nset.insert(node);
		dma[data] = node.id;
		return node;
	}
	int getDN(Data *data) {
		if (data == 0||dma.find(*data)==dma.end()) return -1;
		return dma[*data];
	}
	int findData(Data data,int id,Data &rdata) {
		for (auto i : dma) {
			if (i.second == id) {
				Data mdata = i.first;
				if (mdata == data) continue;
				rdata = data;
				return 1;
			}
		}
	}
	int findGen(int pos,Mcode mc) {
		//只删除data
		int pl = -1, pr = -1,pol=-1,por=-1;
		Data data;
		if (mc.srcl) 
			GEN(srcl, pl, pol);	
		if (mc.srcr) 
			GEN(srcr, pr, por);
		data = *mc.des;
		if (ISARR(data)) 
			dma.erase(data);
		else {
			Node node = genN(pos, data, mc.op, pl, pr, pol, por);
			dma[data] = node.id;
		}
	}
	void genG(vector<Mcode>& mlist) {
		int len = mlist.size();
		for (int i = 0; i < len; i++) {
			Mcode mc = mlist[i];
			
			if (ISGENC(mc)) {
				if (mc.op == READOP) genN(i, *mc.des);
				else  findGen(i, mc);

				if (mc.des && mc.des->type == MVARADD) {
					Data mdata;
					if (findData(*mc.des, dma[*mc.des], mdata)) {
						Data* srcl = new Data;
						*srcl = mdata;
						mlist[i] = movec(srcl, mc.des);
					}
				}
			}
		}
	}
};
#endif // ! OPTIMIZE