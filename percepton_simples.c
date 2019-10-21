#include <stdlib.h>
#include <stdio.h>
//#include <pthread.h>


typedef struct neuronio{
    float * entradas;
    float * pesos;
    float bias;
    float p_bias;
    float saida;
}neuronio;

neuronio* Iniciar_Neuronios(int n_entradas, float valor_inicial_bias){
    neuronio * neuro;
    neuro                   = (neuronio*)malloc(sizeof(neuronio));
    neuro->entradas         = (float*)malloc(n_entradas*sizeof(float));
    neuro->pesos            = (float*)malloc(n_entradas*sizeof(float));
    for( int i = 0; i < n_entradas ; i++){
        neuro->entradas[i]  = rand()*0.01;
        neuro->pesos[i]     = rand()*0.01;
        printf("Entrada[%d]:%f Pesos:%f\n",i,neuro->entradas[i],neuro->pesos[i]);
    }
    neuro->bias             = 5;
    neuro->p_bias           = rand()*0.01;
    printf("Bias:%f Pesos Bias:%f\n",neuro->bias,neuro->p_bias);
    return neuro;
}

void abrir_arquivo(char* localizacao, char* tipo){
    FILE *arq;
    char ler;
    arq = fopen(localizacao, tipo);
    if (arq == NULL) {
        printf("Problemas na abertura do arquivo\n");
        return;
    }
    while (fscanf(arq,"%c",&ler) != EOF){
        printf("%c",ler);
    }
    fclose(arq);
}

void main(){
    neuronio * neuro;
    neuro = Iniciar_Neuronios(3,0.5);
    abrir_arquivo("percepton_simples.c","rt");
    
}