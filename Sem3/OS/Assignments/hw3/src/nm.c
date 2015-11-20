#include "nm.h"

static const char *HOSTS = { "", "192.168.1.13", "localhost" };
#define NUM_HOSTS (sizeof(HOSTS) / sizeof(char *))

static struct { 
	int *open_sockets; 		
} globals; 

int main(int argc, char *argv[]) {
	
	return 0; 
}
