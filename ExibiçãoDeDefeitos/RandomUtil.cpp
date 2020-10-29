#include "RandomUtil.h"
#include <time.h>

std::string GerarIdAleatorio(int Tipo_Mensagem) {
	std::string Random_Id;
	static const char caracteres[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static const char numeros[] =
		"0123456789";

	int Tamanho_Letras = Tipo_Mensagem == MENSAGEM_DEFEITO ? 2 : 3;
	int Tamanho_Numeros = 4;


	for (int i = 0; i < Tamanho_Letras; i++)
		Random_Id += caracteres[rand() % (sizeof(caracteres) - 1)];

	if (Tipo_Mensagem == MENSAGEM_DADO)
		Random_Id += "-";

	for (int i = 0; i < Tamanho_Numeros; i++)
		Random_Id += numeros[rand() % (sizeof(numeros) - 1)];

	return Random_Id;
}