#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>

int main()
{
	printf("Processo de exibicao de dados de processo iniciando...\n");

	DWORD resultadoEvento;

	HANDLE Evento_Finalizar_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Finalizar_Exibicao_De_Dados");
	HANDLE Evento_Desbloquear_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Exibicao_De_Dados");

	int contador = 0;

	do {
		WaitForSingleObject(Evento_Desbloquear_Exibicao_De_Dados, INFINITE);
		printf("%d\n", contador++);
		resultadoEvento = WaitForSingleObject(Evento_Finalizar_Exibicao_De_Dados, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando processo de exibicao de dados de processo...\n");
}
