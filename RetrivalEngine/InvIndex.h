#ifndef _INVINDEX_H_
#define _INVINDEX_H_

#include "header.h"
#include "soundex.h"

#define InvertIndexOrigin std::map<int, std::map<int, std::set<int, IntCmp>>, IntCmp>
#define InvertIndex std::map<int, WordItem, IntCmp>



extern int IDF_FACTOR;



class InvIndex {
public:
	InvIndex();
	

	void load();

	void debug();
	
private:
	InvertIndexOrigin t_index;
	void pre_calc();
	void createSoundMap();
	void createDocVec();
};


#endif