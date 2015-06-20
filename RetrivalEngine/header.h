#ifndef _HEADER_H_
#define _HEADER_H_

#include <iostream>
#include <vector>
#include <string>
#include <hash_map>
#include <utility>
#include <algorithm>
#include <queue>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <sstream>

#include <WinSock2.h>
#pragma comment(lib,"ws2_32")

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <io.h>  
#include <direct.h>  
#include <stdlib.h>
#include <ctype.h>

class StringCmp {
public:
	bool operator () (const std::string a, const std::string b) const {
		return strcmp(a.c_str(), b.c_str()) < 0;
	}
};

class IntCmp {
public:
	bool operator () (const int a, const int b) const {
		return a < b;
	}
};

struct DocItem {
	int tf;
	double tf_idf;
	std::set<int, IntCmp> pos;
};

struct WordItem {
	double idf;
	std::map<int, DocItem, IntCmp> list;
};

class greater {
public:
	bool operator() (int a, int b) { return a> b; }
};


extern std::map<int, std::map<int, double, IntCmp>> docVec;
extern std::set<int> docList;
extern std::map<int, double> docModule;
extern std::map<std::string, int, StringCmp> word2num;
extern std::map<int, std::string, IntCmp> num2word;

extern std::map<int, WordItem, IntCmp> index;

extern int wordCnt;

extern std::map<std::string, std::vector<int>, StringCmp> soundMap;

extern char sendBuf[65535];
extern SOCKADDR_IN addrClient;
extern std::ostringstream ostr;


#endif