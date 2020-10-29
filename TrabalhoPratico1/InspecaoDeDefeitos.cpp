#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include "ListaCircular.h"
#include "DefeitoSuperficieTira.h"
#include "DadosDeProcesso.h"
#include "RandomUtil.h"
#include "Mensagens.h"

VOID CALLBACK Rotina_Mensagem_Dados_Processo(PVOID param, BOOLEAN TimerOrWaitFired) {

	HANDLE Evento_Timer_Dados_Processo_Executado = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Timer_Dados_Processo_Executado");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Contem_Dado_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Dado_Processo");

	std::string mensagem = SerializarDadosProcesso(GerarDadosProcesso());
	
	Adicionar_Mensagem_Na_Lista(mensagem);
	SetEvent(Evento_Lista_Circular_Contem_Dado_Processo);
	ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);

	SetEvent(Evento_Timer_Dados_Processo_Executado);
}

VOID CALLBACK Rotina_Mensagem_Defeitos_Tiras(PVOID param, BOOLEAN TimerOrWaitFired) {

	HANDLE Evento_Timer_Defeitos_Tiras_Executado = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Timer_Defeitos_Tiras_Executado");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Contem_Defeito = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Defeito");

	std::string mensagem = SerializarDefeitoTira(GerarDefeitoTira());

	Adicionar_Mensagem_Na_Lista(mensagem);
	SetEvent(Evento_Lista_Circular_Contem_Defeito);
	ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);

	SetEvent(Evento_Timer_Defeitos_Tiras_Executado);
}

void Criar_Timer_Dados_Processo(HANDLE* Temporizador_Dados_Processo, HANDLE* Fila_Temporizadores) {
	CreateTimerQueueTimer(
		Temporizador_Dados_Processo,
		*Fila_Temporizadores,
		(WAITORTIMERCALLBACK)Rotina_Mensagem_Dados_Processo,
		NULL,
		100,
		0,
		WT_EXECUTEDEFAULT
	);
}

void Criar_Timer_Defeitos_Tiras(HANDLE* Temporizador_Defeito_Tira, HANDLE* Fila_Temporizadores) {
	//int tempo = 100 + (rand() % 1900);
	int tempo = 250;
	CreateTimerQueueTimer(
		Temporizador_Defeito_Tira,
		*Fila_Temporizadores,
		(WAITORTIMERCALLBACK)Rotina_Mensagem_Defeitos_Tiras,
		NULL,
		tempo,
		0,
		WT_EXECUTEDEFAULT
	);
}

DWORD WINAPI Thread_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;

	HANDLE Evento_Nao_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Desbloquear_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Inspecao_Defeitos");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Evento_Lista_Circular_Nao_Cheia = OpenEvent(SYNCHRONIZE, false, "Evento_Lista_Circular_Nao_Cheia");
	HANDLE Mutex_Acesso_Console = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Console");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);


	HANDLE Evento_Timer_Dados_Processo_Executado = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Timer_Dados_Processo_Executado");
	HANDLE Evento_Timer_Defeitos_Tiras_Executado = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Timer_Defeitos_Tiras_Executado");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");

	HANDLE Fila_Temporizadores = CreateTimerQueue();
	HANDLE Temporizador_Dados_Processo;
	HANDLE Temporizador_Defeito_Tira;

	HANDLE Lista_Handles_Termino_Timers[] = { Evento_Timer_Dados_Processo_Executado, Evento_Timer_Defeitos_Tiras_Executado };

	Criar_Timer_Dados_Processo(&Temporizador_Dados_Processo, &Fila_Temporizadores);

	Criar_Timer_Defeitos_Tiras(&Temporizador_Defeito_Tira, &Fila_Temporizadores);

	do {

		WaitForSingleObject(Evento_Desbloquear_Inspecao_Defeitos, INFINITE);
		
		int Status_Wait = WaitForMultipleObjects(2, Lista_Handles_Termino_Timers, FALSE, INFINITE);

		int Status_Wait_Lista_Livre = WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Livres, 0);
		if (Status_Wait_Lista_Livre == WAIT_TIMEOUT) {
			MostrarMensagem("Lista circular cheia!!!", VERMELHO);
			ReleaseMutex(Mutex_Acesso_Lista_Circular);
			WaitForSingleObject(Evento_Lista_Circular_Nao_Cheia, INFINITE);
			continue;
		}

		//0: Dados Processo
		//1: Defeitos Tiras
		if (Status_Wait == WAIT_OBJECT_0 + 0) {
			DeleteTimerQueueTimer(
				Fila_Temporizadores,
				Temporizador_Dados_Processo,
				INVALID_HANDLE_VALUE
			);
			Criar_Timer_Dados_Processo(&Temporizador_Dados_Processo, &Fila_Temporizadores);
		}
		else if (Status_Wait == WAIT_OBJECT_0 + 1) {
			DeleteTimerQueueTimer(
				Fila_Temporizadores,
				Temporizador_Defeito_Tira,
				INVALID_HANDLE_VALUE
			);
			Criar_Timer_Defeitos_Tiras(&Temporizador_Defeito_Tira, &Fila_Temporizadores);
		}

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Inspecao_Defeitos, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	DeleteTimerQueueEx(Fila_Temporizadores, NULL);
	MostrarMensagem("Finalizando thread de inspecao de defeitos...", AMARELO);

	_endthreadex((DWORD)id);
	return id;
}