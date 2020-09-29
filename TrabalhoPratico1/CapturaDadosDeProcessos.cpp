#include <windows.h>
#include <process.h>
#include <stdio.h>

DWORD WINAPI Thread_Captura_Dados_Processos(LPVOID thread_arg) {

	int id = (int)thread_arg;

	printf("Fechando captura dados processos\n");

	_endthreadex((DWORD)id);
	return id;
}