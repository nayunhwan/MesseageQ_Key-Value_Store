#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 1024

typedef struct {
	long data_type;
	unsigned int key;
	char value[BUFF_SIZE];
} t_data;

key_t msgQKey = 1000;
int ndx = 0;
int msqid;
t_data data;

void putKey(t_data sndData) {

	int err = msgsnd(msqid, &sndData, sizeof(t_data) - sizeof(long), 0);
		if(err == -1) {
			perror("msgsnd() 실패");
			exit(1);
	}

	printf("key = %d value = %s\n", sndData.key, sndData.value);
}

void getKey(unsigned int key) {

}

void deleteKey(unsigned int key) {

}


int main(){
	
	msqid = msgget(msgQKey, IPC_CREAT | 0666);

	while(1) {
		
		t_data sndData;
		sndData.data_type = 1;
		sndData.key = ndx++;
		char op[100];
		char input[BUFF_SIZE];
		scanf("%s %s", op, input);
		if(strcmp(op, "push")) {
						
		}

		putKey(sndData);
		sleep(1);
	}

	return 0;
}