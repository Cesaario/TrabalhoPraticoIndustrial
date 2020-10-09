#pragma warning(disable:4996)
#include "DefeitoSuperficieTira.h"
#include <time.h>
#include "RandomUtil.h"
#include <string>

int Numero_Defeito_Tira = 1;

DefeitoTira GerarDefeitoTira() {
	DefeitoTira defeito;

	srand(time(NULL));
	defeito.numero = Numero_Defeito_Tira;
	defeito.tipo = 11;
	defeito.cadeira = rand() % 6 + 1;
	defeito.gravidade = rand() % 100;
	defeito.classe = rand() % 9 + 1;
	defeito.id_foto = GerarIdAleatorio(6);
	defeito.tempo = GerarTempoAtual();

	Numero_Defeito_Tira++;
	return defeito;
}

std::string SerializarDefeitoTira(DefeitoTira defeito) {
	char NSEQ_Formatado[6];
	char CADEIRA_Formatado[3];
	char GRAVIDADE_Formatado[3];
	char ID_FOTO_Formatado[7];
	char TEMPO_Formatado[13];

	sprintf(NSEQ_Formatado, "%05d", defeito.numero);
	sprintf(CADEIRA_Formatado, "%02d", defeito.cadeira);
	sprintf(GRAVIDADE_Formatado, "%02d", defeito.gravidade);
	strcpy(ID_FOTO_Formatado, defeito.id_foto.c_str());
	strcpy(TEMPO_Formatado, SerializarTempo(defeito.tempo).c_str());

	char Mensagem_Serializada[37];
	sprintf(Mensagem_Serializada, "%s/%d/%s/%s/%d/%s/%s",
		NSEQ_Formatado,
		defeito.tipo,
		CADEIRA_Formatado,
		GRAVIDADE_Formatado,
		defeito.classe,
		ID_FOTO_Formatado,
		TEMPO_Formatado
	);

	return Mensagem_Serializada;
}

DefeitoTira DesserializarDefeitoTira(std::string defeitoString) {
	DefeitoTira defeito;
	if (defeitoString.size() != 36) {
		defeito.tipo = 0;
		return defeito;
	}

	std::string delim = "/";
	auto start = 0U;
	auto end = defeitoString.find(delim);
	int posicao = 0;
	while (end != std::string::npos)
	{
		switch (posicao) {
		case 0:
			defeito.numero = std::stoi(defeitoString.substr(start, end - start));
			break;
		case 1:
			defeito.tipo = std::stoi(defeitoString.substr(start, end - start));
			break;
		case 2:
			defeito.cadeira = std::stoi(defeitoString.substr(start, end - start));
			break;
		case 3:
			defeito.gravidade = std::stoi(defeitoString.substr(start, end - start));
			break;
		case 4:
			defeito.classe = std::stoi(defeitoString.substr(start, end - start));
			break;
		case 5:
			defeito.id_foto = defeitoString.substr(start, end - start);
			break;
		}
		start = end + delim.length();
		end = defeitoString.find(delim, start);
		posicao++;
	}
	defeito.tempo = DesserializarTempo(defeitoString.substr(start, end - start));
	return defeito;
}