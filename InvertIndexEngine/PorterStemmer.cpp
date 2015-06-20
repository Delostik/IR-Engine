#include "PorterStemmer.h"

void PorterStemmer::stem(std::string & word) {
	if (word.size() <= 2 || word == "<s>" || word == "</s>")
		return;

	if (word.size() > 35)
		word = word.substr(0, 35);

	if (word[0] == '\'')
		word = word.substr(1, word.size() - 1);

	if (special(word))
		return;

	changeY(word);
	size_t startR1 = getStartR1(word);
	size_t startR2 = getStartR2(word, startR1);

	step0(word);

	if (step1A(word)) {
		std::replace(word.begin(), word.end(), 'Y', 'y');
		return;
	}

	step1B(word, startR1);
	step1C(word);
	step2(word, startR1);
	step3(word, startR1, startR2);
	step4(word, startR2);
	step5(word, startR1, startR2);

	std::replace(word.begin(), word.end(), 'Y', 'y');
	return;
}

void PorterStemmer::trim(std::string & word) {
	if (word == "<s>" || word == "</s>")
		return;

	std::transform(word.begin(), word.end(), word.begin(), ::tolower);
	auto it = std::remove_if(word.begin(), word.end(), [](char ch) {
		return !((ch >= 'a' && ch <= 'z') || ch == '\'');
	});

	word.erase(it, word.end());
}

size_t PorterStemmer::getStartR1(const std::string & word) {
	if (word.substr(0, 5) == "gener")
		return 5;
	if (word.substr(0, 6) == "commun")
		return 6;
	if (word.substr(0, 5) == "arsen")
		return 5;

	size_t startR1 = firstNonVowelAfterVowel(word, 1);

	return startR1;
}

size_t PorterStemmer::getStartR2(const std::string & word, size_t startR1) {
	if (startR1 == word.size())
		return startR1;

	size_t startR2 = firstNonVowelAfterVowel(word, startR1 + 1);

	return startR2;
}

size_t PorterStemmer::firstNonVowelAfterVowel(const std::string & word, size_t start) {
	for (size_t i = start; i != 0 && i < word.size(); ++i) {
		if (!isVowelY(word[i]) && isVowelY(word[i - 1]))
			return i + 1;
	}

	return word.size();
}

void PorterStemmer::changeY(std::string & word) {
	if (word.find_first_of("y") == std::string::npos)
		return;

	if (word[0] == 'y')
		word[0] = 'Y';

	for (size_t i = 1; i < word.size(); ++i) {
		if (word[i] == 'y' && isVowel(word[i - 1]))
			word[i++] = 'Y';
	}
}


void PorterStemmer::step0(std::string & word) {
	replaceIfExists(word, "'s'", "", 0)
		|| replaceIfExists(word, "'s", "", 0)
		|| replaceIfExists(word, "'", "", 0);
}

bool PorterStemmer::step1A(std::string & word) {
	if (!replaceIfExists(word, "sses", "ss", 0)) {
		if (endsWith(word, "ied") || endsWith(word, "ies")) {
			if (word.size() <= 4)
				word = word.substr(0, word.size() - 1);
			else
				word = word.substr(0, word.size() - 2);
		}
		else if (endsWith(word, "s") && !endsWith(word, "us") && !endsWith(word, "ss")) {
			if (word.size() > 2 && containsVowel(word, 0, word.size() - 2))
				word = word.substr(0, word.size() - 1);
		}
	}

	return word == "inning" || word == "outing" || word == "canning" || word == "herring" ||
		word == "earring" || word == "proceed" || word == "exceed" || word == "succeed";
}

void PorterStemmer::step1B(std::string & word, size_t startR1) {
	bool exists = endsWith(word, "eedly") || endsWith(word, "eed");

	if (exists)
		replaceIfExists(word, "eedly", "ee", startR1) || replaceIfExists(word, "eed", "ee", startR1);
	else {
		size_t size = word.size();
		bool deleted = (containsVowel(word, 0, size - 2) && replaceIfExists(word, "ed", "", 0))
			|| (containsVowel(word, 0, size - 4) && replaceIfExists(word, "edly", "", 0))
			|| (containsVowel(word, 0, size - 3) && replaceIfExists(word, "ing", "", 0))
			|| (containsVowel(word, 0, size - 5) && replaceIfExists(word, "ingly", "", 0));

		if (deleted && (endsWith(word, "at") || endsWith(word, "bl") || endsWith(word, "iz")))
			word = word + "e";
		else if (deleted && endsInDouble(word))
			word = word.substr(0, word.size() - 1);
		else if (deleted && startR1 == word.size() && isShort(word))
			word = word + "e";
	}
}

void PorterStemmer::step1C(std::string & word) {
	size_t size = word.size();
	if (size > 2 && (word[size - 1] == 'y' || word[size - 1] == 'Y'))
	if (!isVowel(word[size - 2]))
		word[size - 1] = 'i';
}

void PorterStemmer::step2(std::string & word, size_t startR1) {
	static const std::vector<std::pair<std::string, std::string>> subs = {
		{ "ational", "ate" }, { "tional", "tion" }, { "enci", "ence" }, { "anci", "ance" },
		{ "abli", "able" }, { "entli", "ent" }, { "izer", "ize" }, { "ization", "ize" },
		{ "ation", "ate" }, { "ator", "ate" }, { "alism", "al" }, { "aliti", "al" },
		{ "alli", "al" }, { "fulness", "ful" }, { "ousli", "ous" }, { "ousness", "ous" },
		{ "iveness", "ive" }, { "iviti", "ive" }, { "biliti", "ble" }, { "bli", "ble" },
		{ "fulli", "ful" }, { "lessli", "less" }
	};

	for (auto & sub : subs)
	if (replaceIfExists(word, sub.first, sub.second, startR1))
		return;

	if (!replaceIfExists(word, "logi", "log", startR1 - 1)) {
		if (endsWith(word, "li") && !endsWith(word, "abli") && !endsWith(word, "entli")
			&& !endsWith(word, "aliti") && !endsWith(word, "alli") && !endsWith(word, "ousli")
			&& !endsWith(word, "bli") && !endsWith(word, "fulli") && !endsWith(word, "lessli"))
		if (word.size() > 3 && word.size() - 2 >= startR1 && isValidLIEnding(word[word.size() - 3]))
			word = word.substr(0, word.size() - 2);
	}
}

void PorterStemmer::step3(std::string & word, size_t startR1, size_t startR2) {
	static const std::vector<std::pair<std::string, std::string>> subs = {
		{ "ational", "ate" }, { "tional", "tion" }, { "alize", "al" }, { "icate", "ic" },
		{ "iciti", "ic" }, { "ical", "ic" }, { "ful", "" }, { "ness", "" }
	};

	for (auto & sub : subs)
	if (replaceIfExists(word, sub.first, sub.second, startR1))
		return;

	replaceIfExists(word, "ative", "", startR2);
}

void PorterStemmer::step4(std::string & word, size_t startR2) {
	static const std::vector<std::pair<std::string, std::string>> subs = {
		{ "al", "" }, { "ance", "" }, { "ence", "" }, { "er", "" }, { "ic", "" },
		{ "able", "" }, { "ible", "" }, { "ant", "" }, { "ement", "" }, { "ment", "" },
		{ "ism", "" }, { "ate", "" }, { "iti", "" }, { "ous", "" }, { "ive", "" }, { "ize", "" }
	};

	for (auto & sub : subs)
	if (replaceIfExists(word, sub.first, sub.second, startR2))
		return;

	if (!endsWith(word, "ement") && !endsWith(word, "ment"))
	if (replaceIfExists(word, "ent", "", startR2))
		return;

	// short circuit
	replaceIfExists(word, "sion", "s", startR2 - 1)
		|| replaceIfExists(word, "tion", "t", startR2 - 1);
}

void PorterStemmer::step5(std::string & word, size_t startR1, size_t startR2) {
	size_t size = word.size();
	if (word[size - 1] == 'e') {
		if (size - 1 >= startR2)
			word = word.substr(0, size - 1);
		else if (size - 1 >= startR1 && !isShort(word.substr(0, size - 1)))
			word = word.substr(0, size - 1);
	}
	else if (word[word.size() - 1] == 'l') {
		if (word.size() - 1 >= startR2 && word[word.size() - 2] == 'l')
			word = word.substr(0, word.size() - 1);
	}
}

bool PorterStemmer::isShort(const std::string & word) {
	size_t size = word.size();

	if (size >= 3) {
		if (!isVowelY(word[size - 3]) && isVowelY(word[size - 2])
			&& !isVowelY(word[size - 1]) && word[size - 1] != 'w'
			&& word[size - 1] != 'x' && word[size - 1] != 'Y')
			return true;
	}
	return size == 2 && isVowelY(word[0]) && !isVowelY(word[1]);
}

bool PorterStemmer::special(std::string & word) {
	static const std::unordered_map<std::string, std::string> exceptions = {
		{ "skis", "ski" }, { "skies", "sky" }, { "dying", "die" }, { "lying", "lie" },
		{ "tying", "tie" }, { "idly", "idl" }, { "gently", "gentl" }, { "ugly", "ugli" },
		{ "early", "earli" }, { "only", "onli" }, { "singly", "singl" }
	};

	auto ex = exceptions.find(word);
	if (ex != exceptions.end()) {
		word = ex->second;
		return true;
	}

	return word == "sky" || word == "news" || word == "howe" ||
		word == "atlas" || word == "cosmos" || word == "bias" ||
		word == "andes";
}

bool PorterStemmer::isVowelY(char ch) {
	return ch == 'e' || ch == 'a' || ch == 'i' ||
		ch == 'o' || ch == 'u' || ch == 'y';
}

bool PorterStemmer::isVowel(char ch) {
	return ch == 'e' || ch == 'a' || ch == 'i' ||
		ch == 'o' || ch == 'u';
}

bool PorterStemmer::endsWith(const std::string & word, const std::string & str) {
	return word.size() >= str.size() &&
		word.substr(word.size() - str.size()) == str;
}

bool PorterStemmer::endsInDouble(const std::string & word) {
	if (word.size() >= 2) {
		char a = word[word.size() - 1];
		char b = word[word.size() - 2];

		if (a == b)
			return a == 'b' || a == 'd' || a == 'f' || a == 'g'
			|| a == 'm' || a == 'n' || a == 'p' || a == 'r' || a == 't';
	}
	return false;
}

bool PorterStemmer::replaceIfExists(std::string & word,
									const std::string & suffix, const std::string & replacement, size_t start) {
	if (!(start > word.size()) && endsWith(word.substr(start, word.size() - start), suffix)) {
		word = word.substr(0, word.size() - suffix.size()) + replacement;
		return true;
	}
	return false;
}

bool PorterStemmer::isValidLIEnding(char ch) {
	return ch == 'c' || ch == 'd' || ch == 'e' || ch == 'g' || ch == 'h'
		|| ch == 'k' || ch == 'm' || ch == 'n' || ch == 'r' || ch == 't';
}

bool PorterStemmer::containsVowel(const std::string & word, size_t start, size_t end) {
	if (start <= end && end < word.size()) {
		for (size_t i = start; i < end; ++i)
		if (isVowelY(word[i]))
			return true;
	}
	return false;
}