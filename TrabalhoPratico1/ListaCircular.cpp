#include <string>
#include <windows.h>
#include "ListaCircular.h"

#define ROXO FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE

std::string Lista_Circular_Memoria[TAMANHO_LISTA];
int Posicao_Ponteiro = 0;

int GetPosicaoPonteiro() {
	return Posicao_Ponteiro % TAMANHO_LISTA;
}

void IncrementarPosicaoPonteiro() {
	Posicao_Ponteiro = (Posicao_Ponteiro + 1) % TAMANHO_LISTA;
}

void Print_Snapshot_Lista(HANDLE Mutex_Acesso_Console, HANDLE Handle_Console) {
	WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
	SetConsoleTextAttribute(Handle_Console, ROXO);

	for (int i = 0; i < 32; i++) printf("*");
	printf(" SNAPSHOT LISTA CIRCULAR EM MEMORIA ");
	for (int i = 0; i < 32; i++) printf("*");
	printf("\n");
	printf("\n");

	for (int i = 0; i < TAMANHO_LISTA; i++) {
		std::string mensagem = Lista_Circular_Memoria[i];
		if (mensagem.size() == 0) {
			printf("_");
		}
		else {
			printf("X");
		}
		if (i == (TAMANHO_LISTA / 2) - 1) {
			printf("\n");
		}
	}
	printf("\n");
	printf("\n");
	for (int i = 0; i < 100; i++) printf("*");
	printf("\n");

	ReleaseMutex(Mutex_Acesso_Console);
}