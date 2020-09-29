#include <string>
#include <sstream>
#include "Tempo.h"

std::string SerializarTempo(Tempo tempo) {
	std::stringstream resultado;
	resultado << tempo.hora << ":";
	resultado << tempo.minuto << ":";
	resultado << tempo.segundo << ":";
	resultado << tempo.milissegundo;
	return resultado.str();
}

Tempo DesserializarTempo(std::string tempoString) {
	std::istringstream iss(tempoString);
	std::string token;

	Tempo tempo;

	int index = 0;
	while (std::getline(iss, token, ':'))
    {
		std::stringstream campoInteiro(token);

		switch (index) {
		case 0:
			campoInteiro >> tempo.hora;
			break;
		case 1:
			campoInteiro >> tempo.minuto;
			break;
		case 2:
			campoInteiro >> tempo.segundo;
			break;
		case 3:
			campoInteiro >> tempo.milissegundo;
			break;
		}
		index++;
    }
	return tempo;
}