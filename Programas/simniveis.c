//----------------------------------------------------------------------------- 
// Simulador de cache básica: UM ÚNICO NÍVEL DE CACHE, UNIFICADA PARA INSTRUÇÕES E DADOS
// Para compilar: gcc cache.c simbasica.c -o simbasica -Wall
// Para executar: simbasica arquivoConfiguracao arquivoAcessos
//----------------------------------------------------------------------------- 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"    // Módulo que implementa cache genérica

//-----------------------------------------------------------------------------
// Variáveis de desempenho
int nAcessosI1,    // Número total de acessos a instruções
    nAcessosD1,    // Número total de acessos a dados
    nAcessosL2,    // Número total de acessos ao L2
    nFalhasI1,     // Número de falhas na cache I1
    nFalhasD1,     // Número de falhas na cache D1
    nFalhasL2;     // Número de falhas na cache L2

// Caches

tCache cacheI1;   // Cache I1
 tCache cacheD1;   // Cache D1
 tCache cacheL2;   // Cache L2

FILE *arqAcessos;  // Arquivo de trace com acessos a memória

//-----------------------------------------------------------------------------
// Protótipos de funções

void inicializa(int argc, char **argv);
void finaliza(void);

//-----------------------------------------------------------------------------
// Programa principal

int main(int argc, char **argv)
{
    char tipoAcesso; // 'I', 'L' ou 'S'
    int result, endereco;
    int resultAcesso;

    inicializa(argc, argv);

    while (!feof(arqAcessos))
    {
        result = fscanf(arqAcessos, " %c %d", &tipoAcesso, &endereco);
        if (result == 2)
        {
            if (tipoAcesso == 'I') {  // Instrução
                nAcessosI1++;
                resultAcesso = buscaInsereCache(&cacheI1, endereco);
                if (resultAcesso == 1 || resultAcesso == 2) {
                    nFalhasI1++;
                    // Acesso a L2
                    nAcessosL2++;
                    resultAcesso = buscaInsereCache(&cacheL2, endereco);
                    if (resultAcesso == 1 || resultAcesso == 2)
                        nFalhasL2++;
                }
            }
            else if (tipoAcesso == 'L' || tipoAcesso == 'S') {  // Dados
                nAcessosD1++;
                resultAcesso = buscaInsereCache(&cacheD1, endereco);
                if (resultAcesso == 1 || resultAcesso == 2) {
                    nFalhasD1++;
                    // Acesso a L2
                    nAcessosL2++;
                    resultAcesso = buscaInsereCache(&cacheL2, endereco);
                    if (resultAcesso == 1 || resultAcesso == 2)
                        nFalhasL2++;
                }
            }
            // Outros caracteres são ignorados
        }
    }

    finaliza();
    return 0;
}

//-----------------------------------------------------------------------------
void inicializa(int argc, char **argv)
{
    char nomeArqConfig[100], nomeArqAcessos[100];
    unsigned int nBlocosI1, assocI1, nWordsI1;
    unsigned int nBlocosD1, assocD1, nWordsD1;
    unsigned int nBlocosL2, assocL2, nWordsL2;
    FILE *arqConfig;

    if (argc != 3) {
        printf("Uso: simbasica arquivoConfiguracao arquivoAcessos\n");
        exit(1);
    }

    strcpy(nomeArqConfig, argv[1]);
    arqConfig = fopen(nomeArqConfig, "rt");
    if (!arqConfig) {
        printf("Arquivo de configuração não encontrado\n");
        exit(1);
    }
    fscanf(arqConfig, "%u %u %u", &nBlocosI1, &assocI1, &nWordsI1);
    fscanf(arqConfig, "%u %u %u", &nBlocosD1, &assocD1, &nWordsD1);
    fscanf(arqConfig, "%u %u %u", &nBlocosL2, &assocL2, &nWordsL2);
    fclose(arqConfig);

    strcpy(nomeArqAcessos, argv[2]);
    arqAcessos = fopen(nomeArqAcessos, "rt");
    if (!arqAcessos) {
        printf("Arquivo de acessos não encontrado\n");
        exit(1);
    }

    // Inicializa contadores
    nAcessosI1 = nAcessosD1 = nAcessosL2 = 0;
    nFalhasI1 = nFalhasD1 = nFalhasL2 = 0;

    // Aloca caches
    alocaCache(&cacheI1, nBlocosI1, assocI1, nWordsI1);
    alocaCache(&cacheD1, nBlocosD1, assocD1, nWordsD1);
    alocaCache(&cacheL2, nBlocosL2, assocL2, nWordsL2);
}

//-----------------------------------------------------------------------------
void finaliza(void)
{
    fclose(arqAcessos);

    printf("nAcessosI1: %d\n", nAcessosI1);
    printf("nAcessosD1: %d\n", nAcessosD1);
    printf("nAcessosL2: %d\n", nAcessosL2);
    printf("nFalhasI1: %d\n", nFalhasI1);
    printf("nFalhasD1: %d\n", nFalhasD1);
    printf("nFalhasL2: %d\n", nFalhasL2);

    liberaCache(&cacheI1);
    liberaCache(&cacheD1);
    liberaCache(&cacheL2);
}
