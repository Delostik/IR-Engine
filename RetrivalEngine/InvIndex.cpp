#include "InvIndex.h"

InvIndex::InvIndex() {
	this->load();
}

void InvIndex::load() {
	std::ifstream fin("invIndex.dat");
	if (!fin) {
		std::cout << "Invert Index doesn't exist!" << std::endl;
		return;
	}
	std::cout << "Read Invert Index from disk... " << std::endl;
	this->t_index.clear();
	std::string word;
	int df, pos, docid, tf;
	std::map<int, std::set<int, IntCmp>> doc;

	while (fin >> word >> df) {
		++wordCnt;
		word2num.insert(std::make_pair(word, wordCnt));
		num2word.insert(std::make_pair(wordCnt, word));

		doc.clear();

		for (auto i = 0; i < df; ++i) {
			std::set<int, IntCmp> docItem;
			fin >> docid >> tf;
			docList.insert(docid);
			for (auto j = 0; j < tf; j++) {
				fin >> pos;
				docItem.insert(pos);
			}
			doc[docid] = docItem;
		}
		t_index[wordCnt] = doc;
	}
	IDF_FACTOR = docList.size();
	std::cout << "DONE. " << word2num.size() << " words been processed." << std::endl << std::endl;
	this->pre_calc();
	this->createDocVec();
	this->createSoundMap();
}

void InvIndex::pre_calc() {
	std::cout << "Calculate tf-idf properties..." << std::endl;
	for (auto &x : t_index) {
		std::map<int, DocItem, IntCmp> tmp_list;
		for (auto &y : x.second) {
			tmp_list[y.first].pos = y.second;
			tmp_list[y.first].tf = y.second.size();
			tmp_list[y.first].tf_idf = 0;
		}
		WordItem word;
		word.idf = log10((double)IDF_FACTOR / (double)x.second.size());
		word.list = tmp_list;
		index[x.first] = word;
		for (auto &y : index[x.first].list) {
			y.second.tf_idf = (1.0 + log2(y.second.tf)) * word.idf;
			auto iter = docModule.find(x.first);
			if (iter == docModule.end()) {
				docModule[x.first] = (y.second.tf_idf)*(y.second.tf_idf);
			}
			else {
				docModule[x.first] += (y.second.tf_idf)*(y.second.tf_idf);
			}
		}
	}

	InvertIndexOrigin().swap(t_index);
	for (auto &x : docModule) {
		x.second = sqrt(x.second);
	}
	std::cout << "DONE" << std::endl << std::endl;
}

void InvIndex::debug() {
	std::cout << "size= " << IDF_FACTOR << std::endl;
	for (auto &x : index) {
		std::cout << x.first << "  idf=" << x.second.idf << std::endl;
		for (auto &y : x.second.list) {
			std::cout << "----docid=" << y.first << std::endl;
			std::cout << "tf=" << y.second.tf << "  tf_idf=" << y.second.tf_idf << std::endl;
		}
	}
}

void InvIndex::createDocVec() {
	std::cout << "Creating Doc Vector..." << std::endl;

	for (auto & x : index) {
		for (auto & y : docList) {
			auto iter = x.second.list.find(y);
			if (iter != x.second.list.end()) {
				docVec[y].insert(std::make_pair(x.first, iter->second.tf_idf));
			}
		}
	}

	std::cout << std::endl;
	std::cout << "DONE" << std::endl << std::endl;
}

void InvIndex::createSoundMap() {
	std::cout << "Creating Sound Map..." << std::endl;
	Soundex soundex;
	for (auto &x : word2num) {
		std::string code = soundex.encode(x.first);
		soundMap[code].push_back(x.second);
	}
	std::cout << "DONE." << std::endl << std::endl;
}