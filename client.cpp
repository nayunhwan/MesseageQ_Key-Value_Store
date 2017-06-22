#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <thread>


#define BUFF_SIZE 100

#define PUT_DATA 1
#define GET_DATA 2
#define DEL_DATA 3
#define DATA_COUNT 9

using namespace std;

typedef struct {
	long data_type;
	unsigned int key;
	char value[BUFF_SIZE];
} t_data;

key_t requestQ = 1000;
key_t responseQ = 1001;

unsigned int ndx = 0;
int requestQID;
int responseQID;
t_data data;

unsigned int stringToInt(char *s) {
	unsigned int result = 0;
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
	int err = msgsnd(requestQID, &sndData, sizeof(t_data) - sizeof(long), 0);
	exceptionErr(err, "putKey 실패");
	printf("PUT :: key = %d value = %s\n", sndData.key, sndData.value);
}

void getKey(t_data sndData) {
	int err = msgsnd(requestQID, &sndData, sizeof(t_data) - sizeof(long), 0);
	exceptionErr(err, "getKey request 실패");

	t_data rcvData;
	err = msgrcv(responseQID, &rcvData, sizeof(t_data) - sizeof(long), GET_DATA, 0);
	exceptionErr(err, "getKey response 실패");
	printf("GET :: key = %d value = %s\n", rcvData.key, rcvData.value);

}

void deleteKey(t_data sndData) {
	int err = msgsnd(requestQID, &sndData, sizeof(t_data) - sizeof(long), 0);
	exceptionErr(err, "deleteKey request 실패");
}

void getDataCount() {
	printf("get data count\n");
	t_data sndData;
	sndData.data_type = DATA_COUNT;
	int err = msgsnd(requestQID, &sndData, sizeof(t_data) - sizeof(long), 0);
	exceptionErr(err, "get data count 실패");

	t_data rcvData;
	err = msgrcv(responseQID, &rcvData, sizeof(t_data) - sizeof(long), DATA_COUNT, 0);
	ndx = rcvData.key;
	printf("ndx = %d\n", ndx);
} 




string generateRandomString() {
	string str;
	int len = rand()%10 + 10;
	char s[len];
	for(int i = 0; i < len; i++){
		s[i] = (rand()%2 == 0)? rand()%('z'-'a') + 'a' : rand()%('z'-'a') + 'A';
	}
	s[len] = '\0';
	str = s;
	return str;
}

void generateRandomData() {

	int n = rand()%90 + 50;
	while(n--) {
		t_data sndData;
		sndData.data_type = PUT_DATA;
		sndData.key = ndx++;
		string str = generateRandomString();		
		strcpy(sndData.value, str.c_str());
		putKey(sndData);
	}

}

void geneateTestSet() {
	int n = rand()%90 + 100;
	while(n--) {
		int select = rand()%100 + 1;
		if(select <= 95) {
			t_data sndData;
			sndData.data_type = GET_DATA;
			sndData.key = rand()%ndx;
			getKey(sndData);
		}
		else {
			t_data sndData;
			sndData.data_type = PUT_DATA;
			sndData.key = ndx++;
			string str = generateRandomString();		
			strcpy(sndData.value, str.c_str());
			putKey(sndData);
		}
	}
}


int main(){
	
	requestQID = msgget(requestQ, IPC_CREAT | 0666);
	exceptionErr(requestQID, "REQUEST QUEUE ERROR");
	responseQID = msgget(responseQ, IPC_CREAT | 0666);
	exceptionErr(responseQID, "RESPONSE QUEUE ERROR");


	getDataCount();


	while(1) {
		
		char op[100];
		char input[BUFF_SIZE];
		scanf("%s %s", op, input);
		srand(time(NULL));

		// Push Data
		if(strcmp(op, "push") == 0) {
			printf("\nPUSH DATA\n");
			t_data sndData;
			sndData.data_type = PUT_DATA;
			sndData.key = ndx++;
			strcpy(sndData.value, input);
			putKey(sndData);
		}
		else if(strcmp(op, "get") == 0) {
			printf("\nGET DATA\n");
			t_data sndData;
			sndData.data_type = GET_DATA;
			sndData.key = stringToInt(input);
			getKey(sndData);
		}
		else if(strcmp(op, "del") == 0) {
			printf("\nDELETE DATA\n");
			t_data sndData;
			sndData.data_type = DEL_DATA;
			sndData.key = stringToInt(input);
			deleteKey(sndData);
		}
		else if(strcmp(op, "ran") == 0) {
			generateRandomData();
		}
		else if(strcmp(op, "test") == 0){
			geneateTestSet();
		}

		sleep(1);
	}

	return 0;
}