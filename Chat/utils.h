#pragma once
#include <string.h>

#define ST_PORT 9090
#define MSGTIME 10 // Time after which a message will be deleted (in seconds)
#define THREADS 4 // Thread amount
#define LOGIN 2025333163 // -login
#define LOAD 191524050 // -load
#define LOGOUT 2411491820 // -logout
#define EXIT 191282572 // -exit
#define MOCK 191560060 // -mock n
#define SAVE 191761057 // -save n
#define FIND 191302323 // -find "x"
#define CONNECT 3539756540 // -connect port

char* getFirstWord(char** str);
const unsigned long hashStr(const char *str);
