#pragma warning(disable:4996)
#include "DadosDeProcesso.h"
#include <time.h>
#include "RandomUtil.h"
#include <string>

int Numero_Dado_Processo = 1;

DadosProcesso GerarDadosProcesso() {
	DadosProcesso dados;

	srand(time(NULL));
	dados.numero = Numero_Dado_Processo;
	dados.tipo = 22;
	dados.cadeira = rand() % 6 + 1;
	dados.id = GerarIdAleatorio(8);
	dados.temp = 200.0f + (((float)rand() / (float)(RAND_MAX)) * 800);
	dados.vel = 200.0f + (((float)rand() / (float)(RAND_MAX)) * 800);
	dados.tempo = GerarTempoAtual();

	Numero_Dado_Processo++;
	return dados;
}

std::string SerializarDadosProcesso(DadosProcesso dados) {
	char NSEQ_Formatado[6];
	char CADEIRA_Formatado[3];
	char ID_Formatado[9];
	char TEMP_Formatado[6];
	char VEL_Formatado[6];
	char TEMPO_Formatado[13];

	sprintf(NSEQ_Formatado, "%05d", dados.numero);
	sprintf(CADEIRA_Formatado, "%02d", dados.cadeira);
	strcpy(ID_Formatado, dados.id.c_str());
	sprintf(TEMP_Formatado, "%05.1f", dados.temp);
	sprintf(VEL_Formatado, "%05.1f", dados.vel);
	strcpy(TEMPO_Formatado, SerializarTempo(dados.tempo).c_str());

	char Mensagem_Serializada[46];
	sprintf(Mensagem_Serializada, "%s/%d/%s/%s/%s/%s/%s",
		NSEQ_Formatado,
		dados.tipo,
		CADEIRA_Formatado,
		ID_Formatado,
		TEMP_Formatado,
		VEL_Formatado,
		TEMPO_Formatado
	);

	return Mensagem_Serializada;
}

DadosProcesso DesserializarDadosProcesso(std::string dadosString) {
	DadosProcesso dados;
	if (dadosString.size() != 45) {
		dados.tipo = 0;
		return dados;
	}

	std::string delim = "/";
	auto start = 0U;
	auto end = dadosString.find(delim);
	int posicao = 0;
	while (end != std::string::npos)
	{
		switch (posicao) {
		case 0:
			dados.numero = std::stoi(dadosString.substr(start, end - start));
			break;
		case 1:
			dados.tipo = std::stoi(dadosString.substr(start, end - start));
			break;
		case 2:
			dados.cadeira = std::stoi(dadosString.substr(start, end - start));
			break;
		case 3:
			dados.id = dadosString.substr(start, end - start);
			break;
		case 4:
			dados.temp = std::stof(dadosString.substr(start, end - start));
			break;
		case 5:
			dados.vel = std::stof(dadosString.substr(start, end - start));
			break;
		}
		start = end + delim.length();
		end = dadosString.find(delim, start);
		posicao++;
	}
	dados.tempo = DesserializarTempo(dadosString.substr(start, end - start));
	return dados;
}