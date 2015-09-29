#include "server.h"

int main(int argc, char *argv[]) {
	int sock;
	char *buffer;

	char *site = "<html><body>hello</body></html>";

	struct sockaddr_in address;
	struct sockaddr padd;

	address.sin_family = AF_INET;
	address.sin_port   = htons(30009);
	inet_pton(AF_INET, "192.168.33.10", &address.sin_addr);


	buffer = (char *) calloc(sizeof(char), 256);


	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket Failed to open:");
		exit(1);
	}

	bind(sock, (struct sockaddr *) &address, sizeof(address));

	if (listen(sock, 1) == -1) {
		perror("Failed to listen");
		close(sock);
		exit(1);
	}

	socklen_t psize;
	int csock;
	
	if ((csock = accept(sock, &padd, &psize)) == -1) {
		perror("Failed to except");
		close(sock); 
		exit(1);
	}

	read(csock, buffer, 256);

	printf("%s", buffer);

	if(write(csock, site, strlen(site)) == -1) {
		perror("FUCK");
	}

	close(csock);


	close(sock);	

	return 0;
}
