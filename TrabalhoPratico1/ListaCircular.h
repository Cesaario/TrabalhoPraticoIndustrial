#pragma once
#define TAMANHO_LISTA 200
extern int Posicao_Livre;
extern int Posicao_Ocupado;
extern std::string Lista_Circular_Memoria[];

void IniciarHandlesListaCircular();
int GetPosicaoLivre();
void IncrementarPosicaoLivre();