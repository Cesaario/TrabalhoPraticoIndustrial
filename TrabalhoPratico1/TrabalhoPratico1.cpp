#pragma warning(disable:4996)
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>
#include <time.h>
#include "InspecaoDeDefeitos.h"
#include "CapturaDefeitosDasTiras.h"
#include "CapturaDadosDeProcessos.h"
#include "LeituraDoTeclado.h"
#include "ListaCircular.h"
#include "Mensagens.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

#define ID_LEITURA_SISTEMA_INSPECAO_DEFEITOS 1
#define ID_LEITURA_CAPTURA_DEFEITOS_TIRAS 2
#define ID_CAPTURA_DADOS_PROCESSOS 3

#define WIN32_LEAN_AND_MEAN 

HANDLE Handle_Thread_Sistema_Inspecao_Defeitos;
HANDLE Handle_Thread_Captura_Defeitos_Tiras;
HANDLE Handle_Thread_Captura_Dados_Processos;
HANDLE Handle_Thread_Leitura_Teclado;

HANDLE Evento_Nao_Finalizar_Inspecao_Defeitos;
HANDLE Evento_Nao_Finalizar_Defeitos_Das_Tiras;
HANDLE Evento_Nao_Finalizar_Dados_De_Processo;
HANDLE Evento_Nao_Finalizar_Exibicao_De_Defeitos;
HANDLE Evento_Nao_Finalizar_Exibicao_De_Dados;

HANDLE Evento_Desbloquear_Inspecao_Defeitos;
HANDLE Evento_Desbloquear_Defeitos_Das_Tiras;
HANDLE Evento_Desbloquear_Dados_De_Processo;
HANDLE Evento_Desbloquear_Exibicao_De_Defeitos;
HANDLE Evento_Desbloquear_Exibicao_De_Dados;

HANDLE Evento_Limpar_Janela;

HANDLE Semaforo_Acesso_Lista_Circular_Livres;
HANDLE Semaforo_Acesso_Lista_Circular_Ocupados;
HANDLE Evento_Lista_Circular_Nao_Cheia;
HANDLE Evento_Lista_Circular_Contem_Dado_Processo;
HANDLE Evento_Lista_Circular_Contem_Defeito;

HANDLE Mutex_Acesso_Lista_Circular;
HANDLE Mutex_Acesso_Console;

HANDLE Evento_Timer_Dados_Processo_Executado;
HANDLE Evento_Timer_Defeitos_Tiras_Executado;

HANDLE Semaforo_Arquivo_Dados_Processo_Livre;
HANDLE Evento_Arquivo_Nao_Cheio;
HANDLE Mutex_Acesso_Arquivo;

HANDLE Handle_Console;

int main()
{
	DWORD status, dwThreadID, dwExitCode;

	STARTUPINFO si;
	PROCESS_INFORMATION NewProcess;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	//TODO: Tratamento de erros.

	Evento_Nao_Finalizar_Inspecao_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Nao_Finalizar_Inspecao_Defeitos");
	Evento_Nao_Finalizar_Defeitos_Das_Tiras = CreateEvent(NULL, TRUE, TRUE, "Evento_Nao_Finalizar_Defeitos_Das_Tiras");
	Evento_Nao_Finalizar_Dados_De_Processo = CreateEvent(NULL, TRUE, TRUE, "Evento_Nao_Finalizar_Dados_De_Processo");
	Evento_Nao_Finalizar_Exibicao_De_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Nao_Finalizar_Exibicao_De_Defeitos");
	Evento_Nao_Finalizar_Exibicao_De_Dados = CreateEvent(NULL, TRUE, TRUE, "Evento_Nao_Finalizar_Exibicao_De_Dados");

	Evento_Desbloquear_Inspecao_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Inspecao_Defeitos");
	Evento_Desbloquear_Defeitos_Das_Tiras = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Defeitos_Das_Tiras");
	Evento_Desbloquear_Dados_De_Processo = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Dados_De_Processo");
	Evento_Desbloquear_Exibicao_De_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Exibicao_De_Defeitos");
	Evento_Desbloquear_Exibicao_De_Dados = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Exibicao_De_Dados");

	Semaforo_Acesso_Lista_Circular_Livres = CreateSemaphore(NULL, TAMANHO_LISTA, TAMANHO_LISTA, "Semaforo_Acesso_Lista_Circular_Livres");
	Semaforo_Acesso_Lista_Circular_Ocupados = CreateSemaphore(NULL, 0, TAMANHO_LISTA, "Semaforo_Acesso_Lista_Circular_Ocupados");
	Evento_Lista_Circular_Nao_Cheia = CreateEvent(NULL, FALSE, FALSE, "Evento_Lista_Circular_Nao_Cheia");
	Evento_Lista_Circular_Contem_Dado_Processo = CreateEvent(NULL, FALSE, FALSE, "Evento_Lista_Circular_Contem_Dado_Processo");
	Evento_Lista_Circular_Contem_Defeito = CreateEvent(NULL, FALSE, FALSE, "Evento_Lista_Circular_Contem_Defeito");

	Mutex_Acesso_Lista_Circular = CreateMutex(NULL, FALSE, "Mutex_Acesso_Lista_Circular");
	Mutex_Acesso_Console = CreateMutex(NULL, FALSE, "Mutex_Acesso_Console");

	Evento_Timer_Dados_Processo_Executado = CreateEvent(NULL, FALSE, FALSE, "Evento_Timer_Dados_Processo_Executado");
	Evento_Timer_Defeitos_Tiras_Executado = CreateEvent(NULL, FALSE, FALSE, "Evento_Timer_Defeitos_Tiras_Executado");
	Mutex_Acesso_Arquivo = CreateMutex(NULL, FALSE, "Mutex_Acesso_Arquivo");

	Semaforo_Arquivo_Dados_Processo_Livre = CreateSemaphore(NULL, 100, 100, "Semaforo_Arquivo_Dados_Processo_Livre");
	Evento_Arquivo_Nao_Cheio = CreateEvent(NULL, FALSE, FALSE, "Evento_Arquivo_Nao_Cheio");

	Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

	Handle_Thread_Leitura_Teclado = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Leitura_Teclado,
		(void*)
		ID_CAPTURA_DADOS_PROCESSOS,
		0,
		(CAST_LPDWORD)&dwThreadID
	);

	Handle_Thread_Sistema_Inspecao_Defeitos = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Sistema_Inspecao_Defeitos,
		(void*)
		ID_LEITURA_SISTEMA_INSPECAO_DEFEITOS,
		0,
		(CAST_LPDWORD)&dwThreadID
	);

	Handle_Thread_Captura_Defeitos_Tiras = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Captura_Defeitos_Tiras,
		(void*)
		ID_LEITURA_CAPTURA_DEFEITOS_TIRAS,
		0,
		(CAST_LPDWORD)&dwThreadID
	);

	Handle_Thread_Captura_Dados_Processos = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Captura_Dados_Processos,
		(void*)
		ID_CAPTURA_DADOS_PROCESSOS,
		0,
		(CAST_LPDWORD)&dwThreadID
	);

	status = CreateProcess(
		"..\\Debug\\ExibicaoDeDefeitos.exe", // Caminho do arquivo execut�vel
		NULL,								 // Apontador p/ par�metros de linha de comando
		NULL,								 // Apontador p/ descritor de seguran�a
		NULL,								 // Idem, threads do processo
		FALSE,								 // Heran�a de handles
		CREATE_NEW_CONSOLE,					 // Flags de cria��o
		NULL,								 // Heran�a do amniente de execu��o
		NULL,						 // Diret�rio do arquivo execut�vel
		&si,								 // lpStartUpInfo
		&NewProcess);						 // lpProcessInformation

	status = CreateProcess(
		"..\\Debug\\ExibicaoDadosDeProcesso.exe", // Caminho do arquivo execut�vel
		NULL,									  // Apontador p/ par�metros de linha de comando
		NULL,									  // Apontador p/ descritor de seguran�a
		NULL,									  // Idem, threads do processo
		FALSE,									  // Heran�a de handles
		CREATE_NEW_CONSOLE,						  // Flags de cria��o
		NULL,									  // Heran�a do amniente de execu��o
		NULL,							  // Diret�rio do arquivo execut�vel
		&si,									  // lpStartUpInfo
		&NewProcess);							  // lpProcessInformation

	HANDLE Threads[4] = {
		Handle_Thread_Sistema_Inspecao_Defeitos,
		Handle_Thread_Captura_Defeitos_Tiras,
		Handle_Thread_Captura_Dados_Processos,
		Handle_Thread_Leitura_Teclado
	};

	status = WaitForMultipleObjects(4, Threads, true, INFINITE);

	if (status != WAIT_OBJECT_0) {
		printf("Erro em WaitForMultipleObjects! Codigo = %d\n", GetLastError());
		return 0;
	}

	MostrarMensagem("Finalizando...", AMARELO);

	CloseHandle(Handle_Thread_Sistema_Inspecao_Defeitos);
	CloseHandle(Handle_Thread_Captura_Defeitos_Tiras);
	CloseHandle(Handle_Thread_Captura_Dados_Processos);
	CloseHandle(Handle_Thread_Leitura_Teclado);

	CloseHandle(Evento_Nao_Finalizar_Inspecao_Defeitos);
	CloseHandle(Evento_Nao_Finalizar_Defeitos_Das_Tiras);
	CloseHandle(Evento_Nao_Finalizar_Dados_De_Processo);
	CloseHandle(Evento_Nao_Finalizar_Exibicao_De_Defeitos);
	CloseHandle(Evento_Nao_Finalizar_Exibicao_De_Dados);

	CloseHandle(Evento_Desbloquear_Inspecao_Defeitos);
	CloseHandle(Evento_Desbloquear_Defeitos_Das_Tiras);
	CloseHandle(Evento_Desbloquear_Dados_De_Processo);
	CloseHandle(Evento_Desbloquear_Exibicao_De_Defeitos);
	CloseHandle(Evento_Desbloquear_Exibicao_De_Dados);

	CloseHandle(Evento_Limpar_Janela);

	CloseHandle(Semaforo_Acesso_Lista_Circular_Livres);
	CloseHandle(Semaforo_Acesso_Lista_Circular_Ocupados);
	CloseHandle(Evento_Lista_Circular_Nao_Cheia);
	CloseHandle(Evento_Lista_Circular_Contem_Dado_Processo);
	CloseHandle(Evento_Lista_Circular_Contem_Defeito);

	CloseHandle(Mutex_Acesso_Lista_Circular);

	SetConsoleTextAttribute(Handle_Console, BRANCO);
	return 0;
}