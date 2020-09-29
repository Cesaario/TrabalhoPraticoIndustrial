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

	if (Evento_Finalizar_Inspecao_Defeitos == NULL) {
		//TODO
		printf("ERRO\n");
		_endthreadex((DWORD)id);
		return id;
	}

	do{
		tecla = _getch();

	} while (tecla != ESC);

	ResetEvent(Evento_Finalizar_Inspecao_Defeitos);
	printf("Resetando evento de inspeção de defeitos\n");
	
	
	printf("Finalizando thread the leitura de teclado\n");

	_endthreadex((DWORD)id);
	return id;
}