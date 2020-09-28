#include <windows.h>
#include <stdio.h>
#include <process.h>

HANDLE Timer_Inspecao_De_Defeitos;

DWORD WINAPI Thread_Leitura_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;

	int i = 0;
	while (i < 100000) {

		WaitForSingleObject(Timer_Inspecao_De_Defeitos, INFINITE);
		printf("%d\n", i);
		i++;
	}

	_endthreadex((DWORD)id);
	return id;
}