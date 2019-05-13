#pragma once
#include "chat.h"

struct thread_data {
	struct chat* chatPtr;
	int start, step, *returnValue;
	char *toFind;
};

struct message {
	struct message* previous;
	__time64_t* time;
	char* messageStr;
	char* user;
	struct message* next;
};

struct chat {
	struct message* top;
	int iMsgs;
};

void pushMsg(char* msg, int lengthMsg, char* usr, int lengthUsr, struct chat& chatPtr) {
	struct message* stack;
	stack = chatPtr.top;
	chatPtr.top = (struct message*)malloc(sizeof(struct message));
	chatPtr.top->previous = NULL;
	chatPtr.top->messageStr = (char*)malloc(lengthMsg*(sizeof(char)));
	strcpy_s(chatPtr.top->messageStr, lengthMsg, msg);
	chatPtr.top->user = (char*)malloc(lengthUsr*(sizeof(char)));
	strcpy_s(chatPtr.top->user, lengthUsr, usr);
	chatPtr.top->time = (__time64_t*)malloc(sizeof(__time64_t));
	_time64(chatPtr.top->time);
	chatPtr.top->next = stack;
	if (stack != NULL) {
		stack->previous = chatPtr.top;
	}
	return;
}

void pushSystemMsg(char* msg, struct chat& chatPtr) {
	pushMsg(msg, strlen(msg) + 1, "System", 8, chatPtr);
	return;
}

void login(char** usr, int* length) {
	char buffer[100];
	printf("Enter your nickname: \n");
	fgets(buffer, 100, stdin);
	*length = strlen(buffer);
	buffer[*length - 1] = '\0';
	buffer[*length] = -52;
	*usr = (char*)malloc((*length)*(sizeof(char)));
	strcpy_s(*usr, *length, buffer);
}

void logout(char** usr) {
	free(*usr);
	*usr = NULL;
	return;
}

void welcomeMsg(char* usr) {
	printf("Welcome %s!\nPress enter to continue...\n", usr);
}

void showUser(char* usr) {
	printf("%s: ", usr);
}

void showMsg(struct message* msgPtr) {
	char timebuff[100];
	struct tm buf;
	errno_t error;
	int length;

	error = localtime_s(&buf, msgPtr->time);
	if (error) {
		printf("Time error!");
		return;
	}

	asctime_s(timebuff, sizeof(timebuff), &buf);
	length = strlen(timebuff);
	timebuff[length - 1] = '\0';
	timebuff[length] = -52;
	printf("%s - %s: %s \n", timebuff, msgPtr->user, msgPtr->messageStr);
}

void showMsgs(struct chat& chatPtr) {
	struct message* nexts;
	nexts = chatPtr.top;
	int i = 29;
	while (nexts != NULL && i > 0) {
		showMsg(nexts);
		nexts = nexts->next;
		i--;
	}
	while (i > 0) {
		printf("\n");
		i--;
	}
	return;
}

void delMsgs(struct chat& chatPtr) {
	struct message* nexts, *temp;
	nexts = chatPtr.top;
	while (nexts != NULL) {
		if (time(NULL) - (*nexts->time) > MSGTIME) {
			if (nexts->previous != NULL) {
				nexts->previous->next = NULL;
				nexts->previous = NULL;
			}
			temp = nexts;
			while (temp != NULL) {
				temp = nexts->next;
				free(nexts->messageStr);
				free(nexts->time);
				free(nexts->user);
				delete(nexts);
				nexts = temp;
			}
		}
		else nexts = nexts->next;
	}
	return;
}


chat* createChat() {
	chat* toReturn;
	toReturn = (struct chat*)malloc(sizeof(struct chat));
	toReturn->top = NULL;
	toReturn->iMsgs = 0;
	return toReturn;
}

void deleteChat(struct chat *chatPtr) {
	free((void *)chatPtr->iMsgs);
	free(chatPtr);
	return;
}

char* findInMsg(struct message* msgPtr, char* toFind) {
	for (int i = 0; i < 100; i++)
	{
		strstr(msgPtr->messageStr, toFind);
	}
	return strstr(msgPtr->messageStr, toFind);
}

int findInChat(char* toFind, struct chat& chatPtr, int start, int step) {
	int toReturn = 0;
	struct message* nexts;
	nexts = chatPtr.top;

	while (nexts != NULL && start > 0) {
		nexts = nexts->next;
		start--;
	}

	while (nexts != NULL) {
		if (findInMsg(nexts, toFind)) {
			//showMsg(nexts);
			toReturn++;
		}
		for (int i = 0; nexts != NULL && i < step; i++) {
			nexts = nexts->next;
		}
	}

	return toReturn;
}

void loadMsg(struct chat& chatPtr, char *usr) {
	if (IsClipboardFormatAvailable(CF_TEXT)) {
		OpenClipboard(GetConsoleWindow());
		static HANDLE hCbMem;
		hCbMem = GetClipboardData(CF_TEXT);
		CloseClipboard();

		int strLength = strlen((char *)hCbMem);
		char* buff = (char*)malloc(100 * (sizeof(char)));
		strcpy_s(buff, strLength + 1, (char *)hCbMem);
		buff[strLength - 1] = '\0';
		buff[strLength] = -52;
		pushMsg((char *)hCbMem, strLength + 1, usr, strlen(usr) + 1, chatPtr);
	}
	return;
}

void saveMsg(struct chat& chatPtr, int n) {
	struct message* nexts;
	nexts = chatPtr.top;
	while (nexts != NULL && n > 1) {
		nexts = nexts->next;
		n--;
	}
	if (nexts == NULL) return;
	HGLOBAL handle = GlobalAlloc(GMEM_FIXED, strlen(nexts->messageStr) + 1);
	memcpy(handle, nexts->messageStr, strlen(nexts->messageStr));

	if (OpenClipboard(GetConsoleWindow())) {
		EmptyClipboard();
		SetClipboardData(CF_TEXT, handle);
		CloseClipboard();
	}
	return;
}

void mockMsgs(struct chat& chatPtr) { // mocks 10200 messages
	struct message* top = chatPtr.top;

	const char *filename = "lorem.txt";
	FILE *file;
	errno_t err = fopen_s(&file, filename, "r");
	char buff[256];
	int strLength = 0;

	while (fgets(buff, sizeof(buff), file)) {
		strLength = strlen(buff);
		buff[strLength - 1] = '\0';
		buff[strLength] = -52;
		pushMsg(buff, strLength, (char*)"Test", 5, chatPtr);
	}
}

//HANDLE* initThreads() {
//	HANDLE threads[THREADS]; // toReturn
//
//	return threads;
//}
//
//void runThreads(void* f, HANDLE* threads, void* toReturn) {
//
//}
//
//bool sumThreadReturn(ANYTYPE* returnTab, ANYTYPE& toReturn) {
//	returnTab->value = { (void*)2 };
//	int a = 0;
//	toReturn.value = (void*)10;
//	//int a = (int)toReturn.value;
//	for (int i = 0; i < THREADS; i++) {
//		a += (int)returnTab[i].value;
//	}
//	return true;
//}
//
//DWORD WINAPI threadFunc(LPVOID lpParam)
//{
//	struct thread_data data = *(struct thread_data *)lpParam;
//	clock_t start, end;
//	double time_taken;
//	start = clock();
//
//	*data.returnValue = findInChat(data.toFind, *data.chatPtr, data.start, data.step);
//
//	end = clock();
//	time_taken = ((double)(end - start));
//
//	printf("Found %d occurances in %fms!\n", *data.returnValue, time_taken);
//	return 0;
//}


//start = clock();
//int occurances = findInChat("lorem", *chat, 0, 1);
//end = clock();
//time_taken = ((double)(end - start));
//printf("Found %d occurances in %fms!\n", occurances, time_taken);

//ANYTYPE return_t_data[THREADS];
//ANYTYPE returned;
//returned.value = (void *)10;
//char* toFind = "lorem";
//struct thread_data t_data[threads_amount];
//for (int i = 0; i < threads_amount; i++) {
//	t_data[i] = { chat, 0, threads_amount, &return_t_data[i], toFind };
//}

//start = clock();

//for (int i = 0; i < threads_amount; i++) {
//	t_data[i].start = i;
//	threads[i] = CreateThread(
//		NULL, // atrybuty bezpieczeñstwa
//		0, // inicjalna wielkoœæ stosu
//		threadFunc, // funkcja w¹tku
//		&t_data[i],// dane dla funkcji w¹tku
//		0, // flagi utworzenia
//		&id);

//	if (threads[i] != INVALID_HANDLE_VALUE)
//	{
//		printf("Thread created! - %x\n", id);
//		SetThreadPriority(threads[i], THREAD_PRIORITY_NORMAL);
//	}
//}
//
//WaitForMultipleObjects(threads_amount, threads, TRUE, INFINITE);

//for (int i = 0; i < threads_amount; i++) {
//	CloseHandle(threads[i]);
//}

//sumThreadReturn(*return_t_data, (void*)returned);

//end = clock();
//time_taken = ((double)(end - start));

//printf("Found %d occurances in total in %fms!\n", returned, time_taken);