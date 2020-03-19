
#include <unistd.h>
#include <cstdio>
#include <iostream>

int main() {
	char buffer[256];
	while (!feof(stdin)) {
		size_t bytes = fread(buffer, 1, sizeof(buffer), stdin);
		size_t b = fwrite(buffer, 1, bytes, stdout);
		fflush(stdout);
	}
	return 0;
}
