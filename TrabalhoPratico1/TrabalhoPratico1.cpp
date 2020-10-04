#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>
#include "InspecaoDeDefeitos.h"
#include "CapturaDefeitosDasTiras.h"
#include "CapturaDadosDeProcessos.h"
#include "LeituraDoTeclado.h"
#include "ListaCircular.h"

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

#define ID_LEITURA_SISTEMA_INSPECAO_DEFEITOS 1
#define ID_LEITURA_CAPTURA_DEFEITOS_TIRAS 2
#define ID_CAPTURA_DADOS_PROCESSOS 3

HANDLE Handle_Thread_Leitura_Sistema_Inspecao_Defeitos;
HANDLE Handle_Thread_Captura_Defeitos_Tiras;
HANDLE Handle_Thread_Captura_Dados_Processos;
HANDLE Handle_Thread_Leitura_Teclado;

HANDLE Evento_Finalizar_Inspecao_Defeitos;
HANDLE Evento_Finalizar_Defeitos_Das_Tiras;
HANDLE Evento_Finalizar_Dados_De_Processo;
HANDLE Evento_Finalizar_Exibicao_De_Defeitos;
HANDLE Evento_Finalizar_Exibicao_De_Dados;

HANDLE Evento_Desbloquear_Inspecao_Defeitos;
HANDLE Evento_Desbloquear_Defeitos_Das_Tiras;
HANDLE Evento_Desbloquear_Dados_De_Processo;
HANDLE Evento_Desbloquear_Exibicao_De_Defeitos;
HANDLE Evento_Desbloquear_Exibicao_De_Dados;

HANDLE Semaforo_Acesso_Lista_Circular_Livres;
HANDLE Semaforo_Acesso_Lista_Circular_Bloqueados;

int main()
{

	DWORD status, dwThreadID, dwExitCode;

	STARTUPINFO si;
	PROCESS_INFORMATION NewProcess;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	//TODO: Tratamento de erros.

	Evento_Finalizar_Inspecao_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Finalizar_Inspecao_Defeitos");
	Evento_Finalizar_Defeitos_Das_Tiras = CreateEvent(NULL, TRUE, TRUE, "Evento_Finalizar_Defeitos_Das_Tiras");
	Evento_Finalizar_Dados_De_Processo = CreateEvent(NULL, TRUE, TRUE, "Evento_Finalizar_Dados_De_Processo");
	Evento_Finalizar_Exibicao_De_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Finalizar_Exibicao_De_Defeitos");
	Evento_Finalizar_Exibicao_De_Dados = CreateEvent(NULL, TRUE, TRUE, "Evento_Finalizar_Exibicao_De_Dados");

	Evento_Desbloquear_Inspecao_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Inspecao_Defeitos");
	Evento_Desbloquear_Defeitos_Das_Tiras = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Defeitos_Das_Tiras");
	Evento_Desbloquear_Dados_De_Processo = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Dados_De_Processo");
	Evento_Desbloquear_Exibicao_De_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Exibicao_De_Defeitos");
	Evento_Desbloquear_Exibicao_De_Dados = CreateEvent(NULL, TRUE, TRUE, "Evento_Desbloquear_Exibicao_De_Dados");

	Semaforo_Acesso_Lista_Circular_Livres = CreateSemaphore(NULL, 200, )

	Handle_Thread_Leitura_Teclado = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Leitura_Teclado,
		(void*)
		ID_CAPTURA_DADOS_PROCESSOS,
		0,
		(CAST_LPDWORD)&dwThreadID
	);

	Handle_Thread_Leitura_Sistema_Inspecao_Defeitos = (HANDLE)_beginthreadex(
		NULL,
		0,
		(CAST_FUNCTION)Thread_Leitura_Sistema_Inspecao_Defeitos,
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
		"..\\Debug\\ExibicaoDeDefeitos.exe", // Caminho do arquivo executável
		NULL,								 // Apontador p/ parâmetros de linha de comando
		NULL,								 // Apontador p/ descritor de segurança
		NULL,								 // Idem, threads do processo
		FALSE,								 // Herança de handles
		CREATE_NEW_CONSOLE,					 // Flags de criação
		NULL,								 // Herança do amniente de execução
		"C:\\Windows",						 // Diretório do arquivo executável
		&si,								 // lpStartUpInfo
		&NewProcess);						 // lpProcessInformation

	status = CreateProcess(
		"..\\Debug\\ExibicaoDadosDeProcesso.exe", // Caminho do arquivo executável
		NULL,									  // Apontador p/ parâmetros de linha de comando
		NULL,									  // Apontador p/ descritor de segurança
		NULL,									  // Idem, threads do processo
		FALSE,									  // Herança de handles
		CREATE_NEW_CONSOLE,						  // Flags de criação
		NULL,									  // Herança do amniente de execução
		"C:\\Windows",							  // Diretório do arquivo executável
		&si,									  // lpStartUpInfo
		&NewProcess);							  // lpProcessInformation

	HANDLE Threads[4] = {
		Handle_Thread_Leitura_Sistema_Inspecao_Defeitos,
		Handle_Thread_Captura_Defeitos_Tiras,
		Handle_Thread_Captura_Dados_Processos,
		Handle_Thread_Leitura_Teclado
	};

	status = WaitForMultipleObjects(4, Threads, true, INFINITE);

	if (status != WAIT_OBJECT_0) {
		printf("Erro em WaitForMultipleObjects! Codigo = %d\n", GetLastError());
		return 0;
	}

	printf("Finalizando...");

	CloseHandle(Handle_Thread_Leitura_Sistema_Inspecao_Defeitos);
	CloseHandle(Handle_Thread_Captura_Defeitos_Tiras);
	CloseHandle(Handle_Thread_Captura_Dados_Processos);
	CloseHandle(Handle_Thread_Leitura_Teclado);

	return 0;
}