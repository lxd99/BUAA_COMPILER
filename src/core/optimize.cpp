#define  _CRT_SECURE_NO_WARNINGS
#include "optimize.h"

BlockTable blktab;
extern struct McodeList mcodlis;
static int oid=0;
static int pos = 0;
Mcode onextC() {
	static int len = blktab.bma.size()-1;
	if (pos >= blktab.get(oid)->size) {
		pos = 0;
		oid++;
	}
	if (oid <= len) 
		return blktab.get(oid)->get(pos++);
	
	return Mcode(ENDOP, 0, 0);
}
void genBlkTab() {
	Block* blk = new Block;
	for (auto i : mcodlis.cList) {
		if (i.opf.isBegin) {
			if (blk->cList.size() > 0) {
				blktab.push(blk);
				blk = new Block();
			}
		}
		blk->push(i);
	}
	if (blk->cList.size() > 0)
		blktab.push(blk);
	blktab.genDAG();
}
void oGenI() {
	genBlkTab();
	if (OMDEBUG == 1)
		blktab.out();

	Mcode(*p)() = onextC;
	blktab.reGen();
	gDeclare(p);
	gFun(p);
	pos = 0;
	oid = blktab.mid;
	gMain(p);
}
