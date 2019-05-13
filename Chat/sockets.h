#pragma once
#pragma comment(lib, "WSOCK32.lib")
#include "chat.h"

int connectAsClient(u_short port, SOCKET &s);
int disconnectClient(SOCKET &s);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);