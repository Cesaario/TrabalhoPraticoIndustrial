#pragma warning(disable:4996)
#include <string>
#include <sstream>
#include <time.h>
#include "Tempo.h"

Tempo GerarTempoAtual() {
	Tempo tempo;

	SYSTEMTIME time_windows;
	GetSystemTime(&time_windows);

	tempo.hora = time_windows.wHour;
	tempo.minuto = time_windows.wMinute;
	tempo.segundo = time_windows.wSecond;
	tempo.milissegundo = time_windows.wMilliseconds;

	return tempo;
}

std::string SerializarTempo(Tempo tempo) {
	char Tempo_Serializado[13];
	sprintf(Tempo_Serializado, "%02d:%02d:%02d:%03d", tempo.hora, tempo.minuto, tempo.segundo, tempo.milissegundo);
	return Tempo_Serializado;
}

Tempo DesserializarTempo(std::string tempoString) {
	Tempo tempo;

	std::string delim = ":";
	auto start = 0U;
	auto end = tempoString.find(delim);
	int posicao = 0;

	while (end != std::string::npos)
	{
		switch (posicao) {
		case 0:
			tempo.hora = std::stoi(tempoString.substr(start, end - start));
			break;
		case 1:
			tempo.minuto = std::stoi(tempoString.substr(start, end - start));
			break;
		case 2:
			tempo.segundo = std::stoi(tempoString.substr(start, end - start));
			break;
		}
		start = end + delim.length();
		end = tempoString.find(delim, start);
		posicao++;
	}
	tempo.milissegundo = std::stoi(tempoString.substr(start, end));
	return tempo;
}