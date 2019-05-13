#pragma once
#include "chat.h"
#include "sockets.h"
#include "utils.h"
//#include <vld.h>

struct chat* gChat;
char* user = NULL;
int usrLength;
SOCKET s = NULL;
SOCKET sis[5];
int users = 0;
bool server = false;

struct thrSock
{
	struct chat *chatPtr;
	SOCKET si;
};

DWORD WINAPI acceptThreadFunc(void *arg)
{
	struct thrSock *my_data = (struct thrSock *)arg;
	SOCKET si;
	if (server) si = my_data->si;
	else si = s;
	char* buff = (char*)malloc(256 * (sizeof(char)));
	char* old = buff;
	sis[users++] = si;

	while (recv(si, buff, 256, 0) > 0)
	{
		if (strcmp(buff, "KONIEC") == 0)
		{
			closesocket(si);
			WSACleanup();
			free(buff);
			return 0;
		}
		if (server) {
			for (int i = 0; i < users; i++) {
				if (sis[i] != si) send(sis[i], buff, strlen(buff) + 1, 0);
			}
		}
		char* userL = getFirstWord(&buff);
		pushMsg(buff, strlen(buff) + 1, userL, strlen(userL) + 1, *gChat);
		system("cls");
		delMsgs(*gChat);
		showMsgs(*gChat);
		showUser(user);
		buff = old;
	};
	free(buff);
	return 0;
}

DWORD WINAPI serverThread(void *arg)
{
	server = true;
	struct thrSock *my_data = (struct thrSock *)arg;
	WSADATA wsas;
	int result;
	WORD wersja;
	wersja = MAKEWORD(1, 1);
	result = WSAStartup(wersja, &wsas);

	//SOCKET  s;
	s = socket(AF_INET, SOCK_STREAM, 0);

	struct  sockaddr_in  sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(ST_PORT);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	result = bind(s, (struct sockaddr FAR*)&sa, sizeof(sa));
	result = listen(s, 5);

	SOCKET  si;
	struct  sockaddr_in  sc;
	int  lenc;
	for (;;)
	{
		lenc = sizeof(sc);
		si = accept(s, (struct  sockaddr  FAR *) &sc, &lenc);

		thrSock data;
		data.si = si;
		//data.chatPtr = my_data->chatPtr;
		DWORD id;
		HANDLE acceptThread = CreateThread(
			NULL,				// atrybuty bezpieczeñstwa
			0,					// inicjalna wielkoœæ stosu
			acceptThreadFunc,	// funkcja w¹tku
			(void *)&data,		// dane dla funkcji w¹tku
			0,					// flagi utworzenia
			&id);
		if (acceptThread != INVALID_HANDLE_VALUE)
			SetThreadPriority(acceptThread, THREAD_PRIORITY_NORMAL);

	}
	return 0;
}

int openServer(struct chat& chatPtr) {
	DWORD id;
	thrSock data;
	data.chatPtr = &chatPtr;
	HANDLE server = CreateThread(
		NULL,					// atrybuty bezpieczeñstwa
		0,						// inicjalna wielkoœæ stosu
		serverThread,			// funkcja w¹tku
		(void *)&data,			// dane dla funkcji w¹tku
		0,						// flagi utworzenia
		&id);
	if (server != INVALID_HANDLE_VALUE)
		SetThreadPriority(serverThread, THREAD_PRIORITY_NORMAL);
	return 1;
}

int connectAsClient(u_short port) {
	struct sockaddr_in sa;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	WSAStartup(wersja, &wsas);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr("192.168.0.178");

	int result;
	result = connect(s, (struct sockaddr FAR *) &sa, sizeof(sa));

	if (result == SOCKET_ERROR)
	{
		return 0;
	}
	thrSock data;
	data.si = s;
	//data.chatPtr = my_data->chatPtr;
	DWORD id;
	HANDLE acceptThread = CreateThread(
		NULL,				// atrybuty bezpieczeñstwa
		0,					// inicjalna wielkoœæ stosu
		acceptThreadFunc,	// funkcja w¹tku
		(void *)&data,		// dane dla funkcji w¹tku
		0,					// flagi utworzenia
		&id);
	if (acceptThread != INVALID_HANDLE_VALUE)
		SetThreadPriority(acceptThread, THREAD_PRIORITY_NORMAL);
	return 1;
}

int readCommand(struct chat& chatPtr, char* buff, char* buff2) {
	int strLength;
	char * firstWord = NULL;
	fgets(buff, 100, stdin);
	strLength = strlen(buff);
	buff[strLength - 1] = '\0';
	buff[strLength] = -52;
	strcpy_s(buff2, strLength, buff);
	if (user == NULL) {
		switch (hashStr(getFirstWord(&buff))) {
		case LOGIN:
			login(&user, &usrLength);
			welcomeMsg(user);
			break;
		case EXIT:
			return false;
		default:
			printf("Please log in! (-login)\n");
			break;
		}
	}
	else {
		if (buff[0] == '-') {
			int n = 0;
			switch (hashStr(getFirstWord(&buff))) {
			case SAVE:
				n = atoi(getFirstWord(&buff));
				saveMsg(chatPtr, n);
				break;
			case LOAD:
				loadMsg(chatPtr, user);
				break;
			case LOGIN:
				pushSystemMsg("To change user please logout first!", chatPtr);
				break;
			case EXIT:
				if (s != NULL) {
					send(s, "KONIEC", 8, 0);
				}
				return false;
			case LOGOUT:
				system("cls");
				logout(&user);
				return true;
			case MOCK:
				n = atoi(getFirstWord(&buff));
				for (int i = 0; i < n; i++) {
					mockMsgs(chatPtr);
				}
				break;
			case FIND:
			{
				clock_t start, end;
				start = clock();
				int occurances = findInChat(getFirstWord(&buff), chatPtr, 0, 1);
				end = clock();
				char* tempBuff = (char*)malloc(100 * (sizeof(char)));
				snprintf(tempBuff, 100, "Found %d occurances in %fms!", occurances, ((double)(end - start)));
				pushSystemMsg(tempBuff, chatPtr);
				free(tempBuff);
				break;
			}
			case CONNECT:
			{
				n = atoi(getFirstWord(&buff));
				int error = connectAsClient((u_short)n);
				if (error == 0) {
					openServer(chatPtr);
				}
				break;
			}
			default:
				pushSystemMsg("Command not recognized!", chatPtr);
				break;
			}
		}
		else {
			if (s != NULL) {
				char* tempBuff = (char*)malloc(256 * (sizeof(char)));
				snprintf(tempBuff, 256, "%s %s", user, buff2);
				int len = strlen(tempBuff);
				for (int i = 0; i < users; i++) {
					send(sis[i], tempBuff, len + 1, 0);
				}
				free(tempBuff);
			}
			pushMsg(buff2, strLength, user, usrLength, chatPtr);
		}
		system("cls");
		delMsgs(chatPtr);
		showMsgs(chatPtr);
		showUser(user);
	}
	return true;
}

void readCommands(struct chat &chatPtr) {
	char* buff = (char*)malloc(100 * (sizeof(char)));
	char* buff2 = (char*)malloc(100 * (sizeof(char)));
	printf("Start by logging in! (-login)\n");
	while (readCommand(chatPtr, buff, buff2) != 0);
	free(buff);
	free(buff2);
}

int main(void) {
	const int threads_amount = 16;
	HANDLE threads[threads_amount];
	DWORD id;
	struct chat* chat = NULL;
	clock_t start, end;
	double time_taken;

	chat = createChat();
	gChat = chat;
	readCommands(*chat);

	// printf("-login: %lu", hashStr("-connect"));

	logout(&user);
	deleteChat(chat);
	return 0;
}