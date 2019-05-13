#pragma once
#include "utils.h"

struct ANYTYPE
{
	enum {
		typUndefined,
		typInt,           // 1
		typUint,
		typString,
		typByteString,
		typLong,          // 5
		typFloat,
		typDouble,
	} iType;

	void* value;
};

char* getFirstWord(char** str) {
	char *toReturn = *str, *temp = NULL;
	strtok_s(*str, " ", &temp);
	*str = temp;
	return toReturn;
}

const unsigned long hashStr(const char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}