#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <list>

#define BUFF_SIZE 1024

using namespace std;

typedef struct {
	long data_type;
	unsigned int key;
	char value[BUFF_SIZE];
} t_data;


key_t msgQKey = 1000;
int ndx = 0;
int msqid;
t_data data;
list<t_data> mainList;

void receiveMSG() {
	t_data rcvData;
	int err = msgrcv(msqid, &rcvData, sizeof(t_data)-sizeof(long), 1, 0);

	if(err == -1) {
		perror("msgsnd() 실패");
		exit(1);
	}

	printf("%d - %s\n", rcvData.key, rcvData.value);
	mainList.push_front(rcvData);
}

void showList() {
	printf("------- Show List -------\n");
	list<t_data>::iterator iter;
	for(iter = mainList.begin(); iter != mainList.end(); iter++) {
		printf("%d - %s\n", iter -> key, iter -> value);
	}
}


int main() {
	
	msqid = msgget(msgQKey, IPC_CREAT | 0666);

	if(msqid == -1) {
		perror("msgget() 실패");
		exit(1);
	}

	while(1) {
		t_data rcvData;
		int err = msgrcv(msqid, &rcvData, sizeof(t_data)-sizeof(long), 1, 0);
		if(err == -1) {
			perror("msgrcv() 실패");
			exit(1);
		}
		printf("%d - %s\n", rcvData.key, rcvData.value);
		mainList.push_front(rcvData);
		showList();
		sleep(1);
	}

	return 0;
}