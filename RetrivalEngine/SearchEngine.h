#ifndef _SEARCHENGINE_H_
#define _SEARCHENGINE_H_

#include "header.h"
#include "PorterStemmer.h"
#include "senna.h"
#include "soundex.h"

#define WINDOW_SIZE 3


const std::string non_comp[] = { "NN", "VB" };
const std::string be_word[] = { "am", "is", "are", "be", "being", "was", "were" };

struct SENNA_result {
	std::string word, ner, pos;
	SENNA_result(std::string a, std::string b, std::string c) :
		word(a), pos(b), ner(c) {
	}
};

class SearchEngine {
public:
	void search(std::string);
	std::vector<int> result;

private:

	bool tokenizer();

	int resCnt;

	std::vector<SENNA_result> window;
	std::string key;
	PorterStemmer stemmer;
	std::vector<SENNA_result> senna_result;
	std::string kgram_check();

	std::set<std::string, StringCmp> tokens;


	int position_search();
	int token_search();

	Soundex soundex;

};



#endif