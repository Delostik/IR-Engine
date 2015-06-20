#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include "lemmatizer.h"
#include "header.h"
#include "senna.h"
#include "PorterStemmer.h"

#define WINDOW_SIZE 3

#define WordListSingleDoc std::map<std::string, std::set<int, IntCmp>, StringCmp>
#define InvertList std::map<std::string, std::map<int, std::set<int, IntCmp>>, StringCmp>

const std::string non_comp[] = { "NN", "VB" };
const std::string be_word[] = { "am", "is", "are", "be", "being", "was", "were" };

class StringCmp {
public:
	bool operator () (const std::string &a, const std::string &b) const {
		return strcmp(a.c_str(), b.c_str()) < 0;
	}
};

class IntCmp {
public:
	bool operator () (const int a, const int b) const {
		return a < b;
	}
};

struct SENNA_result {
	std::string word, ner, pos;
	SENNA_result(std::string &a, std::string &b, std::string &c) :
		word(a), pos(b), ner(c) {
	}
};

class Tokenizer {
public:
	InvertList token;

	void push(std::string, int);
	void debug();
	void persistence();
	void load();
	void importDir(std::string);
private:
	std::vector<SENNA_result> window;
	Lemmatizer lemmatizer;
	PorterStemmer stemmer;
	void Tokenizer::merge(WordListSingleDoc &, int);
	WordListSingleDoc tokenizer(std::string text, int docid);
	void getFiles(std::string, std::vector<std::string>&);
	std::vector<SENNA_result> senna_result;
	std::string kgram_check();
};


#endif