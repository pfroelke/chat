#pragma once
#include "sockets.h"

int disconnectClient(SOCKET &s) {
	closesocket(s);
	WSACleanup();
	return 1;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		printf("1");
		return (0);

	case WM_DESTROY:
		printf("2");
		return (0);

	case WM_CHANGECBCHAIN:
		printf("3");
		return (0);

	case WM_DRAWCLIPBOARD:
		printf("4");
		return (0);

	default:
		printf("def");
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}