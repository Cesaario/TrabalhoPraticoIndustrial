#include <time.h>
#include <string>
#include "DefeitoSuperficieTira.h"
#include "RandomUtil.h"

int Numero_Defeito_Tira = 1;

DefeitoTira GerarDefeitoTira() {
	DefeitoTira defeito;


	defeito.numero = Numero_Defeito_Tira;
	defeito.tipo = 11;
	defeito.cadeira = rand() % 6 + 1;
	defeito.gravidade = rand() % 100;
	defeito.classe = rand() % 9 + 1;
	defeito.id_foto = GerarIdAleatorio(MENSAGEM_DEFEITO);
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

	sprintf_s(NSEQ_Formatado, 6, "%05d", defeito.numero);
	sprintf_s(CADEIRA_Formatado, 3, "%02d", defeito.cadeira);
	sprintf_s(GRAVIDADE_Formatado, 3, "%02d", defeito.gravidade);
	strcpy_s(ID_FOTO_Formatado, 7, defeito.id_foto.c_str());
	strcpy_s(TEMPO_Formatado, 13, SerializarTempo(defeito.tempo).c_str());

	char Mensagem_Serializada[37];
	sprintf_s(Mensagem_Serializada, 37, "%s/%d/%s/%s/%d/%s/%s",
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

std::string FormatarDefeitoTira(DefeitoTira defeito) {
	char NSEQ_Formatado[6];
	char CADEIRA_Formatado[3];
	char GRAVIDADE_Formatado[3];
	char ID_FOTO_Formatado[7];
	char TEMPO_Formatado[13];

	sprintf_s(NSEQ_Formatado, 6, "%05d", defeito.numero);
	sprintf_s(CADEIRA_Formatado, 3, "%02d", defeito.cadeira);
	sprintf_s(GRAVIDADE_Formatado, 3, "%02d", defeito.gravidade);
	strcpy_s(ID_FOTO_Formatado, 7, defeito.id_foto.c_str());
	strcpy_s(TEMPO_Formatado, 13, SerializarTempo(defeito.tempo).c_str());

	char Mensagem_Formatada[71];
	sprintf_s(Mensagem_Formatada, 71, "%s NSEQ: %s CAD: %s ID FOTO: %s GRAV: %s CLASSE: %d",
		TEMPO_Formatado,
		NSEQ_Formatado,
		CADEIRA_Formatado,
		ID_FOTO_Formatado,
		GRAVIDADE_Formatado,
		defeito.classe
	);

	return Mensagem_Formatada;
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