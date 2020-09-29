#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include "LeituraDoTeclado.h"

#define	ESC 0x1B

int tecla = 0;

DWORD WINAPI Thread_Leitura_Teclado(LPVOID thread_arg) {

	int id = (int)thread_arg;

	HANDLE Evento_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Finalizar_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Defeitos_Das_Tiras");
	HANDLE Evento_Finalizar_Dados_De_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Dados_De_Processo");

	//TODO: Tratamento de erros.

	do{
		tecla = _getch();

	} while (tecla != ESC);

	ResetEvent(Evento_Finalizar_Inspecao_Defeitos);
	ResetEvent(Evento_Finalizar_Defeitos_Das_Tiras);
	ResetEvent(Evento_Finalizar_Dados_De_Processo);
	
	printf("Finalizando thread the leitura de teclado\n");

	_endthreadex((DWORD)id);
	return id;
}