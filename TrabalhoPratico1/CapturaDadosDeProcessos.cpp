#include <windows.h>
#include <process.h>
#include <stdio.h>

DWORD WINAPI Thread_Captura_Dados_Processos(LPVOID thread_arg) {

	int id = (int)thread_arg;

	int i = 0;
	while (i < 100000) {
		//printf("%d\n", i);
		i++;
	}

	_endthreadex((DWORD)id);
	return id;
}