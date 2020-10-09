#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>
#include <string>
#include "LeituraDoTeclado.h"
#include "ListaCircular.h"

#define	ESC 0x1B
#define DESBLOQUEADA true
#define BLOQUEADA false;

#define DARKRED   FOREGROUND_RED
#define GRAY   FOREGROUND_INTENSITY
#define RED   FOREGROUND_INTENSITY | FOREGROUND_RED
#define YELLOW   FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define CYAN   FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE
#define MAGENTA   FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE

int tecla = 0;

DWORD WINAPI Thread_Leitura_Teclado(LPVOID thread_arg) {

	int id = (int)thread_arg;

	HANDLE Evento_Nao_Finalizar_Inspecao_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Nao_Finalizar_Inspecao_Defeitos");
	HANDLE Evento_Nao_Finalizar_Defeitos_Das_Tiras = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Nao_Finalizar_Defeitos_Das_Tiras");
	HANDLE Evento_Nao_Finalizar_Dados_De_Processo = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Nao_Finalizar_Dados_De_Processo");
	HANDLE Evento_Nao_Finalizar_Exibicao_De_Defeitos = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Nao_Finalizar_Exibicao_De_Defeitos");
	HANDLE Evento_Nao_Finalizar_Exibicao_De_Dados = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, false, "Evento_Nao_Finalizar_Exibicao_De_Dados");

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

	HANDLE Mutex_Acesso_Console = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Console");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

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
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, CYAN);
			printf("Alternando tarefa de inspe��o de defeitos...\n");
			ReleaseMutex(Mutex_Acesso_Console);
			AlternarEvento(Evento_Desbloquear_Inspecao_Defeitos, &Estado_Inspecao_Defeitos);
			break;
		case 'd':
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, CYAN);
			printf("Alternando tarefa de defeitos das tiras...\n");
			ReleaseMutex(Mutex_Acesso_Console);
			AlternarEvento(Evento_Desbloquear_Defeitos_Das_Tiras, &Estado_Defeitos_Das_Tiras);
			break;
		case 'e':
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, CYAN);
			printf("Alternando tarefa de dados de processo...\n");
			ReleaseMutex(Mutex_Acesso_Console);
			AlternarEvento(Evento_Desbloquear_Dados_De_Processo, &Estado_Dados_De_Processo);
			break;
		case 'a':
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, CYAN);
			printf("Alternando tarefa de exibi��o de defeitos...\n");
			ReleaseMutex(Mutex_Acesso_Console);
			AlternarEvento(Evento_Desbloquear_Exibicao_De_Defeitos, &Estado_Exibicao_De_Defeitos);
			break;
		case 'l':
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, CYAN);
			printf("Alternando tarefa de exibi��o de dados...\n");
			ReleaseMutex(Mutex_Acesso_Console);
			AlternarEvento(Evento_Desbloquear_Exibicao_De_Dados, &Estado_Exibicao_De_Dados);
			break;
		case 'c':
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, GRAY);
			printf("Limpando janela...\n");
			ReleaseMutex(Mutex_Acesso_Console);
			SetEvent(Evento_Limpar_Janela);
			break;
		case 'v':
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, MAGENTA);
			Print_Snapshot_Lista();
			ReleaseMutex(Mutex_Acesso_Console);
			break;
		case 27: //ESC
			break;
		default:
			WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
			SetConsoleTextAttribute(Handle_Console, RED);
			printf("-----------------------\n");
			SetConsoleTextAttribute(Handle_Console, DARKRED);
			printf("Comando nao recohecido!\n");
			SetConsoleTextAttribute(Handle_Console, RED);
			printf("i: Alterna a tarefa de inspecao de defeitos\n");
			printf("d: Alterna a tarefa de captura de mensagens de defeitos de tiras\n");
			printf("e: Alterna a tarefa de captura de mensagens de dados de processo\n");
			printf("a: Alterna a tarefa de exibicao de defeitos de tiras\n");
			printf("l: Alterna a tarefa de exibicao de dados de processo\n");
			printf("c: Limpa a janela de console da tarefa de exibicao de dados de processo\n");
			printf("v: Mostra uma snapshot da lista circular em memoria\n");
			printf("ESC: Encerra o programa\n");
			printf("-----------------------\n");
			ReleaseMutex(Mutex_Acesso_Console);
		}

	} while (tecla != ESC);

	//Evento de finalizar threads
	ResetEvent(Evento_Nao_Finalizar_Inspecao_Defeitos);
	ResetEvent(Evento_Nao_Finalizar_Defeitos_Das_Tiras);
	ResetEvent(Evento_Nao_Finalizar_Dados_De_Processo);
	ResetEvent(Evento_Nao_Finalizar_Exibicao_De_Defeitos);
	ResetEvent(Evento_Nao_Finalizar_Exibicao_De_Dados);

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

	WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
	SetConsoleTextAttribute(Handle_Console, YELLOW);
	printf("Finalizando thread the leitura de teclado\n");
	ReleaseMutex(Mutex_Acesso_Console);

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