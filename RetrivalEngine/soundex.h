#ifndef _SOUNDEX_H_
#define _SOUNDEX_H_

#include "header.h"

class Soundex {
public:
	std::string encode(const std::string &word) const;
private:
	std::string zeroPad(const std::string &word) const;
	std::string head(const std::string &word) const;
	std::string tail(const std::string &word) const;
	std::string encodeDigits(const std::string &word) const;
	std::string encodeDigit(const char letter) const;
	std::string lastDigit(const std::string &word) const;
	void encodeHead(std::string &encoding, const std::string &word) const;
	void encodeTail(std::string &encoding, const std::string &word) const;
	void encodeLetter(std::string &encoding, const char letter, const char prevLetter) const;
	bool isComplete(const std::string &word) const;
	bool isVowel(const char letter) const;

	const unsigned MAX_ENCODING_LENGTH = 4;
	const std::string NOT_A_DIGIT { "*" };
};

#endif