#include "optimize.h"

BlockTable blktab;
extern struct McodeList mcodlis;
static int id=0;
Mcode onextC() {
	static int len = blktab.bma.size()-1;
	int pos = 0;
	if (id <= len) {
		Block* blk = blktab.get(id);
		return blk->get(pos++);
		if (pos >= blk->size) {
			pos = 0;
			id++;
		}
	}
	else 
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
	Mcode(*p)() = onextC;
	gDeclare(p);
	gFun(p);
	gMain(p);
}
