#include <stdio.h>
#include "libnetfiles.h"

int main(){
	
	int filedes;
	//char buffer[500];
	//char * writestr = "whoaaa!\n";

	if(netserverinit("localhost", 0) == -1){
		printf("Connection failed\n");
		return 0;
	}

	filedes = netopen("hello.txt", 0);

	//netread(filedes, buffer, 50);
	//printf("file says: %s\n", buffer);
	//netwrite(filedes, writestr, 50);
	//memset(buffer, 0, 500);
	//netread(file, buffer, 50);
	//printf("file says: %s\n", buffer);
	//netclose(filedes);

	return 0;
}