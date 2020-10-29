#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "ListaCircular.h"
#include "DadosDeProcesso.h"
#include "Mensagens.h"
#include <iostream>

#define TAMANHO_ARQUIVO 46

DWORD WINAPI Thread_Captura_Dados_Processos(LPVOID thread_arg) {

	int id = (int)thread_arg;
	DWORD resultadoEvento = WAIT_OBJECT_0;

	HANDLE Evento_Nao_Finalizar_Dados_De_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Dados_De_Processo");
	HANDLE Evento_Desbloquear_Dados_De_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Dados_De_Processo");

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Evento_Lista_Circular_Contem_Dado_Processo = OpenEvent(SYNCHRONIZE, false, "Evento_Lista_Circular_Contem_Dado_Processo");
	HANDLE Evento_Lista_Circular_Nao_Cheia = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Nao_Cheia");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");

	HANDLE Semaforo_Arquivo_Dados_Processo_Livre = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Arquivo_Dados_Processo_Livre");
	HANDLE Evento_Arquivo_Nao_Cheio = OpenEvent(SYNCHRONIZE, false, "Evento_Arquivo_Nao_Cheio");
	HANDLE Mutex_Acesso_Arquivo = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Arquivo");

	HANDLE Mutex_Acesso_Console = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Console");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

	HANDLE Arquivo_Dados_De_Processo = CreateFile(
		"..\\DadosProcesso.txt",
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	HANDLE Pipe_Dados_De_Processo = CreateNamedPipe(
		"\\\\.\\pipe\\Pipe_Dados_De_Processo",
		PIPE_ACCESS_OUTBOUND,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
		1,
		sizeof(char) * 1000,
		sizeof(char) * 1000,
		1000,
		NULL
	);

	ConnectNamedPipe(Pipe_Dados_De_Processo, NULL);

	int Ponteiro_Escrita_Arquivos = 0;
	int Ponteiro_Leitura_Dados = 0;

	do {
		WaitForSingleObject(Evento_Desbloquear_Dados_De_Processo, INFINITE);
		
		int Status_Wait_Lista_Ocupada = WaitForSingleObject(Semaforo_Acesso_Lista_Circular_Ocupados, 0);
		if (Status_Wait_Lista_Ocupada == WAIT_TIMEOUT) {
			WaitForSingleObject(Evento_Lista_Circular_Contem_Dado_Processo, INFINITE);
			continue;
		}

		int Status_Semaforo_Arquivo = WaitForSingleObject(Semaforo_Arquivo_Dados_Processo_Livre, 0);
		if (Status_Semaforo_Arquivo == WAIT_TIMEOUT) {
			MostrarMensagem("Arquivo cheio!!!", VERMELHO);
			WaitForSingleObject(Evento_Arquivo_Nao_Cheio, INFINITE);
			MostrarMensagem("Arquivo liberado!!!", CIANO);
			continue;
		}
		ResetEvent(Evento_Arquivo_Nao_Cheio);

		WaitForSingleObject(Mutex_Acesso_Lista_Circular, INFINITE);
		std::string Proxima_Mensagem_Da_Fila = Lista_Circular_Memoria[Ponteiro_Leitura_Dados % TAMANHO_LISTA];
		DadosProcesso dados = DesserializarDadosProcesso(Proxima_Mensagem_Da_Fila);

		if (dados.tipo == 22) {
			Lista_Circular_Memoria[Ponteiro_Leitura_Dados % TAMANHO_LISTA] = "";
			
			/* std::ostringstream Mensagem_Stream;
			Mensagem_Stream << "[TIPO 22] Mensagem consumida! Posicao: " << Ponteiro_Leitura_Dados % TAMANHO_LISTA;
			MostrarMensagem(Mensagem_Stream.str(), VERDE); */

			DWORD Bytes_Escritos;

			WaitForSingleObject(Mutex_Acesso_Arquivo, INFINITE);
			LockFile(Arquivo_Dados_De_Processo, Ponteiro_Escrita_Arquivos * sizeof(char) * TAMANHO_ARQUIVO, NULL, sizeof(char) * TAMANHO_ARQUIVO, NULL);
			SetFilePointer(Arquivo_Dados_De_Processo, Ponteiro_Escrita_Arquivos * sizeof(char) * TAMANHO_ARQUIVO, NULL, FILE_BEGIN);
			WriteFile(
				Arquivo_Dados_De_Processo,
				Proxima_Mensagem_Da_Fila.c_str(),
				sizeof(char) * TAMANHO_ARQUIVO,
				&Bytes_Escritos,
				NULL
			);
			UnlockFile(Arquivo_Dados_De_Processo, Ponteiro_Escrita_Arquivos * sizeof(char) * TAMANHO_ARQUIVO, NULL, sizeof(char) * TAMANHO_ARQUIVO, NULL);
			ReleaseMutex(Mutex_Acesso_Arquivo);

			char Mensagem_Novos_Dados = '1';
			WriteFile(
				Pipe_Dados_De_Processo,
				&Mensagem_Novos_Dados,
				sizeof(char),
				&Bytes_Escritos,
				NULL
			);

			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Livres, 1, NULL);
			SetEvent(Evento_Lista_Circular_Nao_Cheia);
			Ponteiro_Escrita_Arquivos = (Ponteiro_Escrita_Arquivos + 1) % 100;
		}
		else {
			//A tarefa nao corresponde ao tipo procurado, portanto, nao vamos retira-la.
			ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 1, NULL);
			ReleaseSemaphore(Semaforo_Arquivo_Dados_Processo_Livre, 1, NULL);
		}

		Ponteiro_Leitura_Dados++;
		ReleaseMutex(Mutex_Acesso_Lista_Circular);

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Dados_De_Processo, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	MostrarMensagem("Finalizando thread de captura de dados de processo...", AMARELO);

	_endthreadex((DWORD)id);
	return id;
}