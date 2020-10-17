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

DWORD WINAPI Thread_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento;

	HANDLE Evento_Nao_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Desbloquear_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Inspecao_Defeitos");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Nao_Cheia = OpenEvent(SYNCHRONIZE, false, "Evento_Lista_Circular_Nao_Cheia");
	HANDLE Evento_Lista_Circular_Contem_Defeito = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Defeito");
	HANDLE Evento_Lista_Circular_Contem_Dado_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Dado_Processo");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");

	HANDLE Mutex_Acesso_Console = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Console");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

	do {
		Sleep(1000);

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
			MostrarMensagem("Lista circular cheia!!!", VERMELHO);

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

		std::ostringstream Mensagem_Stream;
		Mensagem_Stream << "Mensagem \"" << mensagem << "\" adicionada na posicao " << GetPosicaoPonteiro() << " da lista.";
		MostrarMensagem(Mensagem_Stream.str(), BRANCO);

		IncrementarPosicaoPonteiro();
		ReleaseMutex(Mutex_Acesso_Lista_Circular);

		if (tipo == 11) {
			SetEvent(Evento_Lista_Circular_Contem_Defeito);
		}
		else if(tipo == 22) {
			SetEvent(Evento_Lista_Circular_Contem_Dado_Processo);
		}

		ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Inspecao_Defeitos, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	MostrarMensagem("Finalizando thread de inspecao de defeitos...", AMARELO);

	_endthreadex((DWORD)id);
	return id;
}