#ifndef _LEMMATIZER_H_
#define _LEMMATIZER_H_

#include "header.h"

class Lemmatizer {
public:
	std::string convert(std::string);
	std::string convert(std::string, std::string);
private:
	std::string convert_lowercase(std::string);
	std::string restore_none(std::string);
	std::string restore_verb(std::string);
};


#endif