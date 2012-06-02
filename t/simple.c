#include <tap.h>

int main() {
	setvbuf(stdout, NULL, _IONBF, 0);
	plan(1);
	ok(1);
	done_testing();
}
