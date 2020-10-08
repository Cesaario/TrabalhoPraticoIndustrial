#pragma once
#include "Tempo.h"
#include<string>

typedef struct DadosProcesso {
	int numero;
	int tipo;
	int cadeira;
	std::string id;
	float temp;
	float vel;
	Tempo tempo;
} DadosProcesso;