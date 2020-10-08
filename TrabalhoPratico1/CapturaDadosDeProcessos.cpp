#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string>
#include "ListaCircular.h"
#include "DadosDeProcesso.h"

int Ponteiro_Leitura_Dados = 0;

DWORD WINAPI Thread_Captura_Dados_Processos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento = WAIT_OBJECT_0;

	HANDLE Evento_Finalizar_Dados_De_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Dados_De_Processo");
	HANDLE Evento_Desbloquear_Dados_De_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Dados_De_Processo");

	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Semaforo_Acesso_Lista_Circular_Nao_Vazia = OpenSemaphore(SYNCHRONIZE, false, "Semaforo_Acesso_Lista_Circular_Nao_Vazia");

	HANDLE Mutex_Acesso_Lista_circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_circular");

	do {
		WaitForSingleObject(Evento_Desbloquear_Dados_De_Processo, INFINITE);
		
		int Status_Wait_Lista_Ocupada = WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Ocupados, 0);

		if (Status_Wait_Lista_Ocupada == WAIT_TIMEOUT) {
			printf("Lista circular vazia!!!\n");
			WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Nao_Vazia, INFINITE);
			continue;
		}

		WaitForSingleObject(Mutex_Acesso_Lista_circular, INFINITE);
		
		std::string Proxima_Mensagem_Da_Fila = Lista_Circular_Memoria[Ponteiro_Leitura_Dados];
		DadosProcesso dados = DesserializarDadosProcesso(Proxima_Mensagem_Da_Fila);
		if (dados.tipo == 22) {
			Lista_Circular_Memoria[Ponteiro_Leitura_Dados] = "";
			printf("Mensagem de dados lida!\n");
		}
		else {
			//A tarefa n�o corresponde ao tipo procurado, portanto, n�o vamos retir�-la.
			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);
		}

		Ponteiro_Leitura_Dados++;
		ReleaseMutex(Mutex_Acesso_Lista_circular);

		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Dados_De_Processo, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando thread de captura de dados de processo...\n");

	_endthreadex((DWORD)id);
	return id;
}