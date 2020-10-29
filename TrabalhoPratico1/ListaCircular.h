#pragma once
#define TAMANHO_LISTA 200
extern int Posicao_Ponteiro;
extern std::string Lista_Circular_Memoria[];

int GetPosicaoPonteiro();
void IncrementarPosicaoPonteiro();
void Print_Snapshot_Lista(HANDLE Mutex_Acesso_Console, HANDLE Handle_Console);
void Adicionar_Mensagem_Na_Lista(std::string mensagem);