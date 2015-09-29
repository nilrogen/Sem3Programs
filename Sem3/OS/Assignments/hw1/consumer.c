#include "assignment1.h"

char *gettimestring(const struct timeval *time) {
	struct tm *p_tm;
	// Too lazy to program this well
	static char retv[40]; 

	p_tm = localtime(&time->tv_sec);
	strftime(retv, sizeof(retv), "%H:%M:%S", p_tm);
	return retv;
}

int main(int argc, char *argv[]) {
	char *pnum; // The name of the Thread (argv[1]) 

	donut_t *p_data; // The pointer to the shared region

	ushort xsub1[3];
	struct timeval randtime;

	int i, j;
	int donuts[12 * N_DOZEN][2];
	int cdonut;

	p_data = getSHMSafe();

	if (argc > 1) {
		pnum = argv[1];
	}
	else {
		pnum = "N/A";
	}

	gettimeofday(&randtime, (struct timezone *) 0);
	xsub1[0] = (ushort) randtime.tv_usec;
	xsub1[1] = (ushort) (randtime.tv_usec >> 16);
	xsub1[2] = (ushort) (getpid());


	for (i = 0; i < 12 * N_DOZEN; i++) {
		cdonut = nrand48(xsub1) % D_TYPES;
		
		Pout(p_data, cdonut);
		Pmutex(p_data, cdonut);

		// CS
		donuts[i][0] = p_data->p_flavors[cdonut][p_data->p_out[i]]; // Get
		p_data->p_out[cdonut] = (p_data->p_out[cdonut] + 1) % D_SIZE;
		donuts[i][1] = cdonut;

		Vmutex(p_data, cdonut);
		Vin(p_data, cdonut);

	}

	j = 0;
	gettimeofday(&randtime, (struct timezone *) 0);				
	printf("\nConsumer %s Done:", pnum);
	printf("\nPID: %d, Time: %s", getpid(), gettimestring(&randtime));
	printf("\n\nDonut order by dozen:");
	for (i = 0; i < 12 * N_DOZEN; i++) {
		if (!(i % 12)) {
			printf("\nDozen %d:", j);
			j++;
		}
		printf(" (%d, %d)", donuts[i][1], donuts[i][0]);
	}
	printf("\n\n");

	safeUnmap(p_data);
	return 0;
}
