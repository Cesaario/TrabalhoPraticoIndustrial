#include <string>
#include <sstream>
#include "Tempo.h"

std::string SerializarTempo(Tempo tempo) {
	char Tempo_Serializado[12];
	sprintf(Tempo_Serializado, "%02d:%02d:%02d:%03d", tempo.hora, tempo.minuto, tempo.segundo, tempo.milissegundo);
	return Tempo_Serializado;
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