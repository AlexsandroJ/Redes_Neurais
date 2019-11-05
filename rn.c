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
    int* entradas_por_camada;
}rede_neural;

typedef struct dados_temp{
    float ** pesos_old;
    float * derivadas_parcial;
    float soma;
}dados_temp;


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
    rede->entradas_por_camada   = (int*)malloc(n_camadas*sizeof(int));
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
        rede->entradas_por_camada[camada]   = n_entradas;
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
        printf("\t\t\tCamada:%d\n",k);
        for( int i = 0 ; i < rede->neuronios_por_camada[k] ; i++){
            printf("\tNeuronio:%d\n",i);
            for( int j = 0 ; j < rede->entradas_por_camada[k] ; j++ ){
                printf("\t\tEntrada:%.4f Peso:%.4f\n",*rede->neuro[k][i]->entradas[j],rede->neuro[k][i]->pesos[j]);
            }
            printf("\tBias:%.4f Peso:%.4f\n",rede->neuro[k][i]->bias,rede->neuro[k][i]->p_bias);
            printf("Saida:%.4f\n",rede->neuro[k][i]->saida);
        } 
    }
}

float Ativacao(float x){
    x = 1/(1+ exp(-x));
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
    //          Todas as entradas de uma camada tem seus enderecos salvos como ponteiros nas entradas dos
    //          Neuronios da proxima camada
            rede->neuro[k][i]->saida = Ativacao(saida);
        } 
    }
}

float Derivar_Sigmoid(float x){
    x = pow(2.71828182,-x)/pow(1+exp(-x),2);
    return x;
}

dados_temp* Alocar_Memoria_Pesos_old_temp(rede_neural* rede){
    // Salvar temporariamente os pesos dos neuronio da camada
    // Aloca a quantidade de neuronios na camada
    dados_temp* temp;
    temp                    = (dados_temp*)malloc(sizeof(dados_temp));
    temp->pesos_old         = (float**)malloc(10*sizeof(float*));
    temp->derivadas_parcial = (float*)malloc(10*sizeof(float));
    temp->soma              = 0;
    for( int i = 0 ; i < 10; i++){
    // Aloca a quantidade entradas para cada neuronio
        temp->pesos_old[i] = (float*)malloc(10*sizeof(float));
    }
    return temp;
}


dados_temp* Calcular_Erro( rede_neural* rede, int Dy[], float taxa){
    //      Alocar memoria para salvar os pesos antigos
    dados_temp* temp;
    temp = Alocar_Memoria_Pesos_old_temp(rede );
    float soma_pesos = 0;
    //      ____________________________________Primeiro Caso_____________________________
    //      Calcular e atualizar primeiro os pesos dos neuronios da ultima camada
    //      Primeiro laco serve para percorrer os neuronio da ultima camada
    //      por isso n_camadas - 1, percorrer apenas a ultima camada  \/
    for( int i = 0 ; i < rede->neuronios_por_camada[ rede->n_camadas - 1 ]; i++){
    //      Calculo das Derivadas Parciais
        temp->derivadas_parcial[i] = 
        taxa*
        (Dy[i] + rede->neuro[ rede->n_camadas - 1 ][i]->saida)*
        (Derivar_Sigmoid(rede->neuro[ rede->n_camadas - 1 ][i]->saida));
        printf("Neuronio:%d Erro: %.4f Saida:%.4f\n",i,Dy[i] - rede->neuro[ rede->n_camadas - 1 ][i]->saida,rede->neuro[ rede->n_camadas - 1 ][i]->saida);
    //      Calculo do novo peso do bias
        rede->neuro[ rede->n_camadas - 1][i]->p_bias = temp->derivadas_parcial[i]*rede->neuro[ rede->n_camadas - 1][i]->bias;
    //      Segundo laco serve para percorrer as entradas dos neuronios,
    //      sendo que o numero de entradas e pesos depende do numero de
    //      entradas da camada anterior, por isso e n_camadas - 2    AQUI  \/ 
        for( int j = 0 ; j < rede->neuronios_por_camada[ rede->n_camadas - 2 ] ; j ++){
    //      Salvar valores dos pessos antigos
            temp->pesos_old[i][j] = rede->neuro[ rede->n_camadas - 1 ][i]->pesos[j];
    //      Atulaizacao dos pesos dos neuronios da ultima camada
    //      peso_new = peso_old - 
    //      (derivada_erro_em_relacao_a_fucao_de_ativacao*
    //      derivada_da_fucao_ativacao_em_relacao_a_saida_do_neuronio*
    //      derivada_da_saida_em_relacao_ao_peso)
            rede->neuro[ rede->n_camadas - 1 ][i]->pesos[j] = 
            rede->neuro[ rede->n_camadas - 1 ][i]->pesos[j] -
            temp->derivadas_parcial[i]*
            (*rede->neuro[ rede->n_camadas - 1 ][i]->entradas[j]);
        }
    }
    // retorna os pesos antigos
    return temp;
}
void Atualizar_Pesos( rede_neural* rede,int Dy[], float taxa){
    // Receber os pesos antigos da camada mais externa
    dados_temp*  pesos_old_temp;
    float        soma_pesos_old = 0;
    pesos_old_temp = Calcular_Erro(rede, Dy,taxa);
    
    // Percorre as camadas
    for( int k = rede->n_camadas -2; k >= 0 ; k--){
    // Percorre os neuronios de cada camada
       for( int i = 0 ; i < rede->neuronios_por_camada[k] ; i++){
    // Percorre os pesos de cada entrada, de cada neuronio da camada
            for(int j = 0 ; j < rede->entradas_por_camada[k] ; j++){
    // Percorre e soma os pesos da camada anterior ligados os neronio
                for( int v = 0 ; v < rede->entradas_por_camada[k + 1] ; v++){
                    soma_pesos_old += pesos_old_temp->pesos_old[v][j];
                }
    //  Salvar os pesos antigos
                pesos_old_temp->pesos_old[i][j] = rede->neuro[ k ][i]->pesos[j];
    // Atualização dos pesos
                rede->neuro[ k ][i]->pesos[j] = 
                rede->neuro[ k ][i]->pesos[j] -
                (*rede->neuro[ k ][i]->entradas[j])*
                soma_pesos_old;
            }
    // Atualizacao dos pesos do bias
            rede->neuro[ k ][i]->p_bias = rede->neuro[ k ][i]->bias*soma_pesos_old;
        }
    }
}

void Atualizar_entradas(rede_neural* rede, float* entradas){
    for( int i = 0 ; i < rede->neuronios_por_camada[0] ; i++){
        for( int j = 0 ; j < rede->neuronios_por_camada[0] ; j++ ){
    //          Adicionando entradas na camada de entrada
            rede->neuro[0][i]->entradas[j] = &entradas[j];
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
    Calcular_Saidas(rede);
    Atualizar_Pesos(rede,flores->classe[0],0.1);
    Printar_Rede(rede);
        
    Atualizar_entradas(rede,flores->valores[1]);
    Calcular_Saidas(rede);
    Atualizar_Pesos(rede,flores->classe[1],0.1);
    Printar_Rede(rede);
        

    /*for( int i = 0 ; i < 3 ; i++){
        Printar_Rede(rede);
        Atualizar_Pesos(rede,flores->classe[i],0.1);
        Atualizar_entradas(rede,flores->valores[i]);
        Calcular_Saidas(rede);
    }*/
}