#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int sleep_sec;
	if (argc < 2){
		printf("Please enter sleep time\n");
		exit();
	}

	sleep_sec = atoi(argv[1]);
	if (sleep_sec > 0){
		sleep(sleep_sec);
        printf("Sleep %d\n",sleep_sec);
	} else {
		printf("Invalid interval %s\n", argv[1]);
	}
	exit();
}