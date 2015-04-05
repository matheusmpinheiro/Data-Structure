

#include <stdio.h>
#include <stdlib.h>

#define MAX 980

typedef struct {
	char cod[9];
	char tituloPt[MAX+1];
	char tituloOr[MAX+1];
	char diretor[MAX+1];
	char genero[MAX+1];
	char duracao[4];
	char estreia[5];
	char encerramento[5];
	char nTemporadas[3];
	char nEpisodios[5];
	char pais[MAX+1];
	int  nota;
	char resumo[MAX+1];
	int  total;
} Serie;


typedef struct {
	char cod[8];
	int RRN;
} ItemPrimario;

typedef struct noPrimario{
	ItemPrimario *itens;
	struct noPrimario **filhos;
	int qtdChaves;
	int folha;
} NoPrimario;

typedef struct {
	int quantidade;
	ItemPrimario *itens;
} IndicePrimario;

int ordem;
//typedef NoPrimario IndicePrimario;


typedef struct {
	char chave[200];
	char cod[9];
} ItemSecundario;

typedef struct {
	int quantidade;
	ItemSecundario *itens;
} IndiceSecundario;

/***  Rotinas Indices  ***/
void verificarIndices(IndicePrimario *primario, NoPrimario *iprimary, IndiceSecundario *ititle, IndiceSecundario *idirector);
void criarIndiceTitulo(IndiceSecundario *indice);
void criarIndiceDiretor(IndiceSecundario *indice);
void buscarIndiceSecundarioRelativo(IndiceSecundario *indice, char *cod, int *indices, int *qtd);
int  buscarIndiceSecundario(IndiceSecundario *indice, char *cod);
int  inserirIndiceSecundario(IndiceSecundario *indice, ItemSecundario item);
void deslocarDireitaSecundario(IndiceSecundario *indice, int n);
void deslocarEsquerdaSecundario(IndiceSecundario *indice, int n);
int  removerIndiceSecundario(IndiceSecundario *indice, char *cod);
void imprimir2(IndiceSecundario *indice);
void apagarIndiceSecundario(IndiceSecundario *indice);

/***  Rotinas Indice Primario  ***/
void criarIndice(IndicePrimario *indice);
void apagarIndice(IndicePrimario *indice);
int  buscarIndice(IndicePrimario *indice, char *cod);
int  inserirIndice(IndicePrimario *indice, ItemPrimario item);
void deslocarDireitaPrimario(IndicePrimario *indice, int n);
void deslocarEsquerdaPrimario(IndicePrimario *indice, int n);
int  removerIndicePrimario(IndicePrimario *indice, char *cod);

/***  Rotinas Séries  ***/
void imprimirSerie(Serie serie);
Serie carregarSerie(FILE *pArq);
Serie lerSerie();
int  cadastrarSerie(IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor);
int  removerSerie(char *chave, IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *autor);
int  alterarNota(IndicePrimario *indice, char *chave);
int  buscarSerie(char *chave, int modo, IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor);
int  listarSerie(int modo, IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor);


/*** Tentativas funções Arvore B ***/
int  criarNo(NoPrimario *pnovo);
int  criarArvoreB(NoPrimario *raiz);
int  buscaArvoreB(NoPrimario *raiz, char *cod, ItemPrimario *aux, int *indice);
int  splitArvoreB(NoPrimario *x, int i, NoPrimario *y);
int  inserirNoNaoCheio(NoPrimario *x, ItemPrimario item);
int  inserirArvoreB(NoPrimario *raiz, ItemPrimario item);
void imprimeB(NoPrimario *no, int nivel);


/*** Auxiliares  ***/
int  liberarEspaco(IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor);
int  isInt(char *string);
void maiusculo(char *string);

void buildMenu(int *op);
void buildMenuBusca(int *op);
void buildMenuLista(int *op);

void imprimir(IndicePrimario *indice);
void imprimir2(IndiceSecundario *indice);