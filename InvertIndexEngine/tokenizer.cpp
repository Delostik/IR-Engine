#include "tokenizer.h"


void Tokenizer::push(std::string text, int docid) {
	WordListSingleDoc tmp = this->tokenizer(text, docid);
	this->merge(tmp, docid);
}

void Tokenizer::merge(WordListSingleDoc &temp_token, int docid) {
	for (auto &x : temp_token) {
		auto iter = token.find(x.first);
		if (iter != this->token.end()) {
			iter->second[docid] = x.second;
		}
		else {
			this->token[x.first] = [](int docid, std::set<int, IntCmp> pos) ->std::map<int, std::set<int, IntCmp>> {
				std::map<int, std::set<int, IntCmp>> ret;
				ret[docid] = pos;
				return ret;
			}(docid, x.second);
		}
	}
}

WordListSingleDoc Tokenizer::tokenizer(std::string text, int docid) {
	WordListSingleDoc ret;
	int tail;
	int pos_index(0);

	senna(text.c_str());
	senna_result.clear();
	std::ifstream fin("pos_ner.dat");
	std::string word, pos, ner;
	std::string multiword = "";
	int multiword_index = 0;

	while (fin >> word >> pos >> ner) {
		stemmer.trim(word);
		stemmer.stem(word);
		senna_result.push_back(SENNA_result(word, pos, ner));
		++pos_index;
		if (ner.size() > 1 && ner[1] == '-') {
			if (ner[0] == 'B') {
				multiword = word;
				multiword_index = pos_index;
			}
			else if (ner[0] == 'I') {
				multiword = multiword + "_" + word;
			}
			else if (ner[0] == 'E') {
				multiword = multiword + "_" + word;
				auto iter = ret.find(multiword);
				if (iter != ret.end()) {
					ret[multiword].insert(multiword_index);
				}
				else {
					ret[multiword].insert(multiword_index);
				}
			}
		}

		// check Part of Speech result
		if (word.size() <= 1 && !(word[0] >= 'a' && word[0] <= 'z')) continue;
		
		auto iter = ret.find(word);

		if (iter != ret.end()) {
			ret[word].insert(pos_index);
		}
		else {
			ret[word].insert(pos_index);
		}
	}
	fin.close();
	
	// check k-gram words
	pos_index = 1;

	for (auto i = 0; i < senna_result.size(); ++i) {
		if (window.size() < WINDOW_SIZE) window.push_back(senna_result[i]);
		else {
			window.erase(window.begin());
			window.push_back(senna_result[i]);
			pos_index++;
		}
		std::string s(this->kgram_check());
		if (s.size()) {
			auto iter = ret.find(s);
			if (iter == ret.end()) {
				ret[s].insert(pos_index);
			}
		}
	}
	return ret;
}

void Tokenizer::load() {
	std::ifstream fin("invIndex.dat");
	if (!fin) {
		std::cout << "Invert Index doesn't exist!" << std::endl;
		return;
	}
	this->token.clear();
	std::string word;
	int df, pos, docid, tf;
	std::map<int, std::set<int, IntCmp>> doclist;

	while (fin >> word >> df) {
		doclist.clear();
		for (auto i = 0; i < df; ++i) {
			std::set<int, IntCmp> docItem;
			fin >> docid >> tf;
			for (auto j = 0; j < tf; j++) {
				fin >> pos;
				docItem.insert(pos);
			}
			doclist[docid] = docItem;
		}
		token[word] = doclist;
	}
}

void Tokenizer::persistence() {
	std::ofstream fout("invIndex.dat");
	std::cout << "Write into files..." << std::endl;
	for (auto x : this->token) {
		fout << x.first << " " << x.second.size() << std::endl;
		for (auto y : x.second) {
			fout << std::setw(10) << "" << std::setiosflags(std::ios::left) << std::setw(10) << y.first << " ";
			fout << std::setiosflags(std::ios::left) << std::setw(10) << y.second.size() << "\t";
			for (auto z : y.second) {
				fout << " " << z;
			}
			fout << std::endl;
		}
	}
	fout.close();
}

void Tokenizer::getFiles(std::string path, std::vector<std::string> &files) {
	int hFile = 0;
	std::string p;

	_finddata_t fileinfo;

	if ((hFile = _findfirst(p.assign(path).append("/*").c_str(), &fileinfo)) != -1) {
		do {
			if (fileinfo.attrib & _A_SUBDIR) {
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0) {
					this->getFiles(p.assign(path).append("/").append(fileinfo.name), files);
				}
			}
			else {
				files.push_back(p.assign(path).append("/").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

void Tokenizer::importDir(std::string path) {
	std::vector<std::string> files;
	getFiles(path, files);
	if (!files.size()) {
		std::cout << "No files found." << std::endl;
		return;
	}
	int t = 0;
	for (auto x : files) {
		t++;
		std::cout << "Processing NO." << t << " File" << std::endl;
		int slash = x.find_last_of("/\\");
		std::string filename(x.substr(slash + 1));
		int dot = filename.find_last_of(".");
		filename = filename.substr(0, dot);
		int docid = atoi(filename.c_str());
		std::ifstream fin(x);
		if (!fin) {
			std::cout << "File <" << x << "] cannot read. Skipped." << std::endl;
			continue;
		}
		std::string text((std::istreambuf_iterator<char>(fin)),
						 std::istreambuf_iterator<char>());

		this->push(text, docid);
		fin.close();
	}
}

std::string Tokenizer::kgram_check() {
	std::string s;
	int cnt = 0;
	for (auto i = 0; i < std::min(WINDOW_SIZE, (int)window.size()); ++i) {
		bool flag = false;
		for (auto j = 0; j < 2; j++) {
			if (window[i].pos.find(non_comp[j].c_str()) != std::string::npos) {
				flag = true;
				break;
			}
		}
		for (auto j = 0; j < 7; j++) {
			if (window[i].word.find(be_word[j]) != std::string::npos) {
				flag = false;
				break;
			}
		}
		if (!flag) continue;
		cnt++;
		if (!s.empty()) s += "_";
		s += window[i].word;
	}
	if (cnt == 1) s = "";
	return s;
}
