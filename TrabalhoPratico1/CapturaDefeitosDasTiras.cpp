#include <windows.h>
#include <process.h>
#include <stdio.h>

DWORD WINAPI Thread_Captura_Defeitos_Tiras(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;

	HANDLE Evento_Finalizar_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Defeitos_Das_Tiras");

	do {
		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Defeitos_Das_Tiras, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando thread de captura de defeitos das tiras...\n");

	_endthreadex((DWORD)id);
	return id;
}