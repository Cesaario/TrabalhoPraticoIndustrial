#pragma once
#include "Tempo.h"
#include<string>

typedef struct DefeitoTira {
	int numero;
	int tipo;
	int cadeira;
	int gravidade;
	int classe;
	std::string id_foto;
	Tempo tempo;
} DefeitoTira;

DefeitoTira GerarDefeitoTira();

std::string SerializarDefeitoTira(DefeitoTira defeito);

DefeitoTira DesserializarDefeitoTira(std::string defeitoString);

std::string FormatarDefeitoTira(DefeitoTira defeito);