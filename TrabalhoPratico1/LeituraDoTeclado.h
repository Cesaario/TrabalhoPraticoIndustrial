#pragma once
#include <windows.h>
#include <stdio.h>
#include <process.h>

DWORD WINAPI Thread_Leitura_Teclado(LPVOID thread_arg);

void AlternarEvento(HANDLE evento, bool *Estado_Atual);