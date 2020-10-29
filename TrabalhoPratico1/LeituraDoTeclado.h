#pragma once
#include <windows.h>
#include <stdio.h>
#include <process.h>

DWORD WINAPI Thread_Leitura_Teclado(LPVOID thread_arg);

void AlternarEvento(HANDLE evento, bool *Estado_Atual);

void Mostrar_Comandos_Ajuda();

std::string Gerar_Mensagem_Bloqueio_Desbloqueio(bool status, char* tarefa);