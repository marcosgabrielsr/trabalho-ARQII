//-----------------------------------------------------------------------------
// Simulador de cache básica: UM ÚNICO NÍVEL DE CACHE, DIVIDIDA PARA INSTRUÇÕES E DADOS
//
// Para compilar: gcc cache.c simsplit.c -o simsplit -Wall
// Para executar: simsplit arquivoConfiguracao arquivoAcessos
//-----------------------------------------------------------------------------
// Bibliotecas

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"	// Módulo que implementa cache genérica

//-----------------------------------------------------------------------------
// Variaveis

						// Medidas de desempenho
int nAcessosI1,		// Número total de acessos a instruções (Modificado Nome e adionado informação)
	 nAcessosD1,	// Número total de acessos a dados
	 nFalhasI1,	    // Número de falhas na cache I1
     nFalhasD1;    // Número de falhas na cache D1

tCache cacheI1;	// Cache I1
tCache cacheD1; // cache D1

FILE *arqAcessos;	// Arquivo de trace com acessos a memória

//-----------------------------------------------------------------------------
// Prototipos de funcoes

void inicializa(int, char **);
void finaliza(void);

//-----------------------------------------------------------------------------
// Programa principal

int main(int argc, char **argv)
{
	char tipoAcesso;	// 'I' (leitura de instrução), 'L' (LOAD = leitura de dado) ou 'S' (STORE = escrita de dado)
	int result,
		 resultAcesso,
		 endereco;		// Endereço de memória acessado

	inicializa(argc, argv);

	// Enquanto não termina arquivo de acessos
	while (! feof(arqAcessos))
	{
		// Lê um endereço do arquivo de acessos
		result = fscanf(arqAcessos, "%c %d\n", &tipoAcesso, &endereco);

		if ((result != 0) && (result != EOF))
		{
			if (tipoAcesso == 'I')	// Acesso é leitura de uma instrução
			{
				nAcessosI1++;
                resultAcesso = buscaInsereCache(&cacheI1, endereco);
                if (resultAcesso == 1 || resultAcesso == 2 ){
                    nFalhasI1++;
                }
			}

			else{	// Acesso é leitura de dado ('L') ou escrita de dado ('S')
				nAcessosD1++;
                resultAcesso = buscaInsereCache(&cacheD1, endereco);

			    if ((resultAcesso == 1) || (resultAcesso == 2))	// Se houve falha na cache
				    nFalhasD1 ++;
		}
	}
}


	finaliza();

	return 0;
}
//-----------------------------------------------------------------------------
void inicializa(int argc, char **argv)
{
	char nomeArqConfig[100],
		  nomeArqAcessos[100];

	unsigned int nBlocosI1,				// Número total de blocos da cache I1
					associatividadeI1,	// Número de blocos por conjunto da cache I1
					nPalavrasBlocoI1;	// Número de palavras do bloco da cache I1
    unsigned int nBlocosD1,             // Número total de blocos da cache D1
                    associatividadeD1,  // Número de blocos por conjunto da cache D1
                    nPalavrasBlocoD1;  // Número de palavras do bloco da cache D1

	FILE *arqConfig;	// Arquivo com configuração da cache

	if (argc != 3)
	{
		printf("\nUso: simbasica arquivoConfiguracao arquivoAcessos\n");
		exit(0);
	}

	// Lê arquivo de configuracao e obtem nBlocos, associatividade e nPalavrasBloco da cache L1
	strcpy(nomeArqConfig, argv[1]);
	arqConfig = fopen(nomeArqConfig, "rt");
	if (arqConfig == NULL)
	{
		printf("\nArquivo de configuração não encontrado\n");
		exit(0);
	}
	fscanf(arqConfig, "%d %d %d", &nBlocosI1, &associatividadeI1, &nPalavrasBlocoI1);
    fscanf(arqConfig, "%d %d %d", &nBlocosD1, &associatividadeD1, &nPalavrasBlocoD1);
	fclose(arqConfig);

	// Abre arquivo de acessos
	strcpy(nomeArqAcessos, argv[2]);
   arqAcessos = fopen(nomeArqAcessos, "rt");
	if (arqAcessos == NULL)
	{
		printf("\nArquivo de acessos não encontrado\n");
		exit(0);
	}

	// Inicializa medidas de desempenho
	nAcessosI1 = 0;
	nAcessosD1 = 0;
	nFalhasI1 = 0;
    nFalhasD1 = 0;

	// Aloca e inicializa estrutura de dados da cache L1
	alocaCache(&cacheI1, nBlocosI1, associatividadeI1, nPalavrasBlocoI1);
    alocaCache(&cacheD1, nBlocosD1, associatividadeD1, nPalavrasBlocoD1);
}

//-----------------------------------------------------------------------------
void finaliza(void)
{
	// Fecha arquivo de acessos
	fclose(arqAcessos);

	// Imprime medidas de desempenho
	printf("nAcessosI: %d\n", nAcessosI1);
	printf("nAcessosD: %d\n", nAcessosD1);
	printf("nFalhasI1: %d\n", nFalhasI1);
    printf("nFalhasD1: %d\n", nFalhasD1);

	// Libera estrutura de dados da cache
	liberaCache(&cacheI1);
    liberaCache(&cacheD1);
}

//-----------------------------------------------------------------------------
