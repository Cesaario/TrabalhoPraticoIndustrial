#include <string>
#include <windows.h>
#include "ListaCircular.h"

std::string Lista_Circular_Memoria[TAMANHO_LISTA];
int Posicao_Livre = 0;
int Posicao_Ocupado = 0;

/*HANDLE Semaforo_Acesso_Lista_Circular_Livres;
HANDLE Semaforo_Acesso_Lista_Circular_Ocupados;

void IniciarHandlesListaCircular() {
	Semaforo_Acesso_Lista_Circular_Livres = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Livres");
	Semaforo_Acesso_Lista_Circular_Ocupados = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, false, "Semaforo_Acesso_Lista_Circular_Ocupados");
}*/

std::string* ReferenciaListaCircular() {
	return Lista_Circular_Memoria;
}

int GetPosicaoLivre() {
	return Posicao_Livre % TAMANHO_LISTA;
}

void IncrementarPosicaoLivre() {
	Posicao_Livre = (Posicao_Livre + 1) % TAMANHO_LISTA;
}