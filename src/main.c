/**************************************************
 NAO MUDE NADA DAQUI
 *************************************************/

#include<stdio.h>
#include<stdlib.h>
#include "resolvedor.h"


//Tres registros que serao uteis, Arcos, Vertices e Digrafos
typedef struct HOKAMA_arco{
	int ini, fim;
	struct HOKAMA_arco * prox;
}HOKAMA_Arco;

typedef struct HOKAMA_vertice{
	int id;
	HOKAMA_Arco * arcos_saida; 
	int cor;
}HOKAMA_Vertice;

typedef struct HOKAMA_digrafo{
	int n;
	int m;
	HOKAMA_Vertice * V;
}HOKAMA_Digrafo;


void HOKAMA_seta_cor(HOKAMA_Digrafo *D, int v, int cor){
  D->V[v].cor = cor;
}

HOKAMA_Digrafo * HOKAMA_criar_grafo(int n){
	HOKAMA_Digrafo * G = (HOKAMA_Digrafo *) malloc(sizeof(HOKAMA_Digrafo));
	G->V = (HOKAMA_Vertice *) malloc(n * sizeof(HOKAMA_Vertice));
	G->n = n;
	G->m = 0;
	for(int u = 0; u < n; u++){
		G->V[u].id = u;
		G->V[u].arcos_saida = NULL;
	}
	return G;
}

HOKAMA_Arco * HOKAMA_criar_arco(int ini, int fim){
	HOKAMA_Arco * A = (HOKAMA_Arco *) malloc(sizeof(HOKAMA_Arco));
	A->ini = ini;
	A->fim = fim;
	A->prox = NULL;
  return A;
}

void HOKAMA_adiciona_arco(HOKAMA_Digrafo * G, int ini, int fim){
	G->m++;
	HOKAMA_Arco * saida = HOKAMA_criar_arco(ini, fim);
	saida->prox = G->V[ini].arcos_saida;
	G->V[ini].arcos_saida = saida;
	return;
}

//funcao para imprimir um digrafo no formato GraphViz
// voce pode usar uma ferramente online como o https://dreampuf.github.io/GraphvizOnline/
// para desenha seu grafo nesse formato (soh para grafos pequenos)
void HOKAMA_imprime_digrafo(HOKAMA_Digrafo * G){
  char * cores[] = {"red", "orange", "purple", "yellow", "green", "cyan"};
	printf("digraph G{\n");
	for(int i = 0; i < G->n; i++) {
		printf("  %d[style=filled,color=%s];\n", i, cores[G->V[i].cor]);
	}
	for(int i = 0; i < G->n; i++){
		//u eh o i-esimo vertice
		HOKAMA_Vertice u = G->V[i]; 
		for(HOKAMA_Arco * a = u.arcos_saida; a != NULL; a = a->prox){
			printf("  %d -> %d;\n", a->ini,  a->fim);
		}
	}	
	printf("}\n");
}

//funcoes para liberar a memoria do digrafo
void HOKAMA_libera_lista_arcos(HOKAMA_Arco * lista){
	if(lista == NULL) return;
	HOKAMA_libera_lista_arcos(lista->prox);
	free(lista);
}

void HOKAMA_libera_digrafo(HOKAMA_Digrafo * G){
	for(int i = 0; i < G->n; i++){
		HOKAMA_libera_lista_arcos(G->V[i].arcos_saida);
	}
	free(G->V);
	free(G);
}


void HOKAMA_muda_cor(HOKAMA_Digrafo * D, int id, int cor){
  if(D->V[id].cor == cor) return;
  //printf("mudando a cor do vertice %d para %d\n", id, cor);
  int cor_antiga = D->V[id].cor;
  //mudando a cor antes de propagar (pode evitar um loop infinito)
  D->V[id].cor = cor;
  for(HOKAMA_Arco * a = D->V[id].arcos_saida; a != NULL; a = a->prox){
    int vizinho = a->fim;
    if(D->V[vizinho].cor == cor_antiga){
      HOKAMA_muda_cor(D, vizinho, cor);
    }
  }
  return;
}


int HOKAMA_verifica_fim(HOKAMA_Digrafo * D){
  int cor_inicial = D->V[0].cor;
  for(int v = 1; v < D->n; v++){
    if(cor_inicial != D->V[v].cor) return 0;
    //printf("Cor de %d eh igual ao 0\n", v);
  }
  return 1;
}


int HOKAMA_verifica_movimentos(HOKAMA_Digrafo * D, int num_movimentos, int * movimentos){
  //executa cada movimento
  for(int i = 0; i < num_movimentos; i++){
    //Naum pode mudar para a mesma cor
    if(D->V[0].cor != movimentos[i]){
      HOKAMA_muda_cor(D, 0, movimentos[i]);
    }
  }
  if(HOKAMA_verifica_fim(D)) return 1;
  return 0;
}

int main(int argc, char* argv[]){
	char entrada_filename[30];
  char solucao_filename[30];
  
  scanf("%s", entrada_filename);
  scanf("%s", solucao_filename);
  

  resolvedor(entrada_filename, solucao_filename);

  //********VERIFICADOR DE SOLUCAO **********
  //** Lendo o grafo de entrada **
  FILE *entrada_file = fopen(entrada_filename, "r");

  if(entrada_file == NULL){
    printf("Erro ao abrir arquivo de entrada!\n");
  }
  int n, m;
  
  fscanf(entrada_file, "%d %d", &n, &m);
  	
	HOKAMA_Digrafo * D = HOKAMA_criar_grafo(n);

  for(int v = 0; v < n; v++){
		int cor;
		fscanf(entrada_file, "%d", &cor);
		HOKAMA_seta_cor(D, v, cor);
	}

	for(int e = 0; e < m; e++){
		int ini, fim;
		fscanf(entrada_file, "%d %d", &ini, &fim);
		HOKAMA_adiciona_arco(D, ini, fim);
	}
  //***************************
  
  //** Lendo arquivo de solucao criado pela sua funcao **
  FILE *solucao_file = fopen(solucao_filename, "r");
  if(solucao_file == NULL){
    printf("Erro ao abrir arquivo de solucao!\n");
  }
  
  //UM UPPER BOUND VALIDO
  int num_movimentos;
  fscanf(solucao_file, "%d", &num_movimentos);
  int * movimentos = (int*) malloc(sizeof(int)*num_movimentos);
  
  for(int i = 0; i < num_movimentos; i++){
    fscanf(solucao_file, "%d", &(movimentos[i]));
  }
  //*******************************
  
  //** verifica se sua solucao eh valida **
 
  if(HOKAMA_verifica_movimentos(D, num_movimentos, movimentos) == 1){
    printf("Sua solucao funciona!!\n");
  }else{
    printf("Errada!!\n");
  }

	HOKAMA_libera_digrafo(D);
	return 0;
}

