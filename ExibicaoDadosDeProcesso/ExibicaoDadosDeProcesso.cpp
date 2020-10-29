#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>
#include <iostream>
#include "DadosDeProcesso.h"

#define TAMANHO_ARQUIVO 46

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

int Ponteiro_Leitura_Arquivo = 0;
bool Finalizar = false;
HANDLE Pipe_Limpar_Tela;
HANDLE Handle_Thread_Limpeza_Tela;

DWORD WINAPI Thread_Limpeza_Tela(LPVOID thread_arg) {
	WaitNamedPipe("Pipe_Dados_De_Processo", NMPWAIT_USE_DEFAULT_WAIT);
	Pipe_Limpar_Tela = CreateFile(
		"\\\\.\\pipe\\Pipe_Limpar_Tela",
		GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL
	);

	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

	while (!Finalizar) {
		char Buffer_Mensagem_Pipe;
		ReadFile(Pipe_Limpar_Tela, &Buffer_Mensagem_Pipe, sizeof(char), NULL, NULL);
		system("cls");
		SetConsoleTextAttribute(Handle_Console, FOREGROUND_INTENSITY);
		std::cout << "Tela limpa!" << std::endl;
		SetConsoleTextAttribute(Handle_Console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}

	_endthreadex((DWORD)6);
	return 6;
}

int main()
{
	printf("Processo de exibicao de dados de processo iniciando...\n");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(Handle_Console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	DWORD resultadoEvento = 0;
	DWORD resultadoWait = 0;

	HANDLE Evento_Nao_Finalizar_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Exibicao_De_Dados");
	HANDLE Evento_Desbloquear_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Exibicao_De_Dados");

	HANDLE Semaforo_Arquivo_Dados_Processo_Livre = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Arquivo_Dados_Processo_Livre");
	HANDLE Evento_Arquivo_Nao_Cheio = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Arquivo_Nao_Cheio");
	HANDLE Mutex_Acesso_Arquivo = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Arquivo");

	HANDLE Handles_Tarefa_Exibicao_De_Dados[] = { Evento_Desbloquear_Exibicao_De_Dados };

	DWORD dwThreadID;
	Handle_Thread_Limpeza_Tela = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Limpeza_Tela,
		(void*)
		6,
		0,
		(CAST_LPDWORD)&dwThreadID
	);

	WaitNamedPipe("Pipe_Dados_De_Processo", NMPWAIT_USE_DEFAULT_WAIT);
	HANDLE Pipe_Dados_De_Processo = CreateFile(
		"\\\\.\\pipe\\Pipe_Dados_De_Processo",
		GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL
	);

	HANDLE Arquivo_Dados_De_Processo = CreateFile(
		"..\\DadosProcesso.txt",
		GENERIC_READ,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	int contador = 0;

	do {
		resultadoWait = WaitForSingleObject(Evento_Desbloquear_Exibicao_De_Dados, INFINITE);
		
		char Buffer_Mensagem_Pipe;
		ReadFile(Pipe_Dados_De_Processo, &Buffer_Mensagem_Pipe, sizeof(char), NULL, NULL);

		if (Buffer_Mensagem_Pipe == '1') {
			WaitForSingleObject(Mutex_Acesso_Arquivo, INFINITE);
			LockFile(Arquivo_Dados_De_Processo, Ponteiro_Leitura_Arquivo * sizeof(char) * TAMANHO_ARQUIVO, 0, (Ponteiro_Leitura_Arquivo + 1) * sizeof(char) * TAMANHO_ARQUIVO, 0);
			SetFilePointer(Arquivo_Dados_De_Processo, Ponteiro_Leitura_Arquivo * sizeof(char) * TAMANHO_ARQUIVO, NULL, FILE_BEGIN);
			DWORD Bytes_Lidos;
			char Mensagem_Arquivo[TAMANHO_ARQUIVO];
			bool res = ReadFile(
				Arquivo_Dados_De_Processo,
				&Mensagem_Arquivo,
				sizeof(char) * TAMANHO_ARQUIVO,
				&Bytes_Lidos,
				NULL
			);

			std::string Mensagem_Formatada = FormatarDadosProcesso(DesserializarDadosProcesso(Mensagem_Arquivo));
			std::cout << Mensagem_Formatada << std::endl;

			UnlockFile(Arquivo_Dados_De_Processo, Ponteiro_Leitura_Arquivo * sizeof(char) * TAMANHO_ARQUIVO, 0, (Ponteiro_Leitura_Arquivo + 1) * sizeof(char) * TAMANHO_ARQUIVO, 0);
			ReleaseMutex(Mutex_Acesso_Arquivo);

			Ponteiro_Leitura_Arquivo = (Ponteiro_Leitura_Arquivo + 1) % 100;

			ReleaseSemaphore(Semaforo_Arquivo_Dados_Processo_Livre, 1, NULL);
			SetEvent(Evento_Arquivo_Nao_Cheio);
		}

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Exibicao_De_Dados, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	Finalizar = true;
	printf("Finalizando processo de exibicao de dados de processo...\n");
	WaitForSingleObject(Handle_Thread_Limpeza_Tela, INFINITE);
}
