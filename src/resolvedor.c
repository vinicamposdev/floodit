/**************************************
Aqui voce pode mudar tudo, exceto a 
assinatura da funcao "resolvedor"
pode inclusive ignorar tudo se preferir
***************************************/

#include<stdio.h>
#include<stdlib.h>

//Tres registros que serao uteis, Arcos, Vertices e Digrafos
typedef struct arco{
	int ini, fim;
	struct arco * prox;
}Arco;

typedef struct vertice{
	int id;
	Arco * arcos_saida; 
	int cor;
}Vertice;

typedef struct digrafo{
	int n;
	int m;
	Vertice * V;
}Digrafo;


void seta_cor(Digrafo *D, int v, int cor){
  D->V[v].cor = cor;
}

Digrafo * criar_grafo(int n){
	Digrafo * G = (Digrafo *) malloc(sizeof(Digrafo));
	G->V = (Vertice *) malloc(n * sizeof(Vertice));
	G->n = n;
	G->m = 0;
	for(int u = 0; u < n; u++){
		G->V[u].id = u;
		G->V[u].arcos_saida = NULL;
	}
	return G;
}

Arco * criar_arco(int ini, int fim){
	Arco * A = (Arco *) malloc(sizeof(Arco));
	A->ini = ini;
	A->fim = fim;
	A->prox = NULL;
  return A;
}

void adiciona_arco(Digrafo * G, int ini, int fim){
	G->m++;
	Arco * saida = criar_arco(ini, fim);
	saida->prox = G->V[ini].arcos_saida;
	G->V[ini].arcos_saida = saida;
	return;
}

//funcao para imprimir um digrafo no formato GraphViz
// voce pode usar uma ferramente online como o https://dreampuf.github.io/GraphvizOnline/
// para desenha seu grafo nesse formato (soh para grafos pequenos)
void imprime_digrafo(Digrafo * G){
  char * cores[] = {"red", "orange", "purple", "yellow", "green", "cyan"};
	printf("digraph G{\n");
	for(int i = 0; i < G->n; i++) {
		printf("  %d[style=filled,color=%s];\n", i, cores[G->V[i].cor]);
	}
	for(int i = 0; i < G->n; i++){
		//u eh o i-esimo vertice
		Vertice u = G->V[i]; 
		for(Arco * a = u.arcos_saida; a != NULL; a = a->prox){
			printf("  %d -> %d;\n", a->ini,  a->fim);
		}
	}	
	printf("}\n");
}

//funcoes para liberar a memoria do digrafo
void libera_lista_arcos(Arco * lista){
	if(lista == NULL) return;
	libera_lista_arcos(lista->prox);
	free(lista);
}

void libera_digrafo(Digrafo * G){
	for(int i = 0; i < G->n; i++){
		libera_lista_arcos(G->V[i].arcos_saida);
	}
	free(G->V);
	free(G);
}

void muda_cor(Digrafo * D, int id, int cor){
  //Jah esta dessa cor nada a fazer
  if(D->V[id].cor == cor) return;
  
  int cor_antiga = D->V[id].cor;
  //mudando a cor antes de propagar (pode evitar um loop infinito)
  D->V[id].cor = cor;
  for(Arco * a = D->V[id].arcos_saida; a != NULL; a = a->prox){
    int vizinho = a->fim;
    if(D->V[vizinho].cor == cor_antiga){
      muda_cor(D, vizinho, cor);
    }
  }
  return;
}


int verifica_fim(Digrafo * D){
  int cor_inicial = D->V[0].cor;
  for(int v = 1; v < D->n; v++){
    if(cor_inicial != D->V[v].cor) return 0;
  }
  return 1;
}




//** MEU SOLVEDOR IDIOTA
void resolvedor(char* entrada_filename, char* solucao_filename){

	FILE *entrada_file = fopen(entrada_filename, "r");
  if(entrada_file == NULL){
    printf("Meu erro ao abrir arquivo de entrada!\n");
  }
  
  // Lendo entrada
  int n, m;
  fscanf(entrada_file, "%d %d", &n, &m);  	
	Digrafo * D = criar_grafo(n);

  for(int v = 0; v < n; v++){
		int cor;
		fscanf(entrada_file, "%d", &cor);
		seta_cor(D, v, cor);
	}

	for(int e = 0; e < m; e++){
		int ini, fim;
		fscanf(entrada_file, "%d %d", &ini, &fim);
		adiciona_arco(D, ini, fim);
	}
	fclose(entrada_file);
  //***************************
  
  
 
  // *** Resolvendo de fato (de maneira tosca) **
  //vou criar um vetor para guardar os movimentos
  //estou chutando um numero enorme de n*n*6
  //espero usar menos
  int max_movimentos = n*n*6;
  int * movimentos = (int*) malloc(sizeof(int)*max_movimentos);
  int num_movimentos = 0;  
  
  
  //Enquanto naum deixar todos os nos da mesma cor.
  while(verifica_fim(D) != 1){
    //sorteia uma cor aleatoria
    movimentos[num_movimentos] = rand() % 6;
    //muda a cor o vertice zero com a cor sorteada;
    muda_cor(D, 0, movimentos[num_movimentos]);
    num_movimentos++;
  }
  
  libera_digrafo(D);
  
  
  
  //*** Escrevendo arquivo de solucao ***
  FILE *solucao_file = fopen(solucao_filename, "w");
  if(solucao_file == NULL){
    printf("Meu erro ao abrir arquivo de solucao!\n");
  }

  fprintf(solucao_file, "%d\n", num_movimentos);
  for(int i = 0; i < num_movimentos; i++){
    fprintf(solucao_file, "%d ", movimentos[i]);
  }
  fprintf(solucao_file, "\n");
  fclose(solucao_file);
  

  free(movimentos);	
	return;
}

