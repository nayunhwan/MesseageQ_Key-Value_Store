#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string>
#include <list>
#include <map>
#include <thread>
#include <pthread.h>


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

int ndx = 0;
int requestQID;
int responseQID;

t_data data;

map<unsigned int, string> storage;

pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;


void insertData(unsigned int key, char* value);
void showMap();
void readData();
void writeData();
void exceptionErr(int err, string s);
void receiveMSG();
void receiveOP();
void receiveCount();
void receiveDel();

// list<t_data> mainList;

// void showList() {
// 	printf("------- Show List -------\n");
// 	list<t_data>::iterator iter;
// 	for(iter = mainList.begin(); iter != mainList.end(); iter++) {
// 		printf("%d - %s\n", iter -> key, iter -> value);
// 	}
// }

// int compareByKey(t_data x, t_data y) {
// 	return (x.key > y.key)? -1 : (x.key == y.key)? 0 : 1;
// }

// int compareByValue(t_data x, t_data y) {
// 	return strcmp(x.value, y.value);
// }

// void sortListByKey(list<t_data> &list) {
// 	list.sort(compareByKey);
// }

// void sortListByValue(list<t_data> &list) {
// 	list.sort(compareByValue);
// }

// t_data search(unsigned int key) {
// 	list<t_data>::iterator findItr = find(mainList.begin(), mainList.end(), key);
// 	return findItr;
// }


int main() {
	readData();

	requestQID = msgget(requestQ, IPC_CREAT | 0666);
	exceptionErr(requestQID, "REQUEST QUEUE ERROR");
	responseQID = msgget(responseQ, IPC_CREAT | 0666);
	exceptionErr(responseQID, "RESPONSE QUEUE ERROR");

	thread t1(&receiveMSG);
	thread t2(&receiveOP);
	thread t3(&receiveCount);
	thread t4(&receiveDel);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	return 0;
}

void exceptionErr(int err, string s) {
	if(err == -1) {
		perror(s.c_str());
		exit(1);
	}
}

void showMap() {
	printf("------- Show Map -------\n");
	map<unsigned int, string>::iterator iter;
	for(iter = storage.begin(); iter != storage.end(); iter++) {
		printf("%d - %s\n", iter -> first, iter -> second.c_str());
	}
}

void insertData(unsigned int key, char* value) {
	storage.insert(pair<unsigned int, string>(key, value));
	writeData();
}

void readData() {
	FILE *file = fopen("data.dat", "r");
	if(file == NULL) {
		file = fopen("data.dat", "a");
	}
	else {
		while(!feof(file)) {
			unsigned int key;
			char value[BUFF_SIZE];
			fscanf(file, "%d\t%s", &key, value);
			insertData(key, value);
		}
		fclose(file);
		showMap();	
	}
}

void writeData() {
	FILE *file = fopen("data.dat", "w");
	map<unsigned int, string>::iterator iter;
	for(iter = storage.begin(); iter != storage.end(); iter++) {
		fprintf(file, "%d\t%s\n", iter -> first, iter -> second.c_str());
	}
	fclose(file);
}

void receiveMSG() {
	while(1){
		t_data rcvData;
		int err = msgrcv(requestQID, &rcvData, sizeof(t_data)-sizeof(long), PUT_DATA, 0);
		exceptionErr(err, "receiveMSG() 실패");

		printf("%d - %s\n", rcvData.key, rcvData.value);
		insertData(rcvData.key, rcvData.value);
		showMap();

		// mainList.push_front(rcvData);
		// mainList.sort(compareByKey);
		// showList();
		// storage.insert(pair<unsigned int, string>(rcvData.key, rcvData.value));
	}
}

void receiveOP() {
	while(1) {
		t_data rcvData;
		// Request
		int err = msgrcv(requestQID, &rcvData, sizeof(t_data)-sizeof(long), GET_DATA, 0);
		exceptionErr(err, "receiveOP() 실패");

		string value = storage.find(rcvData.key) -> second;

		t_data sndData;
		sndData.data_type = GET_DATA;
		sndData.key = rcvData.key;
		strcpy(sndData.value, value.c_str());
		printf("key: %d, value %s\n", sndData.key, sndData.value);

		// Response
		err = msgsnd(responseQID, &sndData, sizeof(t_data) - sizeof(long), 0);
		exceptionErr(err, "receiveOP() -> msgsnd 실패");

	}
}

void receiveDel() {
	while(1) {
		t_data rcvData;
		int err = msgrcv(requestQID, &rcvData, sizeof(t_data)-sizeof(long), DEL_DATA, 0);
		exceptionErr(err, "receiveDel() 실패");
		map<unsigned int, string>::iterator iter = storage.find(rcvData.key);
		storage.erase(iter++);
		showMap();	
	}
	
}

void receiveCount() {
	while(1) {
		t_data rcvData;

		int err = msgrcv(requestQID, &rcvData, sizeof(t_data)-sizeof(long), DATA_COUNT, 0);
		exceptionErr(err, "receiveCount() 실패");

		t_data sndData;
		sndData.data_type = DATA_COUNT;
		sndData.key = storage.size();
		printf("size = %lu\n", storage.size());
		err = msgsnd(responseQID, &sndData, sizeof(t_data) - sizeof(long), 0);
		exceptionErr(err, "receiveCount() -> response 실패");		
	}
}