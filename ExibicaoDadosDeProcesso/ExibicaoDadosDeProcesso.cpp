#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>
#include <iostream>

#define TAMANHO_ARQUIVO 46


int main()
{
	printf("Processo de exibicao de dados de processo iniciando...\n");

	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	printf("DIR: %s\n", NPath);

	DWORD resultadoEvento = 0;
	DWORD resultadoWait = 0;

	HANDLE Evento_Nao_Finalizar_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Exibicao_De_Dados");
	HANDLE Evento_Desbloquear_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Exibicao_De_Dados");
	HANDLE Evento_Limpar_Janela = OpenEvent(SYNCHRONIZE, false, "Evento_Limpar_Janela");

	HANDLE Semaforo_Arquivo_Dados_Processo_Livre = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Arquivo_Dados_Processo_Livre");
	HANDLE Evento_Arquivo_Nao_Cheio = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Arquivo_Nao_Cheio");

	HANDLE Handles_Tarefa_Exibicao_De_Dados[] = { Evento_Limpar_Janela, Evento_Desbloquear_Exibicao_De_Dados };

	HANDLE Arquivo_Dados_De_Processo = CreateFile(
		"..\\DadosProcesso.txt",
		GENERIC_READ,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (Arquivo_Dados_De_Processo == INVALID_HANDLE_VALUE)
	{
		printf("a8sgdy8ag %d\n", GetLastError());
	}
	printf("Com stringaAASDASDA! %d\n", GetLastError());

	int contador = 0;
	int Ponteiro_Leitura_Arquivo = 0;

	do {
		resultadoWait = WaitForMultipleObjects(2, Handles_Tarefa_Exibicao_De_Dados, FALSE, INFINITE);
		
		SetFilePointer(Arquivo_Dados_De_Processo, Ponteiro_Leitura_Arquivo * sizeof(char) * TAMANHO_ARQUIVO, NULL, FILE_BEGIN);
		LockFile(Arquivo_Dados_De_Processo, Ponteiro_Leitura_Arquivo * sizeof(char) * TAMANHO_ARQUIVO, 0, (Ponteiro_Leitura_Arquivo + 1) * sizeof(char) * TAMANHO_ARQUIVO, 0);
		DWORD Bytes_Lidos;
		char Mensagem_Arquivo[TAMANHO_ARQUIVO];
		bool res = ReadFile(
			Arquivo_Dados_De_Processo,
			&Mensagem_Arquivo,
			sizeof(char) * TAMANHO_ARQUIVO,
			&Bytes_Lidos,
			NULL
		);
		std::cout << std::string(Mensagem_Arquivo) << std::endl;
		UnlockFile(Arquivo_Dados_De_Processo, Ponteiro_Leitura_Arquivo * sizeof(char) * TAMANHO_ARQUIVO, 0, (Ponteiro_Leitura_Arquivo + 1) * sizeof(char) * TAMANHO_ARQUIVO, 0);

		Ponteiro_Leitura_Arquivo++;

		ReleaseSemaphore(Semaforo_Arquivo_Dados_Processo_Livre, 1, NULL);
		SetEvent(Evento_Arquivo_Nao_Cheio);

		Sleep(1000);

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Exibicao_De_Dados, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando processo de exibicao de dados de processo...\n");
}
