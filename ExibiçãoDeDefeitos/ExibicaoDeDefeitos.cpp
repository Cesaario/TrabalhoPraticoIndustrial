#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>
#include <iostream>
#include "DefeitoSuperficieTira.h"

#define TAMANHO_MENSAGEM 37

int main()
{
    printf("Processo de exibicao de defeitos iniciando...\n");

	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(Handle_Console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	DWORD resultadoEvento;

	HANDLE Evento_Nao_Finalizar_Exibicao_De_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Exibicao_De_Defeitos");
	HANDLE Evento_Desbloquear_Exibicao_De_Defeitos = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Exibicao_De_Defeitos");

	WaitNamedPipe("Pipe_Defeitos_Das_Tiras", NMPWAIT_USE_DEFAULT_WAIT);

	HANDLE Pipe_Defeitos_Das_Tiras = CreateFile(
		"\\\\.\\pipe\\Pipe_Defeitos_Das_Tiras",
		GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL
	);

	do {
		WaitForSingleObject(Evento_Desbloquear_Exibicao_De_Defeitos, INFINITE);

		char Mensagem_Lida[TAMANHO_MENSAGEM];
		DWORD Bytes_Lidos;

		ReadFile(
			Pipe_Defeitos_Das_Tiras,
			Mensagem_Lida,
			37,
			&Bytes_Lidos,
			NULL
		);

		std::string Mensagem_Formatada = FormatarDefeitoTira(DesserializarDefeitoTira(Mensagem_Lida));

		std::cout << Mensagem_Formatada << std::endl;

		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Exibicao_De_Defeitos, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	CloseHandle(Pipe_Defeitos_Das_Tiras);
	printf("Finalizando processo de exibicao de defeitos...\n");
}
