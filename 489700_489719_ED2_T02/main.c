#include <stdio.h>
#include <stdlib.h>

#include "489700_489719_ED2_T02.h"

#define CADASTRAR_SERIE 1
#define ALTERAR_NOTA 2
#define REMOVER_SERIE 3
#define BUSCAR_SERIE 4
#define LISTAR_SERIE 5
#define LIBERAR_ESPACO 6
#define FINALIZAR 7

#define ERRO_LEITURAARQUIVO "Erro com leitura de arquivo!\n"
#define ERRO_SERIECADASTRADA "Serie ja esta cadastrada!\n"
#define ERRO_SERIENAOENCONTRADA "Registro nao encontrado!\n"
#define ERRO_MODOINVALIDO "Modo de busca invalido!\n"
#define ERRO_ARQUIVOVAZIO "Arquivo vazio!\n"
#define ERRO_OPINVALIDA "Opcao invalida!\n"
#define ERRO_REALOCACAO "Erro com realocação de memória!\n"

#define PRINT_LERCHAVE "Chave da serie: "
#define PRINT_LERDIRETOR "Diretor da serie: "
#define PRINT_LERTITULO "Titulo da serie: "

#define LER_STRING "\n%[^\n]"

int main(int argc, char *argv[]){
	NoPrimario iprimary2;
	IndicePrimario iprimary;
	IndiceSecundario ititle, idirector;
	char buffer[MAX];
	int op, opSec, retorno;;


	//Criação dos índices em RAM
	verificarIndices(&iprimary, &iprimary2, &ititle, &idirector);
	do{
		system("clear");
		buildMenu(&op);
		switch(op){
			case CADASTRAR_SERIE:
				retorno = cadastrarSerie(&iprimary, &ititle, &idirector);
				if (retorno == 0){
					printf (ERRO_SERIECADASTRADA);
				}
				break;

			case ALTERAR_NOTA:
				printf (PRINT_LERCHAVE);
				scanf  (LER_STRING, buffer);
				maiusculo(buffer);
				retorno = alterarNota(&iprimary, buffer);
				if (retorno == 0){
					printf (ERRO_SERIENAOENCONTRADA);
				}
				break;

			case REMOVER_SERIE:
				printf (PRINT_LERCHAVE);
				scanf  (LER_STRING, buffer);
				maiusculo(buffer);
				retorno = removerSerie(buffer, &iprimary, &ititle	, &idirector);
				if (retorno == -1){
					printf (ERRO_REALOCACAO);
				} else if (retorno == 0){
					printf (ERRO_SERIENAOENCONTRADA);
				}
				break;

			case BUSCAR_SERIE:
				buildMenuBusca(&opSec);
				if (opSec == 1){
					printf (PRINT_LERCHAVE);
				} else if (opSec == 2){
					printf (PRINT_LERTITULO);
				} else if (opSec == 3){
					printf (PRINT_LERDIRETOR);
				} 
				scanf  (LER_STRING, buffer);
				if (opSec == 1) maiusculo(buffer);
				retorno = buscarSerie(buffer, opSec, &iprimary, &ititle, &idirector);
				if (retorno == -2){
					printf (ERRO_MODOINVALIDO);
				} else if (retorno == 0){
					printf (ERRO_SERIENAOENCONTRADA);
				}
				break;

			case LISTAR_SERIE:
				buildMenuLista(&opSec);
				if (opSec == 1)
					printf ("Funcionalidade não implementada\n");
				else{
					retorno = listarSerie(opSec, &iprimary, &ititle, &idirector);
					if (retorno == -1){
						printf (ERRO_ARQUIVOVAZIO);
					} else if (retorno == 0){
						printf (ERRO_MODOINVALIDO);
					}
				}
				break;

			case LIBERAR_ESPACO:
				if (!(liberarEspaco(&iprimary, &ititle, &idirector)))
					printf (ERRO_LEITURAARQUIVO);
				break;

			case FINALIZAR:
				apagarIndice(&iprimary);
				apagarIndiceSecundario(&ititle);
				apagarIndiceSecundario(&idirector);
				return 0;
				break;
				
			default:
				printf (ERRO_OPINVALIDA);
		}
		printf ("-- Pressione uma tecla para continuar\n");
		getchar();
		getchar();
	} while (op != 7);

	return 0;
}