#include <string>
#include <windows.h>
#include "ListaCircular.h"

std::string Lista_Circular_Memoria[TAMANHO_LISTA];
int Posicao_Ponteiro = 0;

std::string* ReferenciaListaCircular() {
	return Lista_Circular_Memoria;
}

int GetPosicaoPonteiro() {
	return Posicao_Ponteiro % TAMANHO_LISTA;
}

void IncrementarPosicaoPonteiro() {
	Posicao_Ponteiro = (Posicao_Ponteiro + 1) % TAMANHO_LISTA;
}

void Print_Snapshot_Lista() {
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
}