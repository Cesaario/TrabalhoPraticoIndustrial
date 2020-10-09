#include "RandomUtil.h"
#include <time.h>

std::string GerarIdAleatorio(int tamanho) {
	std::string Random_Id;
	static const char caracteres[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	srand(time(NULL));

	for (int i = 0; i < tamanho; ++i)
		Random_Id += caracteres[rand() % (sizeof(caracteres) - 1)];

	return Random_Id;
}