#include <windows.h>
#include <stdio.h>
#include <process.h>

DWORD WINAPI Thread_Leitura_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;
	int i = 0;

	HANDLE Evento_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Inspecao_Defeitos");

	do {
		//printf("%d\n", ++i);
		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Inspecao_Defeitos, 0);
		//printf("CODIGO: %d\n", resultadoEvento);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Ultimo erro %d\n", GetLastError());

	printf("Fechando inspeção de defeitos\n");

	_endthreadex((DWORD)id);
	return id;
}