#include <windows.h>
#include <process.h>
#include <stdio.h>

DWORD WINAPI Thread_Captura_Dados_Processos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;

	HANDLE Evento_Finalizar_Dados_De_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Dados_De_Processo");
	HANDLE Evento_Desbloquear_Dados_De_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Dados_De_Processo");

	do {
		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Dados_De_Processo, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando thread de captura de dados de processo...\n");

	_endthreadex((DWORD)id);
	return id;
}