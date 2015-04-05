#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "489700_489719_ED2_T02.h"


/* ---------------------------------------------------------------------------------------------------/
/                                 Criar os indices							                          /
/ Funcionamento:																					  /
/    Verifica se o arquivo de dados existe. Caso exista, abre para escrita e leitura, caso contrário, /
/ o arquivo é criado e aberto para escrita e leitura. Após isso os índices são criados em RAM         /
/----------------------------------------------------------------------------------------------------*/

void verificarIndices(IndicePrimario *primario, NoPrimario *iprimary, IndiceSecundario *ititle, IndiceSecundario *idirector){
	//Abre o arquivo para escrita e leitura. Se o arquivo não existe, ele é criado.
	FILE *arq = fopen("series.dat", "r+w");
	fclose(arq);
	printf ("Entre com a ordem da ArvoreB: ");
	scanf (" %d", &ordem);
	ititle->itens = NULL;
	ititle->quantidade = 0;
	idirector->itens = NULL;
	idirector->quantidade = 0;
	if (!criarArvoreB(iprimary)){
		printf ("Erro na criação!\n");
	}
	printf ("%d\n",iprimary->folha);
	criarIndice(primario);
	criarIndiceTitulo(ititle);
	criarIndiceDiretor(idirector);
}

/* ---------------------------------------------------------------------------------------------------/
/                                 Carregar o índice primário em RAM.							      /
/ Funcionamento:																					  /
/    Verifica-se se o arquivo de índice existe. Caso exista, verifica-se se ele está atualizado.	  /
/ Caso esteja atualizado, o arquivo é percorrido e os índices são inseridos na estrutura de dados	  /
/ interna, armazenada em RAM.                                                                         /
/    Caso o índice não esteja atualizado ou não exista, o arquivo de dados é percorrido e é extraído  /
/ o código e o RRN de cada registro, e então esses dados são inseridos na estrutura de dados interna. /
/----------------------------------------------------------------------------------------------------*/

void criarIndice(IndicePrimario *indice){
	//Inicialização da estrutra de dados
	indice->itens = NULL;
	indice->quantidade = 0;

	//Arquivo de índices não existe ou está desatualizado
	FILE *pSer = fopen("series.dat", "r");
	
	char aux;
	int RRN = 0;
	aux = getc(pSer);
	rewind(pSer);
	
	//Varredura do arquivo de dados
	while (aux != EOF){
		//Arquivo não-vazio
		ItemPrimario item;
		char buffer;
		int i=0;

		item.RRN = RRN;

		//Captura do código da serie
		do{
			buffer = getc(pSer);
			if (buffer != '@')
				item.cod[i++] = buffer;
		} while (buffer != '@');

		//Caso o codigo não contenha marcador de remoção, insere-se no indice
		if ((item.cod[0] != '*') && (item.cod[1] != '|'))
			inserirIndice(indice, item);

		//Incremento do RRN
		RRN += 1024;

		//Salto para a proxima serie
		fseek(pSer, 1015, SEEK_CUR);

		//Captura do elemento para verificação de fim de arquivo
		aux = getc(pSer);
		fseek(pSer, -1, SEEK_CUR);
	}
	fclose(pSer);
}

void criarIndiceTitulo(IndiceSecundario *indice){
	FILE *pSer = fopen("series.dat", "r+w");
	char aux;
	int qtd = 0;
	aux = getc(pSer);
	rewind(pSer);
	while (aux != EOF){
		//Arquivo não-vazio
		ItemSecundario item;
		char buffer;
		int i=0;

		//Leitura do Código
		do{
			buffer = getc(pSer);
			if (buffer != '@')
				item.cod[i++] = buffer;
		} while (buffer != '@');
		item.cod[i] = '\0';
		
		i = 0;
		do{
			buffer = getc(pSer);
			if (buffer != '@')
				item.chave[i++] = buffer;
		} while (buffer != '@');
		item.chave[i] = '\0';

		if ((item.cod[0] != '*') && (item.cod[1] != '|'))
			inserirIndiceSecundario(indice, item);
		qtd++;
		fseek(pSer, qtd * 1024, SEEK_SET);
		aux = getc(pSer);
		fseek(pSer, -1, SEEK_CUR);
	}
	fclose(pSer);
}

void criarIndiceDiretor(IndiceSecundario *indice){
	indice->itens = NULL;
	indice->quantidade = 0;

	FILE *pSer = fopen("series.dat", "r");

	char aux;
	int qtd = 0;
	aux = getc(pSer);
	rewind(pSer);
	while (aux != EOF){
		//Arquivo não-vazio
		//Formato CODIGO@TITULO@TITULO@DIRETOR
		ItemSecundario item;
		char buffer;
		int i=0;

		//Leitura do Código
		do{
			buffer = getc(pSer);
			if (buffer != '@')
				item.cod[i++] = buffer;
		} while (buffer != '@');
		item.cod[i] = '\0';

		//Percorrer arquivo até chegar no diretor
		for (i=0; i<2; i++){
			do{
				buffer = getc(pSer);
			} while (buffer != '@');
		}

		i = 0;
		do{
			buffer = getc(pSer);
			if (buffer != '@')
				item.chave[i++] = buffer;
		} while (buffer != '@');
		item.chave[i] = '\0';

		if ((item.cod[0] != '*') && (item.cod[1] != '|'))
			inserirIndiceSecundario(indice, item);
		qtd++;
		fseek(pSer, qtd * 1024, SEEK_SET);
		aux = getc(pSer);
		fseek(pSer, -1, SEEK_CUR);
	}
	fclose(pSer);
}

/* ---------------------------------------------------------------------------------------------------/
/ 					                           Buscar Índice										  /
/ Funcionamento:																					  /
/    Inicialmente verifica-se se existe algum elemento no índice. Após isso, utiliza-se busca binaria /
/ para procurar pelo código inserido no indice armazenado em RAM. Se o código for encontrado, verifi- /
/ ca-se se o mesmo contém o marcador de remoção. Caso contenha, a função termina com valor de retorno /
/ de erro.																							  /
/ Retornos:																							  /
/    -1 -> Índice não encontrado ou arquivo vazio 													  /
/    'I'-> Posição da série no vetor de índice 														  /
/----------------------------------------------------------------------------------------------------*/
int  buscarIndice(IndicePrimario *indice, char *cod){
	//Verifica se existe alguma série no índice
	if (indice->quantidade == 0)
		return -1;

	//Busca binária iterativa
	int esq = 0;
	int dir = indice->quantidade-1;
	int meio;
	int aux;
	while (esq <= dir) {
		meio = esq + (dir-esq)/2;
		aux = strcmp(cod, indice->itens[meio].cod);
		if (aux == 0){
			return meio;
		} else if (aux < 0)
			dir = meio-1;
		else
			esq = meio+1;
	}
	return -1;
}

int  buscarIndiceSecundario(IndiceSecundario *indice, char *cod){
	int i;
	for (i=0; i < indice->quantidade; i++){
		if (strcmp(indice->itens[i].cod, cod) == 0){
			return i;
		}
	}
	return -1;
}


/* ---------------------------------------------------------------------------------------------------/
/ 									    Busca Relativa		  										  /
/ Faz a busca relativa no indice secundario passado como parametro. Armazena em qtd quantos registros /
/ foram encontrados e em indices os indices dos respectivos registros								  /
/----------------------------------------------------------------------------------------------------*/ 
void buscarIndiceSecundarioRelativo(IndiceSecundario *indice, char *cod, int *indices, int *qtd){
	int i, x;
	*qtd = 0;
	int tam = strlen(cod);
	for (i=0, x=0; i<indice->quantidade; i++){
		int j, k;
		int cont = 0;
		for (j=0, k=0; j < strlen(indice->itens[i].chave); j++){
			if (indice->itens[i].chave[j] == cod[k]){
				cont++;
				k++;
			} else {
				k = 0;
				cont = 0;
			}
			if (cont == tam){
				*qtd += 1;
				indices[x] = i;
				x++;
				j = strlen(indice->itens[i].chave);
			}
		}
	}
}

/* ---------------------------------------------------------------------------------------------------/
/ 					                     Inserir Índice												  /
/ Funcionamento:																					  /
/    Inicialmente busca-se pelo item que será inserido no vetor de índices primários. Caso o item nao /
/ seja encontrado, o espaço de memória reservado é realocado para suportar o novo item. Então encon-  /
/ tramos a posição a qual o novo item deverá ocupar no vetor ordenado. Movemos todos os elementos à   /
/ sua direita e o inserimos no espaço "vazio".														  /
/ Retornos:																							  /
/    -1 -> erro de alocação																			  /
/	 0  -> serie já cadastrada																		  /
/    1  -> serie cadastrada com sucesso 															  /
/----------------------------------------------------------------------------------------------------*/
int  inserirIndice(IndicePrimario *indice, ItemPrimario item){
	// Verificação da existencia do item no vetor de indice primario
	if (buscarIndice(indice, item.cod) != -1)
		return 0;

	ItemPrimario *aux;

	// Realocação de memória para suportar o novo item
	aux = (ItemPrimario *) realloc (indice->itens, (indice->quantidade + 1) * sizeof(ItemPrimario));
	if (aux != NULL){
		int i;

		indice->itens = aux;
		// Encontrar a posição na qual o novo valor será inserido
		for (i=0; (strcmp(indice->itens[i].cod, item.cod) < 0) && (i < indice->quantidade); i++);
		
		// Mover todos os maiores que ele para a direita
		deslocarDireitaPrimario(indice, i);

		// Inserção propriamente dita
		indice->itens[i] = item;
		indice->quantidade += 1;
		return 1;
	}
	return -1;
}

int  inserirIndiceSecundario(IndiceSecundario *indice, ItemSecundario item){
	
	if (buscarIndiceSecundario(indice, item.cod) != -1)
		return 0;

	ItemSecundario *aux;

	aux = (ItemSecundario *) realloc (indice->itens, (indice->quantidade + 1) * sizeof(ItemSecundario));
	if (aux != NULL){
		int i;

		indice->itens = aux;
		for (i=0; (strcmp(indice->itens[i].chave, item.chave) <= 0) && (i < indice->quantidade); i++){
			if (strcmp(indice->itens[i].chave, item.chave) == 0 && strcmp(indice->itens[i].cod, item.cod) > 0)
				break;
		}
		deslocarDireitaSecundario(indice, i);

		indice->itens[i] = item;
		indice->quantidade += 1;
		return 1;
	}
	return -1;
}

/* ---------------------------------------------------------------------------------------------------/
/ 			                          Deslocar Direita Primario										  /
/ Funcionamento:																					  /
/    Percorre o vetor de índices, de trás para frente, movendo todos os ítens uma posição para a di-  /
/ reita até chegar em 'n'.				 															  /
/----------------------------------------------------------------------------------------------------*/
void deslocarDireitaPrimario(IndicePrimario *indice, int n){
	int i;
	for (i = indice->quantidade; i > n; i--){
		indice->itens[i] = indice->itens[i-1];
	}
}

/* ---------------------------------------------------------------------------------------------------/
/ 			                          Deslocar Esquerda Primario									  /
/ Funcionamento:																					  /
/    Análogo à anterior, só que para a esquerda				 										  /
/----------------------------------------------------------------------------------------------------*/
void deslocarDireitaSecundario(IndiceSecundario *indice, int n){
	int i;
	for (i = indice->quantidade; i > n; i--){
		indice->itens[i] = indice->itens[i-1];
	}
}

void deslocarEsquerdaPrimario(IndicePrimario *indice, int n){
	int i;
	for (i = n; i < indice->quantidade - 1; i++){
		indice->itens[i] = indice->itens[i+1];
	}
}

void deslocarEsquerdaSecundario(IndiceSecundario *indice, int n){
	int i;
	for (i = n; i < indice->quantidade - 1; i++){
		indice->itens[i] = indice->itens[i+1];
	}
}


/* ---------------------------------------------------------------------------------------------------/
/ 									    Remover  										  			  /
/ Remove uma entrada do arquivo de indices e realoca a memória										  /
/----------------------------------------------------------------------------------------------------*/ 
int  removerIndicePrimario(IndicePrimario *indice, char *cod){
	int i;
	if ((i = buscarIndice(indice, cod)) == -1)
		return 0;
	deslocarEsquerdaPrimario(indice, i);

	ItemPrimario *aux = (ItemPrimario *) malloc (sizeof(ItemPrimario) * indice->quantidade -1);
	if (aux != NULL){
		for (i=0; i<indice->quantidade - 1; i++){
			aux[i] = indice->itens[i];
		}
		indice->itens = aux;
		indice->quantidade -= 1;
		return 1;
	}

	return -1;
}

int  removerIndiceSecundario(IndiceSecundario *indice, char *cod){
	int i;
	if ((i = buscarIndiceSecundario(indice, cod)) == -1)
		return 0;
	deslocarEsquerdaSecundario(indice, i);

	ItemSecundario *aux = (ItemSecundario *) malloc (sizeof(ItemSecundario) * indice->quantidade -1);
	if (aux != NULL){
		for (i=0; i<indice->quantidade - 1; i++){
			aux[i] = indice->itens[i];
		}
		indice->itens = aux;
		indice->quantidade -= 1;
		return 1;
	}

	return -1;
}

/* ---------------------------------------------------------------------------------------------------/
/                           				 Apagar índice											  /
/ Funcionamento:																					  /
/    Libera a memória alocada pelo índice e zera a quantidade de elementos contido nele				  /
/----------------------------------------------------------------------------------------------------*/
void apagarIndice(IndicePrimario *indice){
	free(indice->itens);
	indice->itens = NULL;
	indice->quantidade = 0;
}

void apagarIndiceSecundario(IndiceSecundario *indice){
	free(indice->itens);
	indice->itens = NULL;
	indice->quantidade = 0;
}

/* ---------------------------------------------------------------------------------------------------/
/ 									         Funções de Impressão   							      /
/----------------------------------------------------------------------------------------------------*/
void imprimir(IndicePrimario *indice) {
	int i;
	for (i=0; i < indice->quantidade; i++){
		printf("%s-%d\n", indice->itens[i].cod, indice->itens[i].RRN);
	}
}

void imprimir2(IndiceSecundario *indice) {
	int i;
	for (i=0; i < indice->quantidade; i++){
		printf("%s-%s\n", indice->itens[i].chave, indice->itens[i].cod);
	}
}

void imprimeB(NoPrimario *no, int nivel){
	int i;
	if (no == NULL){
	//	printf("caso base\n");
		return;
	}
	printf ("%d - ", nivel);
	for (i=0; i< no->qtdChaves; i++){
		if (i!=0)
			printf (", ");
		printf ("%s", no->itens[i].cod);
	}
	printf ("\n");
	if (no->folha == 0){
		for (i=0; i<= no->qtdChaves; i++){
		//	printf ("for\n");
			imprimeB(no->filhos[i], nivel+1);
		}
	}
}

/* ---------------------------------------------------------------------------------------------------/
/											Imprimir série 											  /
/ Funcionamento:																					  /
/    Imprime os dados da série de forma formatada													  /
/----------------------------------------------------------------------------------------------------*/
void imprimirSerie(Serie serie){
	int i = 0;
	
	printf ("-----------------------------------------------------------------\n");
	printf ("Código      : %s\n", serie.cod);
	printf ("Titulo Port : %s\n", serie.tituloPt);
	printf ("Titulo Orig : %s\n", serie.tituloOr);
	printf ("Diretor     : %s\n", serie.diretor);
	printf ("Genero      : %s\n", serie.genero);
	
	//Remover '0's à esquerda de "duracao"
	while(serie.duracao[i] == '0') i++;
	printf ("Duracao     : ");
	for ( ; i<3; i++)
		printf ("%c", serie.duracao[i]);
	printf ("\n");

	printf ("Estreia     : %s\n", serie.estreia);
	printf ("Encerramento: %s\n", serie.encerramento);
	
	//Remover '0's à esquerda de "temporadas"
	i = 0;
	while(serie.nTemporadas[i] == '0') i++;
	printf ("Temporadas  : ");
	for ( ; i<2; i++)
		printf ("%c", serie.nTemporadas[i]);
	printf ("\n");

	//Remover '0's à esquerda de "nEpisodios"
	i = 0;
	while(serie.nEpisodios[i] == '0') i++;
	printf ("Episodios   : ");
	for ( ; i<4; i++)
		printf ("%c", serie.nEpisodios[i]);
	printf ("\n");

	printf ("Pais        : %s\n", serie.pais);
	printf ("Nota        : %d\n", serie.nota);
	printf ("Resumo      : %s\n", serie.resumo);
	printf ("-----------------------------------------------------------------\n");
}

/* ---------------------------------------------------------------------------------------------------/
/											Carregar Serie											  /
/ Funcionamento:																					  /
/    Percorre o arquivo de dados e carrega a série para a qual o ponteiro de arquivo aponta para a es-/
/ trutura de dados interna. Assume-se que o ponteiro de arquivo aponte para o primeiro campo da serie /
/ desejada.																							  /
/ Retorna uma Serie com os dados preenchidos.														  /
/----------------------------------------------------------------------------------------------------*/
Serie carregarSerie(FILE *pArq){
	Serie serie;
	int i;
	char buffer;

	//Leitura e atribuição do Código
	fgets(serie.cod, 9, pArq);
	//Capturar '@'
	fgetc(pArq);
	
	//Leitura e atribuição do Titulo em Portugues
	i = 0;
	do{
		buffer = fgetc(pArq);
		if (buffer != '@')
			serie.tituloPt[i++] = buffer;
	} while (buffer != '@');
	serie.tituloPt[i] = '\0';

	//Leitura e atribuição do Titulo Original.
	i = 0;
	do{
		buffer = fgetc(pArq);
		if (buffer != '@')
			serie.tituloOr[i++] = buffer;
	} while (buffer != '@');
	serie.tituloOr[i] = '\0';
	if (strcmp(serie.tituloOr, "Idem") == 0)
		//Caso o título for "Idem", copiar o Titulo em Portugues
		strcpy(serie.tituloOr, serie.tituloPt);

	//Leitura e atribuição do Diretor
	i = 0;
	do{
		buffer = fgetc(pArq);
		if (buffer != '@')
			serie.diretor[i++] = buffer;
	} while (buffer != '@');
	serie.diretor[i] = '\0';

	//Leitura e atribuição do Genero
	i = 0;
	do{
		buffer = fgetc(pArq);
		if (buffer != '@')
			serie.genero[i++] = buffer;
	} while (buffer != '@');
	serie.genero[i] = '\0';

	//Leitura e atribuição da Duração
	fgets(serie.duracao, 4, pArq);
	//Capturar '@'
	buffer = fgetc(pArq);

	//Leitura e atribuição do Ano de estreia
	fgets(serie.estreia, 5, pArq);
	//Capturar '@'
	buffer = fgetc(pArq);

	//Leitura e atribuição do Ano de encerramento
	fgets(serie.encerramento, 5, pArq);
	//Capturar '@'
	buffer = fgetc(pArq);

	//Leitura e atribuição do Numero de temporadas
	fgets(serie.nTemporadas, 3, pArq);
	//Capturar '@'
	buffer = fgetc(pArq);

	//Leitura e atribuição do Numero de episodios
	fgets(serie.nEpisodios, 5, pArq);
	//Capturar '@'
	buffer = fgetc(pArq);

	//Leitura e atribuição do Pais
	i = 0;
	do{
		buffer = fgetc(pArq);
		if (buffer != '@')
			serie.pais[i++] = buffer;
	} while (buffer != '@');
	serie.pais[i] = '\0';

	//Leitura, conversão para inteiro e atribuição da Nota
	serie.nota = (fgetc(pArq)) - '0';
	//Capturar '@'
	buffer = fgetc(pArq);

	//Leitura e atribuição do Resumo
	i = 0;
	do{
		buffer = fgetc(pArq);
		if (buffer != '@')
			serie.resumo[i++] = buffer;
	} while (buffer != '@');
	serie.resumo[i] = '\0';
	

	return serie;
}

/* ---------------------------------------------------------------------------------------------------/
/											Ler Serie												  /
/ Funcionamento:																					  /
/    Le uma serie informada pelo usuário. É usado uma variável de controle para garantir que todos os /
/ campos contenham pelo menos 1 caractere. O tamanho máximo ocupado pela serie, contando com os deli- /
/ mitadores será de 1024 caracteres.
/    Para cada item de tamanho variável lido, armazena-se seu conteudo em um buffer intermediario. En-/
/ tão copia-se para a variável correspondente um máximo de 'n' caracteres, onde 'n' é a quantidade de /
/ caracteres restantes para garantir a situação acima. Então é decrementado a quantidade real de ca-  /
/ racteres da variável de controle - 1, para que todos os campos de tamanho variável tenham pelo menos/
/ 1 caractere.
/    Também é gerado o Código da série, com os tres primeiros caracteres do titulo em portugues (eli- /
/ minando-se os espaços), os tres primeiros caracteres do sobrenome-nome do diretor e os dois ultimos /
/ digitos da data.																					  /
/ Retorna uma Serie com os dados preenchidos.														  /
/----------------------------------------------------------------------------------------------------*/
Serie lerSerie(){
	char *buffer = (char*) malloc (sizeof(char) * 2000);
	int bufferInt, i, j;

	Serie serie;
	
	//Atribuição do numero maximo de caracteres que podem ser inseridos na serie
	serie.total = MAX;

	//Leitura do titulo em portugues
	printf ("Titulo em portugues: ");
	scanf ("\n%[^\n]", buffer);
	strncpy(serie.tituloPt, buffer, serie.total);
	serie.total -= strlen(serie.tituloPt) - 1;

	//Leitura do titulo original
	printf ("Titulo original: ");
	scanf ("\n%[^\n]", buffer);
	//Verificação se o titulo original é igual ao titulo em portugues
	if (strcmp(buffer, serie.tituloPt) == 0)
		strcpy(serie.tituloOr, "Idem");
	else
		strncpy(serie.tituloOr, buffer, serie.total);
	serie.total -= strlen(serie.tituloOr) - 1;
	
	//Leitura do diretor
	printf ("Diretor: ");
	scanf ("\n%[^\n]", buffer);
	strncpy(serie.diretor, buffer, serie.total);
	serie.total -= strlen(serie.diretor) - 1;

	//Leitura do genero
	printf ("Genero: ");
	scanf ("\n%[^\n]", buffer);
	strncpy(serie.genero, buffer, serie.total);
	serie.total -= strlen(serie.genero) - 1;

	//Leitura da duração
	printf ("Duracao: ");
	//Laço para verificar se a entrada é um inteiro
	do{
		scanf ("\n%[^\n]", buffer);
		bufferInt = strlen(buffer);
		//Atribuição de '0' à esquerda para completar 3 caracteres
		if (bufferInt == 0)
			strcpy(buffer, "000");
		else if (bufferInt == 1){
			buffer[2] = buffer[0];
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 2){
			buffer[2] = buffer[1];
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}
		buffer[3] = '\0';
		if (!(bufferInt = isInt(buffer)))
			printf ("Entrada inválida! Entre novamente: ");
	} while (bufferInt == 0);
	strncpy(serie.duracao, buffer, 4);

	//Leitura do ano de estreia
	printf ("Ano de estreia: ");
	//Laço para verificar se a entrada é um inteiro
	do{
		scanf ("\n%[^\n]", buffer);
		//Atribuição de '0' à esquerda para completar 4 caracteres
		if ((bufferInt = strlen(buffer)) == 0)
			strcpy(buffer, "0000");
		else if (bufferInt == 1){
			buffer[3] = buffer[0];
			buffer[2] = '0';
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 2){
			buffer[3] = buffer[1];
			buffer[2] = buffer[0];
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 3){
			buffer[3] = buffer[2];
			buffer[2] = buffer[1];
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}
		buffer[4] = '\0';
		if (!(bufferInt = isInt(buffer)))
			printf ("Entrada inválida! Entre novamente: ");
	} while (bufferInt == 0);
	strncpy(serie.estreia, buffer, 5);

	//Leitura do ano de encerramento
	printf ("Ano de encerramento: ");
	//Laço para verificar se a entrada é um inteiro
	do{
		scanf ("\n%[^\n]", buffer);
		//Atribuição de '0' à esquerda para completar 4 caracteres
		if ((bufferInt = strlen(buffer)) == 0)
			strcpy(buffer, "0000");
		else if (bufferInt == 1){
			buffer[3] = buffer[0];
			buffer[2] = '0';
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 2){
			buffer[3] = buffer[1];
			buffer[2] = buffer[0];
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 3){
			buffer[3] = buffer[2];
			buffer[2] = buffer[1];
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}
		buffer[4] = '\0';
		if (!(bufferInt = isInt(buffer)))
			printf ("Entrada inválida! Entre novamente: ");
	} while (bufferInt == 0);
	strncpy(serie.encerramento, buffer, 5);

	//Leitura do Numero de temporadas
	printf ("Numero de temporadas: ");
	//Laço para verificar se a entrada é um inteiro
	do{
		scanf ("\n%[^\n]", buffer);
		//Atribuição de '0' à esquerda para completar 2 caracteres
		if ((bufferInt = strlen(buffer)) == 0)
			strcpy(buffer, "00");
		else if (bufferInt == 1){
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}
		buffer[2] = '\0';
		if (!(bufferInt = isInt(buffer)))
			printf ("Entrada inválida! Entre novamente: ");
	} while (bufferInt == 0);
	strncpy(serie.nTemporadas, buffer, 3);

	//Leitura do Numero de episodios
	printf ("Numero de episodios: ");
	//Laço para verificar se a entrada é um inteiro
	do{
		scanf ("\n%[^\n]", buffer);
		//Atribuição de '0' à esquerda para completar 4 caracteres
		if ((bufferInt = strlen(buffer)) == 0)
			strcpy(buffer, "0000");
		else if (bufferInt == 1){
			buffer[3] = buffer[0];
			buffer[2] = '0';
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 2){
			buffer[3] = buffer[1];
			buffer[2] = buffer[0];
			buffer[1] = '0';
			buffer[0] = '0';
		} else if (bufferInt == 3){
			buffer[3] = buffer[2];
			buffer[2] = buffer[1];
			buffer[1] = buffer[0];
			buffer[0] = '0';
		}
		buffer[4] = '\0';
		if (!(bufferInt = isInt(buffer)))
			printf ("Entrada inválida! Entre novamente: ");
	} while (bufferInt == 0);
	strncpy(serie.nEpisodios, buffer, 4);

	//Leitura do Pais
	printf ("Pais: ");
	scanf ("\n%[^\n]", buffer);
	strncpy(serie.pais, buffer, serie.total);
	serie.total -= strlen(serie.pais) - 1;

	//Leitura da nota
	printf ("Nota: ");
	//Laço para verificar se a nota está no intervalo permitido
	do{
		scanf("%d", &bufferInt);
		if (bufferInt < 1 || bufferInt > 5)
			printf ("\tNota inválida! Entre novamente: ");
	} while (bufferInt < 1 || bufferInt > 5);
	serie.nota = bufferInt;

	//Leitura do resumo
	printf ("Resumo: ");
	scanf ("\n%[^\n]", buffer);
	strncpy(serie.resumo, buffer, serie.total);
	serie.total -= strlen(serie.resumo);

	//Geração do código.
	//Capturar os tres primeiros caracteres do titulo, eliminando os espaços
	for (i=0, j=0; i<3 && serie.tituloPt[j] != '\0'; i++, j++){
		while (serie.tituloPt[j] == ' '){
			j++;
		}
		if (serie.tituloPt[j] != '\0'){
			serie.cod[i] = toupper(serie.tituloPt[j]);
		} else
			i--;
	}

	//Caso não haja 3 caracteres alfanumericos no titulo, completa-se com '_'
	for (j=i; j<3; j++){
		serie.cod[j] = '_';
	}

	//Capturar caracteres do diretor. Caso não haja caracteres suficientes, insere-se '_'
	if (serie.diretor[0] != '\0')
		serie.cod[3] = toupper(serie.diretor[0]);
	else
		serie.cod[3] = '_';
	if (serie.diretor[1] != '\0')
		serie.cod[4] = toupper(serie.diretor[1]);
	else
		serie.cod[4] = '_';
	if (serie.diretor[2] != '\0')
		serie.cod[5] = toupper(serie.diretor[2]);
	else
		serie.cod[5] = '_';

	//Capturar caracteres do título
	if (serie.estreia[2] != '\0')
		serie.cod[6] = serie.estreia[2];
	else
		serie.cod[6] = '_';
	if (serie.estreia[3] != '\0')
		serie.cod[7] = serie.estreia[3];
	else
		serie.cod[7] = '_';
	serie.cod[8] = '\0';

	//Liberar memória do buffer
	free(buffer);
	return serie;
}


/* ---------------------------------------------------------------------------------------------------/
/										Cadastrar Serie												  /
/ Funcionamento:																					  /
/    Primeiramente busca-se no arquivo de índices a série que se deseja inserir. Se ela não for encon-/
/ trada, abre-se o arquvio para anexação, cria-se os itens de índices (primario, titulo e diretor),   /
/ grava-se a série no arquivo e insere os ítens de índices nos índices correspondentes				  /																						  /
/ Retornos:																							  /
/    -1 -> Erro de leitura de arquivo 																  /
/	 0  -> Serie já cadastrada 																		  /
/    1  -> Serie cadastrada com sucesso 															  /
/----------------------------------------------------------------------------------------------------*/
int  cadastrarSerie(IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor){
	Serie serie;	
	serie = lerSerie();
	ItemPrimario itemPrimario;
	ItemSecundario itemTitulo, itemDiretor;

	//Busca no índice primário
	
	if (buscarIndice(primario, serie.cod) != -1){
		return 0;
	}
	
	FILE *pArq = fopen("series.dat", "a");

	//----Criação dos ítens de índices
	//Primário
	strcpy(itemPrimario.cod, serie.cod);
	itemPrimario.RRN = ftell(pArq);

	//Secundário - Titulo
	strcpy(itemTitulo.chave, serie.tituloPt);
	strcpy(itemTitulo.cod, serie.cod);

	//Secundário - Diretor
	strcpy(itemDiretor.chave, serie.diretor);
	strcpy(itemDiretor.cod, serie.cod);

	int i;
	fprintf(pArq, "%s@", serie.cod);
	fprintf(pArq, "%s@", serie.tituloPt);
	fprintf(pArq, "%s@", serie.tituloOr);
	fprintf(pArq, "%s@", serie.diretor);
	fprintf(pArq, "%s@", serie.genero);
	fprintf(pArq, "%s@", serie.duracao);
	fprintf(pArq, "%s@", serie.estreia);
	fprintf(pArq, "%s@", serie.encerramento);
	fprintf(pArq, "%s@", serie.nTemporadas);
	fprintf(pArq, "%s@", serie.nEpisodios);
	fprintf(pArq, "%s@", serie.pais);
	fprintf(pArq, "%d@", serie.nota);
	fprintf(pArq, "%s@", serie.resumo);
	
	//Completar o restante dos 1024 caracteres com '#'
	for (i=0; i<serie.total; i++)
		fprintf(pArq, "#");
	fclose(pArq);

	//Inserir nos vetores de índices
	inserirIndice(primario, itemPrimario);
	inserirIndiceSecundario(diretor, itemDiretor);
	inserirIndiceSecundario(titulo, itemTitulo);
	return 1;
}


/* ---------------------------------------------------------------------------------------------------/
/										Remover Serie												  /
/ Funcionamento:																					  /
/    Primeiramente busca-se no arquivo de índices a série que se deseja remover. Se ela for encontra- /
/ da, apontamos o ponteiro de arquivo para o RRN do ítem a ser excluído e substituimos os dois primei-/
/ ros caracteres pelo marcador '*|', significando que a série foi removida, e decrementamos o conta-  /
/ dor de itens no indice. 																			  /
/ Retornos:																							  /
/    -1 -> Erro de leitura de arquivo 																  /
/	 0  -> Serie não encontrada 																	  /
/    1  -> Serie removida com sucesso 																  /
/----------------------------------------------------------------------------------------------------*/
int  removerSerie(char *chave, IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *autor){
	int item;

	if ((item = buscarIndice(primario, chave)) == -1){
		return 0;
	}

	FILE *pArq = fopen("series.dat", "r+w");

	fseek(pArq, primario->itens[item].RRN, SEEK_SET);
	putc('*', pArq);
	putc('|', pArq);
	fclose(pArq);
	if (removerIndiceSecundario(titulo, primario->itens[item].cod) == -1)
		return -1;
	imprimir2(titulo);
	if (removerIndiceSecundario(autor, primario->itens[item].cod) == -1)
		return -1;
	if (removerIndicePrimario(primario, primario->itens[item].cod) == -1)
		return -1;
	return 1;
}

/* ---------------------------------------------------------------------------------------------------/
/										Alterar nota												  /
/ Funcionamento:																					  /
/    Primeiramente busca-se no arquivo de índices a série que se deseja alterar a nota. Se ela for en-/
/ contrada, apontamos o ponteiro de arquivo para o RRN do ítem que terá a nota alterada. Percorremos  /
/ então o arquivo de dados até achar a posição à qual a nota ocupa e alteramos o seu valor.			  /
/ Retornos:																							  /
/    -1 -> Erro de leitura de arquivo 																  /
/	 0  -> Serie não encontrada 																	  /
/    1  -> Nota alterada com sucesso 																  /
/----------------------------------------------------------------------------------------------------*/
int  alterarNota(IndicePrimario *indice, char *chave){
	int pos, cont = 0, nota;
	char buffer;
	// Buscar a serie no arquivo de índices
	if ((pos = buscarIndice(indice, chave)) != -1){
		FILE *pArq = fopen("series.dat", "r+w");
		
		// Laço para entrada de uma nota válida
		do{
			printf ("Nova nota: ");
			scanf  ("%d", &nota);
			if (nota < 1 || nota > 5)
				printf ("Nota inválida!\n");
		} while (nota < 1 || nota > 5);

		// Apontar para a posição inicial da série
		fseek(pArq, indice->itens[pos].RRN, SEEK_SET);
		
		// Percorrer arquivo de dados até chegar na posição da nota
		do{
			buffer = fgetc(pArq);
			if (buffer == '@')
				cont++;
		}while(cont < 11);
		
		// Conversão da nota para char e sobrescrita no arquivo
		putc(nota + '0', pArq);
		fclose(pArq);
		return 1;
	}
	return 0;
}


/* ---------------------------------------------------------------------------------------------------/
/										Buscar Serie												  /
/ Funcionamento:																					  /
/    Primeiramente verifica-se o modo de busca. Caso seja pela chave primaria, busca-se a chave no in-/
/ dice primário. Caso encontre, aponta o ponteiro de arquivos para o RRN encontrado, carrega a serie  /
/ e imprime-a.																						  /
/    Se o modo escolhido for pelo titulo ou pelo diretor, buscamos o registro no arquivo de indice se-/
/ cundário correspondente. Se for encontrado algo, procuramos pela sua primeira ocorrência, no caso   /
/ haver mais de um registro com o mesmo diretor/titulo. Então, para cada ocorrência, buscamos novamen-/
/ te no indice primário, carregamos e imprimimos todas as séries com a chave selecionada.			  /
/ Retornos:																							  /
/    -2 -> Modo de busca inválido 																	  /
/    -1 -> Erro de leitura de arquivo 																  /
/	 0  -> Serie não encontrada 																	  /
/    1  -> Serie impressa com sucesso 																  /
/ Modos:																							  /
/    1 -> Chave primária																			  /
/    2 -> Título 																					  /
/    3 -> Diretor 																					  /
/----------------------------------------------------------------------------------------------------*/
int  buscarSerie(char *chave, int modo, IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor){
	Serie serie;
	FILE *pArq;
	int posPri;
	int *indices, qtd, i, j;

	switch(modo){
		//Busca pela chave primária
		case 1:
			// Busca no arquivo de índices primários
			if ((posPri = buscarIndice(primario, chave)) == -1){
				return 0;
			}

			pArq = fopen("series.dat", "r");

			// Apontar para o RRN da série procurada
			fseek(pArq, primario->itens[posPri].RRN, SEEK_SET);
			
			// Carregar dados da série
			serie = carregarSerie(pArq);

			fclose(pArq);

			// Imprimir Serie
			imprimirSerie(serie);
			return 1;
			break;

		// Busca pela chave secundária Titulo
		case 2:
			// Busca no arquivo de índices secundários
			indices = (int *) malloc (sizeof(int) * titulo->quantidade + 1);
			buscarIndiceSecundarioRelativo(titulo, chave, indices, &qtd);
			if (qtd == 0){
				return 0;
			}
			j = 0;
			i = 0;
			// Laço para imprimir as diversas ocorrências
			while (j < qtd){
				printf ("\n");
				// Busca no índice primário
				if ((posPri = buscarIndice(primario, titulo->itens[indices[i]].cod)) == -1){
					imprimir(primario);
					printf("%d\n", primario->quantidade);
					return 0;
				}
				pArq = fopen("series.dat", "r");
				
				// Apontar para o RRN da série procurada
				fseek(pArq, primario->itens[posPri].RRN, SEEK_SET);
				
				// Carregar série
				serie = carregarSerie(pArq);
	
				// Imprimir série
				imprimirSerie(serie);

				i++;
				j++;
			}
			fclose (pArq);
			free(indices);
			return 1;
			break;

		// Busca pela chave secundária Diretor
		case 3:
			// Busca no arquivo de índices secundários
			indices = (int *) malloc (sizeof(int) * diretor->quantidade);
			buscarIndiceSecundarioRelativo(diretor, chave, indices, &qtd);
			if (qtd == 0){
				return 0;
			}
			j = 0;
			i = 0;
			// Laço para imprimir as diversas ocorrências
			while (j < qtd){
				printf ("\n");
				// Busca no índice primário
				if ((posPri = buscarIndice(primario, diretor->itens[indices[i]].cod)) == -1){
					return 0;
				}
				pArq = fopen("series.dat", "r");
				
				// Apontar para o RRN da śerie procurada
				fseek(pArq, primario->itens[posPri].RRN, SEEK_SET);
				
				// Carregar Série
				serie = carregarSerie(pArq);
			
				// Imprimir Série
				imprimirSerie(serie);

				i++;
				j++;
			}
			fclose(pArq);
			free(indices);
			return 1;
			break;

		default:
			return -2;
	}
}

/* ---------------------------------------------------------------------------------------------------/
/										Listar Serie												  /
/ Funcionamento:																					  /
/    Para cada série, é chamada a função buscarSerie, que imprime os dados formatados de uma série	  /
/ Retornos:																							  /
/    -1 -> Arquivo vazio 			 																  /
/	 0  -> Modo de busca inválido  																	  /
/    1  -> Listagem com sucesso 	 																  /
/ Modos:																							  /
/    1 -> Chave primária																			  /
/    2 -> Título 																					  /
/    3 -> Diretor 																					  /
/----------------------------------------------------------------------------------------------------*/
int  listarSerie(int modo, IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor){
	int i;
	for (i=0; i < primario->quantidade; i++){
		if (modo == 1)
			buscarSerie(primario->itens[i].cod, modo, primario, titulo, diretor);
		else if (modo == 2)
			printf ("\n--%d--\n", buscarSerie(titulo->itens[i].chave, modo, primario, titulo, diretor));
		else if (modo == 3)
			buscarSerie(diretor->itens[i].chave, modo, primario, titulo, diretor);
		else
			return 0;
		printf ("\n");
	}
	if (i == 0)
		return -1;
	return 1;
}

/* ---------------------------------------------------------------------------------------------------/
/										Liberar Espaço												  /
/ Funcionamento:																					  /
/    Cria-se um arquivo temporário. Então percorremos o arquivo de dados e transferimos as informa-   /
/ ções das séries sem o marcador de remoção para o arquivo temporário. Ao encontrar uma série com mar-/
/ cador de remoção, pulamos para a próxima série do arquivo de dados. Ao terminar de analisar todas   /
/ as séries do arquivo de dados, o arquivo temporário é renomeado para "series.dat", sobrescrevendo o /
/ antigo arquivo de dados de séries.																  /
/ Retornos:																							  /
/	 0  -> Erro de leitura de arquivo																  /
/    1  -> Espaço liberado com sucesso 																  /
/----------------------------------------------------------------------------------------------------*/
int  liberarEspaco(IndicePrimario *primario, IndiceSecundario *titulo, IndiceSecundario *diretor){
	char buffer[2];
	int cont;

	FILE *pArq = fopen("series.dat", "r");
	FILE *pNovo = fopen("temp.tmp", "w");

	buffer[0] = fgetc(pArq);
	while (buffer[0] != EOF){
		// Verificação do marcador de remoção
		if (buffer[0] != '*'){
			if ((buffer[1] = fgetc(pArq)) != '|'){
				// Se não houver marcador, os dados são copiados para o arquivo temporário
				fputc(buffer[0], pNovo);
				fputc(buffer[1], pNovo);
				for (cont = 2; cont < 1024; cont++)
					fputc(fgetc(pArq), pNovo);
			} else 
				// Se houver marcador, pulamos para a próxima série
				fseek(pArq, 1022, SEEK_CUR);
		} else
			fseek(pArq, 1023, SEEK_CUR);

		// Leitura do caractere para verificação de final de arquivo.
		buffer[0] = fgetc(pArq);
	}
	fclose(pArq);
	fclose(pNovo);

	//Renomear arquivo temporário
	rename("temp.tmp", "series.dat");

	//Criação dos arquivos de índice
	criarIndice(primario);
	criarIndiceTitulo(titulo);
	criarIndiceDiretor(diretor);

	return 1;
}


/* ---------------------------------------------------------------------------------------------------/
/										Rotinas Arvore-B											  /
/----------------------------------------------------------------------------------------------------*/
int  criarNo(NoPrimario *pnovo){
	pnovo->itens = (ItemPrimario *) malloc (sizeof(ItemPrimario) * ordem - 1);
	if (pnovo == NULL)
		return 0;
	int i;
	for (i=0; i<ordem-1; i++){
		strcpy(pnovo->itens[i].cod," ");
	}

	pnovo->filhos = (NoPrimario **) malloc (sizeof(NoPrimario *) * ordem);
	if (pnovo->filhos == NULL){
		printf ("?");
		return 0;
	}
	for (i=0; i<ordem; i++){
		pnovo->filhos[i] = NULL;
	}
	pnovo->qtdChaves = 0;
	pnovo->folha = 1;
	printf ("Criei novo nó!\n");
	return 1;
}

int  criarArvoreB(NoPrimario *raiz){
	NoPrimario *pnovo = (NoPrimario *) malloc (sizeof(NoPrimario));
	if (!criarNo(pnovo))
		return 0;
	pnovo->qtdChaves = 0;
	pnovo->folha = 1;

	*raiz = *pnovo;
	
	return 1;
}

int  buscaArvoreB(NoPrimario *node, char *cod, ItemPrimario *aux, int *indice){
	int i;
	i = 0;
	while (i < node->qtdChaves && strcmp(cod,node->itens[i].cod) > 0){
		i++;
	}
	
	if (i < node->qtdChaves && strcmp(cod, node->itens[i].cod) == 0){
		*indice = 1;
		aux = &(node->itens[i]);
		return 1;
	}

	if (node->folha == 1){
		return 0;
	} else {
		return buscaArvoreB(node->filhos[i], cod, aux, indice);
	}
}

int  splitArvoreB(NoPrimario *x, int i, NoPrimario *y){
	int j;
	NoPrimario *z = (NoPrimario *) malloc (sizeof(NoPrimario));
	if (!criarNo(z))
		return 0;

	int t = (ordem - 1) / 2;
	t++;

	z->folha = y->folha;
	z->qtdChaves = t - 1;
	for (j=0; j <= t-2; j++){
		z->itens[j] = y->itens[j+t];
	}

	if (y->folha == 0){
		for (j=0; j <= t-1; j++){
			z->filhos[j] = z->filhos[j+t];
		}
	}

	y->qtdChaves = t - 1;
	for (j = x->qtdChaves; j >= i+1; j--){
		x->filhos[j+1] = x->filhos[j];
	}
	x->filhos[j+1] = z;

	for (j = x->qtdChaves - 1; j >= i; j--){
		x->itens[j+1] = x->itens[j];
	}
	x->itens[j] = y->itens[t];
	x->qtdChaves += 1;

	return 1;
}

int inserirNoNaoCheio(NoPrimario *x, ItemPrimario item){
	int i = x->qtdChaves - 1;
	if (x->folha == 1){
			printf ("teste\n");
		while (i >= 0 && strcmp(item.cod, x->itens[i].cod) < 0){
			x->itens[i+1] = x->itens[i];
			i--;
		}
		x->itens[i+1] = item;
		x->qtdChaves += 1;
		return 1;
	} else {
		while (i >= 0 && strcmp(item.cod, x->itens[i].cod) < 0){
			i--;
		}
		i++;
		if (x->filhos[i]->qtdChaves == ordem - 1){
			if (!splitArvoreB(x, i, x->filhos[i]))
				return 0;

			if (strcmp(item.cod, x->itens[i].cod))
				i++;
		}
		return inserirNoNaoCheio(x->filhos[i], item);
	}
}

int  inserirArvoreB(NoPrimario *raiz, ItemPrimario item){
	NoPrimario *r = (NoPrimario*) malloc (sizeof(NoPrimario));
	r = raiz;
	printf ("%d\n", r->folha);
	if (r->qtdChaves == ordem -1){
		NoPrimario *s = (NoPrimario *) malloc (sizeof(NoPrimario));
		if (!criarNo(s))
			return 0;
		raiz = s;
		s->folha = 0;
		s->qtdChaves = 0;
		s->filhos[0] = r;
		splitArvoreB(s, 0, r);
		inserirNoNaoCheio(s, item);
		printf ("fim\n");
		imprimeB(raiz, 1);
	} else{
		inserirNoNaoCheio(r, item);
		//*raiz = *r;
	}
	printf ("Fim da função\n");
	imprimeB(raiz, 1);
	return 1;
}

/* ---------------------------------------------------------------------------------------------------/
/										Rotinas Auxiliares											  /
/----------------------------------------------------------------------------------------------------*/


void buildMenu(int *op){
 	printf ("\n");
 	printf ("Opcoes: \n");
 	printf (" 1. Cadastrar nova serie\n");
 	printf (" 2. Alterar nota de uma serie\n");
 	printf (" 3. Remover uma serie\n");
 	printf (" 4. Buscar series\n");
 	printf (" 5. Listagens\n");
 	printf (" 6. Liberar espaco do arquivo\n");
 	printf (" 7. Sair\n");
 	printf ("\n");
 	printf ("> ");
 	scanf (" %d", op);
}

void buildMenuBusca(int *op){
 	printf ("\t1. Busca por codigo\n");
 	printf ("\t2. Busca por titulo\n");
 	printf ("\t3. Busca por diretor\n");
 	printf ("\n");
 	printf ("\t> ");
 	scanf  (" %d", op);
}

void buildMenuLista(int *op){
 	printf ("\t1. Listar Árvore-B\n");
 	printf ("\t2. Listar ordenado por titulo\n");
 	printf ("\t3. Listar ordenado por diretor\n");
 	printf ("\n");
 	printf ("\t> ");
 	scanf  (" %d", op);
}

int  isInt(char *string){
	int i;
	for (i=0; i < strlen(string); i++){
		if (string[i] < '0' || string[i] > '9')
			return 0;
	}
	return 1;
}

void maiusculo(char *string){
	int i;
	for (i=0; i < strlen(string); i++){
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] -= 32;
	}
}
