#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string>
#include "ListaCircular.h"
#include "DefeitoSuperficieTira.h"

int Ponteiro_Leitura_Defeitos = 0;

DWORD WINAPI Thread_Captura_Defeitos_Tiras(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento = WAIT_OBJECT_0;

	HANDLE Evento_Finalizar_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Defeitos_Das_Tiras");
	HANDLE Evento_Desbloquear_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Defeitos_Das_Tiras");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Contem_Defeito = OpenEvent(SYNCHRONIZE, false, "Evento_Lista_Circular_Contem_Defeito");
	HANDLE Evento_Lista_Circular_Nao_Cheia = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Nao_Cheia");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");


	do {
		WaitForSingleObject(Evento_Desbloquear_Defeitos_Das_Tiras, INFINITE);
		
		int Status_Wait_Lista_Ocupada = WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Ocupados, 0);

		if (Status_Wait_Lista_Ocupada == WAIT_TIMEOUT) {
			WaitForSingleObject(Evento_Lista_Circular_Contem_Defeito, INFINITE);
			continue;
		}

		WaitForSingleObject(Mutex_Acesso_Lista_Circular, INFINITE);

		std::string Proxima_Mensagem_Da_Fila = Lista_Circular_Memoria[Ponteiro_Leitura_Defeitos % TAMANHO_LISTA];
		DefeitoTira defeito = DesserializarDefeitoTira(Proxima_Mensagem_Da_Fila);
		if (defeito.tipo == 11) {
			Lista_Circular_Memoria[Ponteiro_Leitura_Defeitos % TAMANHO_LISTA] = "";
			printf("Mensagem [TIPO 11] consumida! Pos: %d\n", Ponteiro_Leitura_Defeitos % TAMANHO_LISTA);
			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Livres, 1, NULL);
			SetEvent(Evento_Lista_Circular_Nao_Cheia);
		}
		else {
			//A tarefa não corresponde ao tipo procurado, portanto, não vamos retirá-la.
			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);
		}

		Ponteiro_Leitura_Defeitos++;
		ReleaseMutex(Mutex_Acesso_Lista_Circular);

		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Defeitos_Das_Tiras, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando thread de captura de defeitos das tiras...\n");

	_endthreadex((DWORD)id);
	return id;
}