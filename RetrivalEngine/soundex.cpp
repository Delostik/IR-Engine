#include "soundex.h"

std::string Soundex::encode(const std::string &word) const {
	return zeroPad(head(word) + tail(encodeDigits(word)));
}

std::string Soundex::head(const std::string &word) const {
	return word.substr(0, 1);
}

std::string Soundex::tail(const std::string &word) const {
	return word.substr(1);
}

bool Soundex::isComplete(const std::string &encoding) const {
	return encoding.length() == MAX_ENCODING_LENGTH;
}

bool Soundex::isVowel(const char letter) const {
	return std::string("aeiouy").find(tolower(letter)) != std::string::npos;
}

std::string Soundex::lastDigit(const std::string &word) const {
	if (word.empty()) return NOT_A_DIGIT;
	return std::string(1, word.back());
}

void Soundex::encodeHead(std::string &encoding, const std::string &word) const {
	encoding += encodeDigit(word.front());
}

void Soundex::encodeLetter(std::string &encoding, const char letter, const char prevLetter) const {
	if (isComplete(encoding)) return;
	if (encodeDigit(letter) == NOT_A_DIGIT) return;

	if (encodeDigit(letter) != lastDigit(encoding) || isVowel(prevLetter))
		encoding += encodeDigit(letter);
}

void Soundex::encodeTail(std::string &encoding, const std::string &word) const {
	for (auto idx = 1u; idx < word.length(); idx++) {
		encodeLetter(encoding, word[idx], word[idx - 1]);
	}
}

std::string Soundex::encodeDigits(const std::string &word) const {
	if (word.empty()) return "";

	std::string encoding;
	encodeHead(encoding, word);
	encodeTail(encoding, word);

	return encoding;
}

std::string Soundex::encodeDigit(char letter) const {
	std::unordered_map<char, std::string> encodings {
		{ 'b', "1" }, { 'f', "1" }, { 'p', "1" }, { 'v', "1" },
		{ 'c', "2" }, { 'g', "2" }, { 'j', "2" }, { 'k', "2" },
		{ 'q', "2" }, { 's', "2" }, { 'x', "2" }, { 'z', "2" },
		{ 'd', "3" }, { 't', "3" },
		{ 'l', "4" },
		{ 'm', "5" }, { 'n', "5" },
		{ 'r', "6" }
};
	auto entry = encodings.find(tolower(letter));
	return entry == encodings.end() ? NOT_A_DIGIT : entry->second;
}

std::string Soundex::zeroPad(const std::string &word) const {
	auto paddingLength = MAX_ENCODING_LENGTH - word.length();
	return word + std::string(paddingLength, '0');
}