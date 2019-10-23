#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <pthread.h>

typedef struct dados{
    float ** valores;
    int** classe;
}dados;

typedef struct neuronio{

    float ** entradas;      // Ponteiros para guardar os enderecos dos valores de entrada
    float * pesos;          // Pesos de cada entrada do neuronio
    float bias;             // Bias, valor de limiar, igual a B na equacao Y = aX + b
    float p_bias;           // Peso para ser atualizado dos bias, assim ele pode ser melhorado
    float saida;            // Saida do neuronio, que serar entrada de outro neuronio, por isso cada entrada armazena ponteiros
}neuronio;

typedef struct  rede_neural{
    neuronio*** neuro;      // Matriz de neuronios, Bidimensional e dinaminaca, onde cada camada pode ter valores de neuronios variados, armazenando um ponteiro que é o neuronio
    int n_camadas;          // Numero de cadas na rede
    int* neuronios_por_camada;// Numero de Neuronios existente em cada camada
}rede_neural;

neuronio* Criar_Neuronio(int n_entradas){
    neuronio * neuro;
    neuro                       = (neuronio*)malloc(sizeof(neuronio));
    neuro->entradas             = (float**)malloc(n_entradas*sizeof(float*));
    neuro->pesos                = (float*)malloc(n_entradas*sizeof(float));
    for( int i = 0; i < n_entradas ; i++){
        neuro->entradas[i]     = NULL;
        neuro->pesos[i]         = rand()*0.00001;
        //printf("Entrada[%d] Pesos:%f\n",i,neuro->pesos[i]);
    }
    neuro->bias                 = rand()*0.00001;
    neuro->p_bias               = rand()*0.00001;
    neuro->saida                = 0;
    //printf("Bias:%f Pesos Bias:%f\n",neuro->bias,neuro->p_bias);
    return neuro;
}

rede_neural* Criar_Rede(int n_camadas){
    rede_neural* rede;
    rede                        = (rede_neural*)malloc(sizeof(rede_neural));
    rede->neuro                 = (neuronio***)malloc(n_camadas*sizeof(neuronio**));
    rede->n_camadas             = n_camadas;
    rede->neuronios_por_camada  = (int*)malloc(n_camadas*sizeof(int));
    return rede;
}

void Adicionar_Camada(rede_neural* rede,int n_entradas, int camada, int n_neuronios_na_camada){
    if( camada > rede->n_camadas || camada < 0 ){
        printf("\t\tCamada Invalida!!!\n");
    }else{
    //          Alocacao dinamica dos neuronios por camada
        rede->neuro[camada]                 = (neuronio**)malloc(n_neuronios_na_camada*sizeof(neuronio*));
        for( int id_neuro = 0 ; id_neuro < n_neuronios_na_camada ; id_neuro++ ){
    //          Alocacao dinamica das entradas de cada neuronio na camada especificada
            printf("\tCamada[%d] Neuronio[%d]\n",camada,id_neuro);     
            rede->neuro[camada][id_neuro]   = Criar_Neuronio(n_entradas);
        }
        rede->neuronios_por_camada[camada]  = n_neuronios_na_camada;
    }
}

void Conectar_Rede(rede_neural* rede, float* entradas){
    for( int i = 0 ; i < rede->neuronios_por_camada[0] ; i++){
        for( int j = 0 ; j < rede->neuronios_por_camada[0] ; j++ ){
    //          Adicionando entradas na camada de entrada
    //          Todos os neuronio da camada de entrada recebem os valores da entrada, rede totalmente conectada
            rede->neuro[0][i]->entradas[j] = &entradas[j];
            //printf("%f-",*rede->neuro[0][i]->entradas[j]);
        }
    }
    //          Primeiro laco para percorrer as camadas da rede
    for( int camada = 1 ; camada < rede->n_camadas ; camada++){
    //          Segundo laco para percorrer cada neuronio da camada
        for( int id_neuro = 0 ; id_neuro < rede->neuronios_por_camada[camada] ; id_neuro++ ){
    //          Terceiro laco para percorrer cada entrada dos neuronios 
    //          Cada neuronio da camada lida tem entradas iguais ao numero de neuronio da camanda anterior
            for( int id_entadas =  0 ; id_entadas < rede->neuronios_por_camada[camada-1] ;id_entadas++ ){
    //          Cada entada do neuronio deve guardar o endereco da saida do neuronio da camada anterior correspondente
                rede->neuro[camada][id_neuro]->entradas[id_entadas] = &rede->neuro[camada - 1][id_entadas]->saida;
            }
        }
    }
}

dados* Abrir_Arquivo(char localizacao[], char tipo[], int n_entradas, int n_linhas,int n_classes){
    dados *d;
    FILE *arq;
    int j = 0;
    d           = (dados*)malloc(sizeof(dados));
    d->valores  = (float**)malloc(n_linhas*sizeof(float*));
    d->classe   = (int**)malloc(n_linhas*sizeof(int*));
    for( int i = 0 ; i < n_linhas; i++){
        d->valores[i] = (float*)malloc(n_entradas*sizeof(float));
        d->classe[i]  = (int*)malloc(n_classes*sizeof(int));
    }
    arq = fopen(localizacao, tipo);
    if (arq == NULL) {
        printf("Problemas na abertura do arquivo\n");
        return NULL;
    }else{    
        while (fscanf(arq,"%f,%f,%f,%f,%d,%d,%d",&d->valores[j][0],&d->valores[j][1],&d->valores[j][2],&d->valores[j][3],&d->classe[j][0],&d->classe[j][1],&d->classe[j][2]) != EOF && j < n_linhas){
            //printf("%f, %f, %f, %f - %d %d %d\n",d->valores[j][0],d->valores[j][1],d->valores[j][2],d->valores[j][3],d->classe[j][0],d->classe[j][1],d->classe[j][2]);
            j++;
        }
        fclose(arq);
        //printf("FIM\n");
    }
    return d;
}

void Printar_Rede( rede_neural* rede){
    printf("\n");
    for(  int k = 0 ; k < rede->n_camadas ; k++  ){
        printf("\t\tCamada:%d\n",k);
        for( int i = 0 ; i < rede->neuronios_por_camada[k] ; i++){
            printf("Neuronio:%d\n",i);
            for( int j = 0 ; j < 4 ; j++ ){
                printf("\tEntrada:%.4f Peso:%.4f\n",*rede->neuro[k][i]->entradas[j],rede->neuro[k][i]->pesos[j]);
            }
            printf("Saida:%.4f\n",rede->neuro[k][i]->saida);
        } 
    }
}

float Ativacao(float x){
    x = 1/(1+ pow(2.71828182,-x));
    return x;
}

void Calcular_Saidas( rede_neural* rede){
    float saida = 0;
    for(  int k = 0 ; k < rede->n_camadas ; k++  ){
        for( int i = 0 ; i < rede->neuronios_por_camada[k] ; i++){
            for( int j = 0 ; j < rede->neuronios_por_camada[k] ; j++ ){
    //          Saida = Soma( entradas*pesos )
                saida = saida +  ((*rede->neuro[k][i]->entradas[j]) * (rede->neuro[k][i]->pesos[j]));
            }
    //          Saida = Soma( entradas*pesos ) + bias*Peso_bias;
            saida = saida + rede->neuro[k][i]->bias * rede->neuro[k][i]->p_bias;
            rede->neuro[k][i]->saida = Ativacao(saida);
        } 
    }
}

float Derivar_Sigmoid(float x){
    x = pow(2.71828182,-x)/pow(1+pow(2.71828182,-x),2);
    return x;
}

void Atualizar_Pesos( rede_neural* rede, int Dy[]){
    float soma_pesos = 0;
    //      ____________________________________Primeiro Caso_____________________________
    //      Calcular e atualizar primeiro os pesos dos neuronios da ultima camada
    //      Primeiro laco serve para percorrer os neuronio da ultima camada
    //      por isso n_camadas - 1, percorrer apenas a ultima camada  \/
    for( int i = 0 ; i < rede->neuronios_por_camada[ rede->n_camadas - 1 ]; i++){
    //      Segundo laco serve para percorrer as entradas dos neuronios,
    //      sendo que o numero de entradas e pesos depende do numero de
    //      entradas da camada anterior, por isso e n_camadas - 2    AQUI  \/ 
        for( int j = 0 ; j < rede->neuronios_por_camada[ rede->n_camadas - 2 ] ; j ++){
    //      Atulaizacao dos pesos dos neuronios da ultima camada
    //      peso_new = peso_old - 
    //      (derivada_erro_em_relacao_a_fucao_de_ativacao*
    //      derivada_da_fucao_ativacao_em_relacao_a_saida_do_neuronio*
    //      derivada_da_saida_em_relacao_ao_peso)
            rede->neuro[ rede->n_camadas - 1 ][i]->pesos[j] = 
            rede->neuro[ rede->n_camadas - 1 ][i]->pesos[j] -
            (Dy[i] - rede->neuro[ rede->n_camadas - 1 ][i]->saida)*
            (Derivar_Sigmoid(*rede->neuro[ rede->n_camadas - 1 ][i]->entradas[j]))*
            (*rede->neuro[ rede->n_camadas - 1 ][i]->entradas[j]);


        }
    }
    
    //      ____________________________________Segundo Caso______________________________
    //      Calcular restante dos outros pesos
    //      Primeiro laco percorre as camadas restantes
    for( int k = rede->n_camadas - 2 ; k >= 0 ; k--){
    //      Segundo laco percorre as entradas e pesos dos neuronios, pois dependem
    //      das camandas anteriores
        for( int i = 0 ; i < 4; i++){
    //      Terceiro laco percorre os neuronio da camada, atualizando seus pesos
            for( int j = 0 ; j < 4 ; j ++){
    //      Quarto laco soma os pesos dos neuronios da camada anterior, que ja foi calculada
                for( int l = 0 ; l < 4 ; l++){
                    soma_pesos = soma_pesos + rede->neuro[k][j]->pesos[l];
                }
                rede->neuro[ k ][i]->pesos[j] = 
                rede->neuro[ k ][i]->pesos[j] -
                (Dy[i] - rede->neuro[ k ][i]->saida)*
                (Derivar_Sigmoid(*rede->neuro[ k ][i]->entradas[j]))*
                (*rede->neuro[ k ][i]->entradas[j])*
                soma_pesos;
                //printf("Camada %d Neuronio %d entrada %d\n",k,i,j);  
            }
        }
    }
}

void main(){
    rede_neural * rede;
    dados* flores;

    rede = Criar_Rede(2);
    Adicionar_Camada(rede,4,0,4);
    Adicionar_Camada(rede,4,1,3);
    flores = Abrir_Arquivo("iris.txt","rt",4,150,3);
    Conectar_Rede(rede,flores->valores[0]);
    //Printar_Rede(rede);
    Calcular_Saidas(rede);
    Printar_Rede(rede);
    for(int i = 0 ; i < 150 ; i++){
        Atualizar_Pesos(rede,flores->classe[i]);
        Printar_Rede(rede);
    }
}