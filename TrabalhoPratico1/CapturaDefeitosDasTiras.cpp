#include <windows.h>
#include <process.h>
#include <stdio.h>

DWORD WINAPI Thread_Captura_Defeitos_Tiras(LPVOID thread_arg) {

	int id = (int)thread_arg;
	printf("Fechando captura tiras\n");

	_endthreadex((DWORD)id);
	return id;
}