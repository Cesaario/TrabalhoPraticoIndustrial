#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <string>
#include <time.h>
#include <iostream>
#include "ListaCircular.h"
#include "DefeitoSuperficieTira.h"
#include "DadosDeProcesso.h"
#include "RandomUtil.h"

#define MENSAGEM_DEFEITO_SUPERFICIE_TIRA 1;
#define MENSAGEM_DADOS_PROCESSO_LAMINAÇÃO 2;

DWORD WINAPI Thread_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;

	HANDLE Evento_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Desbloquear_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Inspecao_Defeitos");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Nao_Cheia = OpenEvent(SYNCHRONIZE, false, "Evento_Lista_Circular_Nao_Cheia");
	HANDLE Evento_Lista_Circular_Contem_Defeito = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Defeito");
	HANDLE Evento_Lista_Circular_Contem_Dado_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Dado_Processo");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");

	do {
		Sleep(100);

		WaitForSingleObject(Evento_Desbloquear_Inspecao_Defeitos, INFINITE);

		int tipo = 0;
		std::string mensagem = "";
		if ((rand() % 100 < 50)) {
			mensagem = SerializarDefeitoTira(GerarDefeitoTira());
			tipo = 11;
		}
		else {
			mensagem = SerializarDadosProcesso(GerarDadosProcesso());
			tipo = 22;
		}

		int Status_Wait_Lista_Livre = WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Livres, 0);
		if (Status_Wait_Lista_Livre == WAIT_TIMEOUT) {
			printf("Lista circular cheia!!!\n");
			WaitForSingleObject(Evento_Lista_Circular_Nao_Cheia, INFINITE);
			continue;
		}

		WaitForSingleObject(Mutex_Acesso_Lista_Circular, INFINITE);
		std::string Mensagem_Atual = Lista_Circular_Memoria[GetPosicaoPonteiro()];;
		while (Mensagem_Atual.size() != 0){
			IncrementarPosicaoPonteiro();
			Mensagem_Atual = Lista_Circular_Memoria[GetPosicaoPonteiro()];
		};

		Lista_Circular_Memoria[GetPosicaoPonteiro()] = mensagem;
		std::cout << "Mensagem " << mensagem << " adicionada na posicao " << GetPosicaoPonteiro() << std::endl;
		IncrementarPosicaoPonteiro();
		ReleaseMutex(Mutex_Acesso_Lista_Circular);

		if (tipo == 11) {
			SetEvent(Evento_Lista_Circular_Contem_Defeito);
		}
		else if(tipo == 22) {
			SetEvent(Evento_Lista_Circular_Contem_Dado_Processo);
		}

		ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);

		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Inspecao_Defeitos, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando thread de inspecao de defeitos...\n");

	_endthreadex((DWORD)id);
	return id;
}