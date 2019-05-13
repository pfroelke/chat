#pragma once
#include "utils.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

void pushMsg(char* msg, int lengthMsg, char* usr, int lengthUsr, struct chat& chatPtr);
void pushSystemMsg(char* msg, struct chat& chatPtr);
void login(char** usr, int* length);
void logout(char** usr);
void welcomeMsg(char* usr);
void showUser(char* usr);
void showMsg(struct message* msgPtr);
void showMsgs(struct chat& chatPtr);
void delMsgs(struct chat& chatPtr);
chat* createChat();
void deleteChat(struct chat *chatPtr);
char* findInMsg(struct message* msgPtr, char* toFind);
int findInChat(char* toFind, struct chat& chatPtr, int start, int step);
void loadMsg(struct chat& chatPtr, char *usr);
void saveMsg(struct chat& chatPtr, int n);
void mockMsgs(struct chat& chatPtr);