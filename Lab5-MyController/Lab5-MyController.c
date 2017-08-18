#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
        struct _pulse   pulse;
        char msg[255];
} my_message_t;

int main(int argc, char *argv[]) {
	FILE * fd;
	char status[255];
	char command[255] = "N/A";
	int rcvid;
	my_message_t msg;
	name_attach_t *attach;

	if ((attach = name_attach(NULL, "dev/example_namespace", 0)) == NULL) {
		printf("Name_Attach failed");
		return EXIT_FAILURE;
	}

	fd = fopen("/dev/labdevice", "w+");

	fscanf(fd, "%s %s\n", command, status);

	if(strcmp(command, "status") == 0){
		printf("Status: %s\n", status);
		if(strcmp(status, "closed") == 0){
				name_detach(attach, 0);
				return EXIT_SUCCESS;
		}
	}
	fclose(fd);

	while (1) {
	    rcvid = MsgReceivePulse(attach->chid, &msg, sizeof(msg), NULL);
	    if (rcvid == 0) {
	    	if (msg.pulse.code == MY_PULSE_CODE) {
	    		printf("Small Integer: %d\n", msg.pulse.value.sival_int);
	    		fd = fopen("/dev/labdevice", "w+");
	    		fscanf(fd, "%s %s\n", command, status);
	    		if(strcmp(command, "status") == 0){
	    		  	printf("Status: %s\n", status);
	    		   	if(strcmp(status, "closed") == 0){
	    		  		name_detach(attach, 0);
	    		   	    return EXIT_SUCCESS;
	    		   	}
	    		}
	    		fclose(fd);
	        }
	    }
	    MsgReply( rcvid, EOK, 0, 0 );
	}

	name_detach(attach, 0);

	return EXIT_SUCCESS;
}
