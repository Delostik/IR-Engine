#include "lemmatizer.h"

std::string Lemmatizer::convert(std::string w) {
	return this->convert_lowercase(w);
}

std::string Lemmatizer::convert(std::string w, std::string pos) {
	w = this->convert_lowercase(w);
	if (pos == "NNS") w = this->restore_none(w);
	if (pos[0] == 'V') w = this->restore_verb(w);
	return w;
}

std::string Lemmatizer::convert_lowercase(std::string w) {
	transform(w.begin(), w.end(), w.begin(), tolower);
	return w;
}

std::string Lemmatizer::restore_none(std::string w) {
	int len = w.size();
	char c1(' '), c2(' '), c3(' '), c4(' ');
	if (len > 0) c1 = w[len - 1];
	if (len > 1) c2 = w[len - 2];
	if (len > 2) c3 = w[len - 3];
	if (len > 3) c4 = w[len - 4];
	if (c1 == 's') {
		if (c2 == 'e') {		// end with "es"
			if (c3 == 'x' || c3 == 's' || (c3 == 'h' && (c4 == 's' || c4 == 'c'))) { // end with x, s, sh, ch
				return w.substr(0, len - 2);
			}
			else if (c3 == 'v') {		// end with 'ves': wolves
				return w.substr(0, len - 3) + 'f';
			}
			else if (c3 == 'i') {		// end with 'ies': families
				return w.substr(0, len - 3) + 'y';
			}
			else return w.substr(0, len - 1);
		}
		else {
			return w.substr(0, len - 1);
		}
	}
	return w;
}

std::string Lemmatizer::restore_verb(std::string w) {
	return w;
}