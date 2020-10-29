#include "Mensagens.h"
#include <string>
#include <iostream>

void MostrarMensagem(std::string mensagem, int cor) {
	HANDLE Mutex_Acesso_Console = OpenMutex(SYNCHRONIZE | MUTEX_MODIFY_STATE, false, "Mutex_Acesso_Console");
	HANDLE Handle_Console = GetStdHandle(STD_OUTPUT_HANDLE);

	WaitForSingleObject(Mutex_Acesso_Console, INFINITE);
	SetConsoleTextAttribute(Handle_Console, cor);
	std::cout << mensagem << std::endl;
	ReleaseMutex(Mutex_Acesso_Console);
}