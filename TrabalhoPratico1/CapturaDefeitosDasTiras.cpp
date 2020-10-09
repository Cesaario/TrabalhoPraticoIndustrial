#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string>
#include "ListaCircular.h"
#include "DefeitoSuperficieTira.h"

#define YELLOW   FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define BLUE   FOREGROUND_BLUE | FOREGROUND_INTENSITY

int Ponteiro_Leitura_Defeitos = 0;

DWORD WINAPI Thread_Captura_Defeitos_Tiras(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento = WAIT_OBJECT_0;

	HANDLE Evento_Nao_Finalizar_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Defeitos_Das_Tiras");
	HANDLE Evento_Desbloquear_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Defeitos_Das_Tiras");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Contem_Defeito = OpenEvent(SYNCHRONIZE, false, "Evento_Lista_Circular_Contem_Defeito");
	HANDLE Evento_Lista_Circular_Nao_Cheia = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Nao_Cheia");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");

	HANDLE Mutex_Acesso_Console = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Console");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

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

			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, BLUE);
			printf("Mensagem [TIPO 11] consumida! Pos: %d\n", Ponteiro_Leitura_Defeitos % TAMANHO_LISTA);
			ReleaseMutex(Mutex_Acesso_Console);

			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Livres, 1, NULL);
			SetEvent(Evento_Lista_Circular_Nao_Cheia);
		}
		else {
			//A tarefa n�o corresponde ao tipo procurado, portanto, n�o vamos retir�-la.
			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);
		}

		Ponteiro_Leitura_Defeitos++;
		ReleaseMutex(Mutex_Acesso_Lista_Circular);

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Defeitos_Das_Tiras, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
	SetConsoleTextAttribute(Handle_Console, YELLOW);
	printf("Finalizando thread de captura de defeitos das tiras...\n");
	ReleaseMutex(Mutex_Acesso_Console);

	_endthreadex((DWORD)id);
	return id;
}