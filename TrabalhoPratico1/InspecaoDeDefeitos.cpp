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

DWORD WINAPI Thread_Leitura_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;

	HANDLE Evento_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Desbloquear_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Inspecao_Defeitos");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Semaforo_Acesso_Lista_Circular_Cheia = OpenSemaphore(SYNCHRONIZE, false, "Semaforo_Acesso_Lista_Circular_Cheia");

	do {
		WaitForSingleObject(Evento_Desbloquear_Inspecao_Defeitos, INFINITE);
		Sleep(1000);

		std::string mensagem = ((rand() % 100 < 50)) ? SerializarDefeitoTira(GerarDefeitoTira()) : SerializarDadosProcesso(GerarDadosProcesso());

		int Status_Wait_Lista_Livre = WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Livres, 0);

		if (Status_Wait_Lista_Livre == WAIT_TIMEOUT) {
			printf("Lista circular cheia!!!\n");
			WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Cheia, INFINITE);
			continue;
		}

		Lista_Circular_Memoria[GetPosicaoLivre()] = mensagem;
		//printf("Adicionado em %d: %s\n", GetPosicaoLivre(), mensagem);
		std::cout << mensagem << std::endl;
		IncrementarPosicaoLivre();
		ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);

		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Inspecao_Defeitos, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando thread de inspeção de defeitos...\n");

	_endthreadex((DWORD)id);
	return id;
}