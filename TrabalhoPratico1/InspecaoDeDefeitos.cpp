#include <windows.h>
#include <stdio.h>
#include <process.h>

HANDLE Timer_Inspecao_De_Defeitos;

int SetupInspecaoDeDefeitos() {

	LARGE_INTEGER tempo;
	tempo.QuadPart = -10000000LL;

	Timer_Inspecao_De_Defeitos = CreateWaitableTimer(NULL, TRUE, NULL);

	if (Timer_Inspecao_De_Defeitos == NULL)
	{
		printf("CreateWaitableTimer failed (%d)\n", GetLastError());
		return 1;
	}

	if (!SetWaitableTimer(Timer_Inspecao_De_Defeitos, &tempo, 0, NULL, NULL, 0))
	{
		printf("SetWaitableTimer failed (%d)\n", GetLastError());
		return 2;
	}
}

DWORD WINAPI Thread_Leitura_Sistema_Inspecao_Defeitos(LPVOID thread_arg) {

	int id = (int)thread_arg;

	int i = 0;
	while (i < 100000) {

		WaitForSingleObject(Timer_Inspecao_De_Defeitos, INFINITE);
		printf("%d\n", i);
		i++;


		LARGE_INTEGER tempo;
		tempo.QuadPart = -10000000LL;
		if (!SetWaitableTimer(Timer_Inspecao_De_Defeitos, &tempo, 0, NULL, NULL, 0))
		{
			printf("SetWaitableTimer failed (%d)\n", GetLastError());
			return 2;
		}
	}

	_endthreadex((DWORD)id);
	return id;
}