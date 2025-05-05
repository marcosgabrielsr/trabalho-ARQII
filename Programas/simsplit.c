//-----------------------------------------------------------------------------
// Simulador de cache básica: UM ÚNICO NÍVEL DE CACHE SPLIT, SEPARADA PARA INSTRUÇÕES E DADOS
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
int nAcessosI1,		// Número total de acessos a instruções
	 nAcessosD1,	// Número total de acessos a dados
	 nFalhasI1,		// Número de falhas na cache L1
     nFalhasD1;     // Número de falhas na cache D1

tCache cacheI1;	// Cache I1
tCache cacheD1; // Cache D1
/* Criar linha similar para cacheD1*/ 

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
			if (tipoAcesso == 'I') { // Acesso é leitura de uma instrução
                nAcessosI1++;

                // Procura bloco na cache I1 e, se não encontra, insere bloco na cache
			    // Valor de retorno = 0 (acerto), 1 (falha sem substituição) ou 2 (falha com substituição)
			    resultAcesso = buscaInsereCache(&cacheI1, endereco);

                if ((resultAcesso == 1) || (resultAcesso == 2))	// Se houve falha na cache
				    nFalhasI1 ++;
            }
			else {	// Acesso é leitura de dado ('L') ou escrita de dado ('S')
				nAcessosD1++;
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

	FILE *arqConfig;	// Arquivo com configuração da cache

	if (argc != 3)
	{
		printf("\nUso: simsplit arquivoConfiguracao arquivoAcessos\n");
		exit(0);
	}

	// Lê arquivo de configuracao e obtem nBlocos, associatividade e nPalavrasBloco da cache I1 e D1
	strcpy(nomeArqConfig, argv[1]);
	arqConfig = fopen(nomeArqConfig, "rt");
	if (arqConfig == NULL)
	{
		printf("\nArquivo de configuração não encontrado\n");
		exit(0);
	}
	fscanf(arqConfig, "%d %d %d", &nBlocosI1, &associatividadeI1, &nPalavrasBlocoI1);
    /* Criar linha similar para cacheD1*/ 
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
    /* Criar linha similar para cacheD1*/ 
}

//-----------------------------------------------------------------------------
void finaliza(void)
{
	// Fecha arquivo de acessos
	fclose(arqAcessos);

	// Imprime medidas de desempenho
	printf("nAcessosI1: %d\n", nAcessosI1);
	printf("nAcessosD1: %d\n", nAcessosD1);
	printf("nFalhasL1: %d\n", nFalhasI1);
    /* Criar linha similar para cacheD1*/ 

	// Libera estrutura de dados da cache
	liberaCache(&cacheI1);
    /* Criar linha similar para cacheD1*/ 
}

//-----------------------------------------------------------------------------
