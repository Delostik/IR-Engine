#include "SearchEngine.h"


void SearchEngine::search(std::string s) {
	for (int i = 0; i<s.size(); i++) {
		if (!(s[i] >= '0' && s[i] <= '9') && !(s[i] >= 'a' && s[i] <= 'z') && !(s[i] >= 'A' && s[i] <= 'Z')) s[i] = ' ';
	}
	resCnt = 0;
	this->key = s;
	bool flag = this->tokenizer();
	int resCnt = 0;
	if (!flag && this->tokens.size() > 1 && this->tokens.size() < 5) {
		this->position_search();
	}
	else {
		this->token_search();
		if (resCnt < 15) {
			this->position_search();
		}
	}
	if (resCnt < 15 && s.find(' ') == std::string::npos) {
		std::string code = soundex.encode(s);
		auto iter = soundMap.find(code);
		if (iter != soundMap.end()) {
			ostr << "=";
			for (auto x : iter->second) {
				ostr << num2word[x] << " ";
			}
		}
	}
}

bool SearchEngine::tokenizer() {
	bool ret = false;
	std::set<std::string, StringCmp>().swap(this->tokens);

	senna(this->key.c_str());

	senna_result.clear();
	std::ifstream fin("pos_ner.dat");
	std::string word, pos, ner;
	std::string multiword = "";
	int multiword_index = 0;

	while (fin >> word >> pos >> ner) {
		stemmer.trim(word);
		stemmer.stem(word);

		senna_result.push_back(SENNA_result(word, pos, ner));
		if (ner.size() > 1 && ner[1] == '-') {
			if (ner[0] == 'B') {
				multiword = word;
			}
			else if (ner[0] == 'I') {
				multiword = multiword + "_" + word;
			}
			else if (ner[0] == 'E') {
				multiword = multiword + "_" + word;
				tokens.insert(multiword);
				ret = true;
			}
		}
		if (word.size() == 1 && !(word[0] >= 'a' && word[0] <= 'z')) continue;
		tokens.insert(word);
	}
	fin.close();
	for (auto i = 0; i < senna_result.size(); ++i) {
		if (window.size() < WINDOW_SIZE) window.push_back(senna_result[i]);
		else {
			window.erase(window.begin());
			window.push_back(senna_result[i]);
		}
		std::string s(this->kgram_check());
		if (s.size()) {
			tokens.insert(s);
			ret = true;
		}
	}
	tokens.insert(this->key);
	for (auto x : tokens) { std::cout << "---" << x << std::endl; }
	return ret;
}

std::string SearchEngine::kgram_check() {
	std::string s;
	int cnt = 0;
	for (auto i = 0; i < min(WINDOW_SIZE, (int)window.size()); ++i) {
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
		if (!s.empty()) s += "_";
		s += window[i].word;
		cnt++;
	}
	if (cnt == 1) s = "";
	return s;

}

int SearchEngine::position_search() {
	std::priority_queue<int, std::vector<int>, greater> q;
	std::vector<int> token_seq;

	std::map<int, int> res_set;
	for (auto &x : tokens) {
		token_seq.push_back(word2num[x]);
	}
	for (auto & x : docList) {
		bool flag = true;
		for (auto &y : token_seq) {
			if (index[y].list.find(x) == index[y].list.end()) {
				flag = false;
				break;
			}
		}
		if (!flag) continue;
		std::priority_queue<int, std::vector<int>, greater>().swap(q);
		for (auto &y : token_seq) {
			for (auto &z : index[y].list[x].pos) {
				q.push(z);
			}
		}
		std::vector<int> window;
		while (!q.empty()) {
			if (window.size() == this->tokens.size()) {
				window.erase(window.begin());
			}
			window.push_back(q.top());
			q.pop();
			if (window.size() == this->tokens.size()) {

				if (window[window.size() - 1] - window[0] < this->tokens.size() + 2) {
					res_set[x]++;
				}
			}
		}
	}
	std::vector<std::pair<int, int>> res;

	for (std::map<int, int>::reverse_iterator x = res_set.rbegin(); x != res_set.rend(); x++) {
		res.push_back(std::make_pair(x->second, x->first));
	}

	std::sort(res.begin(), res.end(), [](std::pair<int, int> &a, std::pair<int, int> &b) -> bool {return a.first > b.first; });
	for (auto &x : res) {
		resCnt++;
		ostr << x.first << " " << x.second << " ";
		if (resCnt >= 20) break;
	}
	return res.size();
}

int SearchEngine::token_search() {
	double token_module = std::sqrt(tokens.size());
	std::vector<int> token_vec;
	for (auto &x : word2num) {
		if (tokens.find(x.first) != tokens.end()) {
			token_vec.push_back(x.second);
		}
	}

	std::map<int, double, IntCmp> SimCos;
	int len = token_vec.size();
	for (auto &x : docList) {
		double tmp = 0;
		for (auto i = 0; i < len; i++) {
			tmp += 1 * docVec[x][token_vec[i]];
		}
		tmp = tmp / (token_module * docModule[x]);
		if (tmp < 1e-4) continue;
		SimCos[x] = tmp;
	}

	std::vector<std::pair<double, int>> res;

	for (std::map<int, double>::reverse_iterator x = SimCos.rbegin(); x != SimCos.rend(); x++) {
		res.push_back(std::make_pair(x->second, x->first));
	}

	std::sort(res.begin(), res.end(), [](std::pair<double, int> &a, std::pair<double, int> &b) -> bool {return a.first > b.first; });
	for (auto &x : res) {
		resCnt++;
		ostr << x.first << " " << x.second << " ";
		if (resCnt >= 20) break;
	}
	return res.size();
}
