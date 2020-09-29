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

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

#define ID_LEITURA_SISTEMA_INSPECAO_DEFEITOS 1
#define ID_LEITURA_CAPTURA_DEFEITOS_TIRAS 2
#define ID_CAPTURA_DADOS_PROCESSOS 3

int main()
{
	HANDLE Handle_Thread_Leitura_Sistema_Inspecao_Defeitos;
	HANDLE Handle_Thread_Captura_Defeitos_Tiras;
	HANDLE Handle_Thread_Captura_Dados_Processos;
	HANDLE Handle_Thread_Leitura_Teclado;

	HANDLE Evento_Finalizar_Inspecao_Defeitos;

	DWORD status, dwThreadID, dwExitCode;

	//string str(L"aa");
	//SetConsoleTitle(str.c_str());0

	//SetupInspecaoDeDefeitos();

	Evento_Finalizar_Inspecao_Defeitos = CreateEvent(NULL, TRUE, TRUE, "Evento_Finalizar_Inspecao_Defeitos");

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