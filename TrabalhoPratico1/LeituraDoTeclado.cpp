#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include "LeituraDoTeclado.h"

#define	ESC 0x1B
#define DESBLOQUEADA true
#define BLOQUEADA false;

int tecla = 0;

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

	HANDLE Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	HANDLE Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
	HANDLE Semaforo_Acesso_Lista_Circular_Cheia = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Cheia");
	HANDLE Evento_Lista_Circular_Contem_Dado_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Dado_Processo");
	HANDLE Evento_Lista_Circular_Contem_Defeito = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Lista_Circular_Contem_Defeito");

	HANDLE Evento_Limpar_Janela = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Limpar_Janela");

	HANDLE Mutex_Acesso_Lista_Circular = OpenMutex(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Mutex_Acesso_Lista_Circular");

	bool Estado_Inspecao_Defeitos = DESBLOQUEADA;
	bool Estado_Defeitos_Das_Tiras = DESBLOQUEADA;
	bool Estado_Dados_De_Processo = DESBLOQUEADA;
	bool Estado_Exibicao_De_Defeitos = DESBLOQUEADA;
	bool Estado_Exibicao_De_Dados = DESBLOQUEADA;

	//TODO: Tratamento de erros.

	do{
		tecla = _getch();

		switch (tecla) {
		case 'i':
			printf("Alternando tarefa de inspeção de defeitos...\n");
			AlternarEvento(Evento_Desbloquear_Inspecao_Defeitos, &Estado_Inspecao_Defeitos);
			break;
		case 'd':
			printf("Alternando tarefa de defeitos das tiras...\n");
			AlternarEvento(Evento_Desbloquear_Defeitos_Das_Tiras, &Estado_Defeitos_Das_Tiras);
			break;
		case 'e':
			printf("Alternando tarefa de dados de processo...\n");
			AlternarEvento(Evento_Desbloquear_Dados_De_Processo, &Estado_Dados_De_Processo);
			break;
		case 'a':
			printf("Alternando tarefa de exibição de defeitos...\n");
			AlternarEvento(Evento_Desbloquear_Exibicao_De_Defeitos, &Estado_Exibicao_De_Defeitos);
			break;
		case 'l':
			printf("Alternando tarefa de exibição de dados...\n");
			AlternarEvento(Evento_Desbloquear_Exibicao_De_Dados, &Estado_Exibicao_De_Dados);
			break;
		case 'c':
			printf("Limpando janela...\n");
			SetEvent(Evento_Limpar_Janela);
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
	ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Livres, 1, NULL);
	ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Ocupados, 2, NULL);
	ReleaseSemaphore(Semaforo_Acesso_Lista_Circular_Cheia, 1, NULL);

	SetEvent(Evento_Lista_Circular_Contem_Defeito);
	SetEvent(Evento_Lista_Circular_Contem_Dado_Processo);
	SetEvent(Evento_Desbloquear_Inspecao_Defeitos);
	SetEvent(Evento_Desbloquear_Defeitos_Das_Tiras);
	SetEvent(Evento_Desbloquear_Dados_De_Processo);
	SetEvent(Evento_Desbloquear_Exibicao_De_Defeitos);
	SetEvent(Evento_Desbloquear_Exibicao_De_Dados);

	ReleaseMutex(Mutex_Acesso_Lista_Circular);
	
	printf("Finalizando thread the leitura de teclado\n");

	_endthreadex((DWORD)id);
	return id;
}

void AlternarEvento(HANDLE evento, bool* Estado_Atual) {
	if (*Estado_Atual == DESBLOQUEADA) {
		ResetEvent(evento);
		*Estado_Atual = BLOQUEADA;
	}
	else {
		SetEvent(evento);
		*Estado_Atual = DESBLOQUEADA;
	}
}