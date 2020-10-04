#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include "LeituraDoTeclado.h"

#define	ESC 0x1B
#define DESBLOQUEADA true
#define BLOQUEADA false;

int tecla = 0;

bool Estado_Inspecao_Defeitos = DESBLOQUEADA;
bool Estado_Defeitos_Das_Tiras = DESBLOQUEADA;
bool Estado_Dados_De_Processo = DESBLOQUEADA;
bool Estado_Exibicao_De_Defeitos = DESBLOQUEADA;
bool Estado_Exibicao_De_Dados = DESBLOQUEADA;

DWORD WINAPI Thread_Leitura_Teclado(LPVOID thread_arg) {

	int id = (int)thread_arg;

	HANDLE Evento_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Finalizar_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Defeitos_Das_Tiras");
	HANDLE Evento_Finalizar_Dados_De_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Dados_De_Processo");
	HANDLE Evento_Finalizar_Exibicao_De_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Exibicao_De_Defeitos");
	HANDLE Evento_Finalizar_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Finalizar_Exibicao_De_Dados");

	HANDLE Evento_Desbloquear_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Desbloquear_Inspecao_Defeitos");
	HANDLE Evento_Desbloquear_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Desbloquear_Defeitos_Das_Tiras");
	HANDLE Evento_Desbloquear_Dados_De_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Desbloquear_Dados_De_Processo");
	HANDLE Evento_Desbloquear_Exibicao_De_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Desbloquear_Exibicao_De_Defeitos");
	HANDLE Evento_Desbloquear_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Desbloquear_Exibicao_De_Dados");

	//TODO: Tratamento de erros.

	do{
		tecla = _getch();

		switch (tecla) {
		case 'i':
			printf("Alternando tarefa de inspeção de defeitos...\n");
			AlternarEvento(Evento_Desbloquear_Inspecao_Defeitos, Estado_Inspecao_Defeitos);
			break;
		default:
			printf("Comando não recohecido \n");
		}

	} while (tecla != ESC);

	//Evento de finalizar threads
	ResetEvent(Evento_Finalizar_Inspecao_Defeitos);
	ResetEvent(Evento_Finalizar_Defeitos_Das_Tiras);
	ResetEvent(Evento_Finalizar_Dados_De_Processo);
	ResetEvent(Evento_Finalizar_Exibicao_De_Defeitos);
	ResetEvent(Evento_Finalizar_Exibicao_De_Dados);

	//Libera as threads bloqueadas
	SetEvent(Evento_Desbloquear_Inspecao_Defeitos);
	SetEvent(Evento_Desbloquear_Defeitos_Das_Tiras);
	SetEvent(Evento_Desbloquear_Dados_De_Processo);
	SetEvent(Evento_Desbloquear_Exibicao_De_Defeitos);
	SetEvent(Evento_Desbloquear_Exibicao_De_Dados);
	
	printf("Finalizando thread the leitura de teclado\n");

	_endthreadex((DWORD)id);
	return id;
}

void AlternarEvento(HANDLE evento, bool Estado_Atual) {
	if (Estado_Atual == DESBLOQUEADA) {
		ResetEvent(evento);
		Estado_Inspecao_Defeitos = BLOQUEADA;
	}
	else {
		SetEvent(evento);
		Estado_Inspecao_Defeitos = DESBLOQUEADA;
	}
}