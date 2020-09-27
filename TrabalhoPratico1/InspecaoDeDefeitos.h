#pragma once
#include <windows.h>
#include <stdio.h>
#include <process.h>

DWORD WINAPI Thread_Leitura_Sistema_Inspecao_Defeitos(LPVOID thread_arg);

int SetupInspecaoDeDefeitos();