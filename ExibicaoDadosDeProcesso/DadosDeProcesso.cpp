#include "DadosDeProcesso.h"
#include <time.h>
#include "RandomUtil.h"
#include <string>

int Numero_Dado_Processo = 1;

DadosProcesso GerarDadosProcesso() {
	DadosProcesso dados;


	dados.numero = Numero_Dado_Processo;
	dados.tipo = 22;
	dados.cadeira = rand() % 6 + 1;
	dados.id = GerarIdAleatorio(MENSAGEM_DADO);
	dados.temp = 200.0f + (((float)rand() / (float)(RAND_MAX)) * 800.0f);
	dados.vel = 200.0f + (((float)rand() / (float)(RAND_MAX)) * 800.0f);
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

	sprintf_s(NSEQ_Formatado, 6, "%05d", dados.numero);
	sprintf_s(CADEIRA_Formatado, 3, "%02d", dados.cadeira);
	strcpy_s(ID_Formatado, 9, dados.id.c_str());
	sprintf_s(TEMP_Formatado, 6, "%05.1f", dados.temp);
	sprintf_s(VEL_Formatado, 6, "%05.1f", dados.vel);
	strcpy_s(TEMPO_Formatado, 13, SerializarTempo(dados.tempo).c_str());

	char Mensagem_Serializada[46];
	sprintf_s(Mensagem_Serializada, 46, "%s/%d/%s/%s/%s/%s/%s",
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

std::string FormatarDadosProcesso(DadosProcesso dados) {
	char NSEQ_Formatado[6];
	char CADEIRA_Formatado[3];
	char ID_Formatado[9];
	char TEMP_Formatado[6];
	char VEL_Formatado[6];
	char TEMPO_Formatado[13];

	sprintf_s(NSEQ_Formatado, 6, "%05d", dados.numero);
	sprintf_s(CADEIRA_Formatado, 3, "%02d", dados.cadeira);
	strcpy_s(ID_Formatado, 9, dados.id.c_str());
	sprintf_s(TEMP_Formatado, 6, "%05.1f", dados.temp);
	sprintf_s(VEL_Formatado, 6, "%05.1f", dados.vel);
	strcpy_s(TEMPO_Formatado, 13, SerializarTempo(dados.tempo).c_str());

	char Mensagem_Formatada[81];
	sprintf_s(Mensagem_Formatada, 81, "%s NSEQ: %s CAD: %s ID PLACA: %s TEMP: %s VEL: %s",
		TEMPO_Formatado,
		NSEQ_Formatado,
		CADEIRA_Formatado,
		ID_Formatado,
		TEMP_Formatado,
		VEL_Formatado
	);

	return Mensagem_Formatada;
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