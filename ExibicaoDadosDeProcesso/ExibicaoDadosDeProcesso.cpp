#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <errno.h>
#include <string>

int main()
{
	printf("Processo de exibicao de dados de processo iniciando...\n");

	DWORD resultadoEvento = 0;
	DWORD resultadoWait = 0;

	HANDLE Evento_Nao_Finalizar_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Nao_Finalizar_Exibicao_De_Dados");
	HANDLE Evento_Desbloquear_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE, false, "Evento_Desbloquear_Exibicao_De_Dados");
	HANDLE Evento_Limpar_Janela = OpenEvent(SYNCHRONIZE, false, "Evento_Limpar_Janela");

	HANDLE Handles_Tarefa_Exibicao_De_Dados[] = { Evento_Limpar_Janela, Evento_Desbloquear_Exibicao_De_Dados };

	int contador = 0;

	do {
		resultadoWait = WaitForMultipleObjects(2, Handles_Tarefa_Exibicao_De_Dados, FALSE, INFINITE);
		if (resultadoWait == WAIT_OBJECT_0 + 0) {
			system("cls");
			contador = 0;
			continue;
		}
		printf("%d\n", contador++);
		resultadoEvento = WaitForSingleObject(Evento_Nao_Finalizar_Exibicao_De_Dados, 0);
	} while (resultadoEvento == WAIT_OBJECT_0);

	printf("Finalizando processo de exibicao de dados de processo...\n");
}
