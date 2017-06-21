#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <thread>

#define BUFF_SIZE 100

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


unsigned int stringToInt(char *s) {
	int result = 0;
	int len = strlen(s);
	for(int i = 0; i < len; i++){
		result *= 10;
		result += (s[i]-'0');
	}
	return result;
}

void exceptionErr(int err, string s) {
	if(err == -1) {
		perror(s.c_str());
		exit(1);
	}
}

void putKey(t_data sndData) {

	int err = msgsnd(msqid, &sndData, sizeof(t_data) - sizeof(long), 0);
		if(err == -1) {
			perror("msgsnd() 실패");
			exit(1);
	}

	printf("key = %d value = %s\n", sndData.key, sndData.value);
}

void getKey(t_data sndData) {
	int err = msgsnd(msqid, &sndData, sizeof(t_data) - sizeof(long), 0);
		if(err == -1) {
			perror("msgsnd() 실패");
			exit(1);
	}

	t_data rcvData;
	err = msgrcv(msqid, &rcvData, sizeof(t_data)-sizeof(long), 2, 0);
	exceptionErr(err, "rcvData 실패");
	printf("key = %d value = %s\n", rcvData.key, rcvData.value);

}

void deleteKey(unsigned int key) {

}



int main(){
	
	msqid = msgget(msgQKey, IPC_CREAT | 0666);

	while(1) {
		
		
		char op[100];
		char input[BUFF_SIZE];
		scanf("%s %s", op, input);

		// Push Data
		if(strcmp(op, "push") == 0) {
			printf("\nPUSH DATA\n");
			t_data sndData;
			sndData.data_type = 1;
			sndData.key = ndx++;
			strcpy(sndData.value, input);
			putKey(sndData);
		}
		else if(strcmp(op, "get") == 0) {
			printf("\nGET DATA\n");
			t_data sndData;
			sndData.data_type = 2;
			sndData.key = stringToInt(input);
			getKey(sndData);
		}

		
		sleep(1);
	}

	return 0;
}