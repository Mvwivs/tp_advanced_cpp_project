
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>

int main() {
	{std::ofstream out("test.txt");}
	char buffer[256];
	while (!feof(stdin)) {
		size_t bytes = fread(buffer, 1, sizeof(buffer), stdin);
		size_t b = fwrite(buffer, 1, bytes, stdout);
		fflush(stdout);
	}
	return 0;
}
