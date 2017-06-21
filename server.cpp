
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string>
#include <list>
#include <map>
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

map<unsigned int, string> mainMap;


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


void insertData(map<unsigned int, string> m, unsigned int key, char* value);
void showMap();
void readData();
void writeData();
void exceptionErr(int err, string s);
void receiveMSG();
void receiveOP();


int main() {
	readData();
	msqid = msgget(msgQKey, IPC_CREAT | 0666);

	if(msqid == -1) {
		perror("msgget() 실패");
		exit(1);
	}

	thread t1(&receiveMSG);
	thread t2(&receiveOP);
	t1.join();
	t2.join();

	return 0;
}


void exceptionErr(int err, string s) {
	if(err == -1) {
		perror(s.c_str());
		exit(1);
	}
}

void insertData(unsigned int key, char* value) {
	mainMap.insert(pair<unsigned int, string>(key, value));
	writeData();
}

void showMap() {
	printf("------- Show Map -------\n");
	map<unsigned int, string>::iterator iter;
	for(iter = mainMap.begin(); iter != mainMap.end(); iter++) {
		printf("%d - %s\n", iter -> first, iter -> second.c_str());
	}
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
	for(iter = mainMap.begin(); iter != mainMap.end(); iter++) {
		fprintf(file, "%d\t%s\n", iter -> first, iter -> second.c_str());
	}
	fclose(file);
}

void receiveMSG() {
	while(1){
		t_data rcvData;
		int err = msgrcv(msqid, &rcvData, sizeof(t_data)-sizeof(long), 1, 0);

		if(err == -1) {
			perror("receiveMSG() 실패");
			exit(1);
		}

		printf("%d - %s\n", rcvData.key, rcvData.value);
		// mainList.push_front(rcvData);
		// mainList.sort(compareByKey);
		// showList();
		// mainMap.insert(pair<unsigned int, string>(rcvData.key, rcvData.value));
		insertData(rcvData.key, rcvData.value);
		showMap();
	}
}

void receiveOP() {
	while(1) {
		t_data rcvData;
		int err = msgrcv(msqid, &rcvData, sizeof(t_data)-sizeof(long), 2, 0);

		if(err == -1) {
			perror("receiveOP() 실패");
			exit(1);
		}


		string value = mainMap.find(rcvData.key) -> second;

		t_data sndData;
		sndData.data_type = 2;
		sndData.key = rcvData.key;
		strcmp(sndData.value, value.c_str());
		
		printf("key: %d, value %s\n", rcvData.key, value.c_str());

		err = msgsnd(msqid, &sndData, sizeof(t_data) - sizeof(long), 0);
		exceptionErr(err, "receiveOP() -> msgsnd 실패");

	}
	
}