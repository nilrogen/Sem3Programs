#include "header.h"


int main(int argc, char *argv[]) {
	val_t *val;

	val = getSHMSafe();

	printf("Setting value\n");
	val->value = 1;
	printf("Value Set\n");

	safeUnmap();

	return 0;
}
