#include "tokenizer.h"
#include <iostream>

int main() {
	Tokenizer * tokenizer = new Tokenizer();

	tokenizer->importDir("Docs");
	tokenizer->persistence();
	
	std::cout << "Invert Index Created Successfully." << std::endl;
	system("pause");
	return 0;
}