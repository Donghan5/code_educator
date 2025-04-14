#include <iostream>
#include <vector>
#include <algorithm>
#include <string>


void testComplexity(void) {
	int i = 0;
	for (i = 0; i < 1000; i++) {
		std::cout << "i = " << i << std::endl;
		for (int j = 0; j < 1000; j++) {
			std::cout << "j = " << j << std::endl;
			for (int k = 0; k < 1000; k++) {
				std::cout << "k = " << k << std::endl;
			}
		}
	}
}

int main() {
	std::cout << "Hello, World!" << std::endl;
	testComplexity();
	return 0;
}
