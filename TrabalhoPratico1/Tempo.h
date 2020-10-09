#pragma once
#include <windows.h>
#include <string>
typedef struct Tempo {
	int hora;
	int minuto;
	int segundo;
	int milissegundo;
} Tempo;

std::string SerializarTempo(Tempo tempo);

Tempo DesserializarTempo(std::string tempoString);

Tempo GerarTempoAtual();