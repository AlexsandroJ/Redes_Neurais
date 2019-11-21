#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <pthread.h>

typedef struct dados{
    float **        valores;
    float **        classe;
}dados;

typedef struct neuronio{

    float **        entradas;      // Ponteiros para guardar os enderecos dos valores de entrada
    float *         pesos;          // Pesos de cada entrada do neuronio
    float           bias;             // Bias, valor de limiar, igual a B na equacao Y = aX + b
    float           p_bias;           // Peso para ser atualizado dos bias, assim ele pode ser melhorado
    float           saida;            // Saida do neuronio, que serar entrada de outro neuronio, por isso cada entrada armazena ponteiros
    float           v;
}neuronio;

typedef struct  rede_neural{
    neuronio ***    neuro;      // Matriz de neuronios, Bidimensional e dinaminaca, onde cada camada pode ter valores de neuronios variados, armazenando um ponteiro que é o neuronio
    int             n_camadas;          // Numero de cadas na rede
    int *           neuronios_por_camada;// Numero de Neuronios existente em cada camada
    int *            entradas_por_camada;
}rede_neural;

typedef struct dados_temp{
    neuronio ***    neuro;
    float **        derivada;
    float **        pesos_old;
    float *         derivadas_parcial;
    float           soma;
    float           med;
}dados_temp;


neuronio* Criar_Neuronio(int n_entradas){
    neuronio * neuro;
    neuro                       = (neuronio*)malloc(sizeof(neuronio));
    neuro->entradas             = (float**)malloc(n_entradas*sizeof(float*));
    neuro->pesos                = (float*)malloc(n_entradas*sizeof(float));
    for( int i = 0; i < n_entradas ; i++){
        neuro->entradas[i]     = NULL;
        neuro->pesos[i]         = rand()*0.0000001;
        //printf("Entrada[%d] Pesos:%f\n",i,neuro->pesos[i]);
    }
    neuro->bias                 = 1;
    neuro->p_bias               = rand()*0.0000001;
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
    d->classe   = (float**)malloc(n_linhas*sizeof(float*));
    for( int i = 0 ; i < n_linhas; i++){
        d->valores[i] = (float*)malloc(n_entradas*sizeof(float));
        d->classe[i]  = (float*)malloc(n_classes*sizeof(float));
    }
    arq = fopen(localizacao, tipo);
    if (arq == NULL) {
        printf("Problemas na abertura do arquivo\n");
        return NULL;
    }else{    
        while (fscanf(arq,"%f,%f,%f,%f,%f,%f",&d->valores[j][0],&d->valores[j][1],&d->valores[j][2],&d->valores[j][3],&d->classe[j][0],&d->classe[j][1]) != EOF && j < n_linhas){
            //printf("%f, %f, %f, %f | %f %f\n",d->valores[j][0],d->valores[j][1],d->valores[j][2],d->valores[j][3],d->classe[j][0],d->classe[j][1]);
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
            printf("\tN:%d ",i);
            for( int j = 0 ; j < rede->entradas_por_camada[k] ; j++ ){
                printf("In[%d]:%.4f W:%.4f|",j,*rede->neuro[k][i]->entradas[j],rede->neuro[k][i]->pesos[j]);
            }
            printf("\tB:%.4f W:%.4f ",rede->neuro[k][i]->bias,rede->neuro[k][i]->p_bias);
            printf("Out:%.4f\n",rede->neuro[k][i]->saida);
        } 
    }
}

float Ativacao(float x){
    x = 1/(1+ exp(-5*x));
    //if( x < 5.0 )return 0;

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
            //if ( saida < 0.5 ) saida = saida*-1;
            rede->neuro[k][i]->saida = Ativacao(saida);
            rede->neuro[k][i]->v     = saida;
        } 
    }
}

float Derivar_Sigmoid(float x){
    //x = pow(2.71828182,-x)/pow(1+exp(-x),2);
    x = 5*x*(1-x);
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
    temp->med               = 100;
    for( int i = 0 ; i < 10; i++){
    // Aloca a quantidade entradas para cada neuronio
        temp->pesos_old[i] = (float*)malloc(10*sizeof(float));
    }

    temp->neuro             = (neuronio***)malloc(rede->n_camadas*sizeof(neuronio**));
    for( int i = 0 ; i < rede->n_camadas ; i++){
        temp->neuro[i]      = (neuronio**)malloc(rede->neuronios_por_camada[i]*sizeof(neuronio*));
        for( int j = 0 ; j < rede->neuronios_por_camada[i] ; j++){
            temp->neuro[i][j] = Criar_Neuronio(rede->entradas_por_camada[i]);
            //temp->neuro[i][j] = rede->neuro[i][j];
        }
    }
    temp->derivada          = (float**)malloc(rede->n_camadas*sizeof(float*));
    for( int i = 0 ; i < rede->n_camadas ; i++){
        temp->derivada[i]   = (float*)malloc(rede->neuronios_por_camada[i]*sizeof(float));
    }
    // Copiar toda a rede
    *temp->neuro             = *rede->neuro;
    return temp;
}


void Calcular_Erro( rede_neural* rede, float Dy[], float taxa, dados_temp* temp){
    float soma_pesos    = 0;
    temp->med           = 0;
    // Salvar Pesos antigos
    *temp->neuro = *rede->neuro;
    //      ____________________________________Primeiro Caso_____________________________
    //      Calcular e atualizar primeiro os pesos dos neuronios da ultima camada
    //      Primeiro laco serve para percorrer os neuronio da ultima camada
    //      por isso n_camadas - 1, percorrer apenas a ultima camada  \/
    for( int i = 0 ; i < rede->neuronios_por_camada[ rede->n_camadas - 1 ]; i++){
    //      Calculo das Derivadas Parciais
        
        temp->derivada[ rede->n_camadas - 1][i] = 
        -1*(Dy[i] - rede->neuro[ rede->n_camadas - 1 ][i]->saida)*
        (Derivar_Sigmoid(rede->neuro[ rede->n_camadas - 1 ][i]->v));
/*
        if( (Dy[i] - rede->neuro[ rede->n_camadas - 1 ][i]->saida) < 0.5  ){
            temp->derivada[ rede->n_camadas - 1][i] = 
            temp->derivada[ rede->n_camadas - 1][i]* 
            (-1);
         }*/
        temp->med += pow( (Dy[i] - rede->neuro[ rede->n_camadas - 1 ][i]->saida), 2)/2;
        
        //printf("Neuronio:%d Erro: %f Out:%f ",i,(Dy[i] - rede->neuro[ rede->n_camadas - 1 ][i]->saida ),rede->neuro[ rede->n_camadas - 1 ][i]->saida);
    //      Calculo do novo peso do bias
        rede->neuro[    rede->n_camadas - 1][i]->p_bias = 
        rede->neuro[    rede->n_camadas - 1][i]->p_bias +
        -1*
        taxa*
        temp->derivada[ rede->n_camadas - 1][i]*
        rede->neuro[    rede->n_camadas - 1][i]->bias;
    //      Segundo laco serve para percorrer as entradas dos neuronios,
    //      sendo que o numero de entradas e pesos depende do numero de
    //      entradas da camada anterior, por isso e n_camadas - 2    AQUI  \/ 
        for( int j = 0 ; j < rede->neuronios_por_camada[ rede->n_camadas - 2 ] ; j ++){
    //      Atulaizacao dos pesos dos neuronios da ultima camada
    //      peso_new = peso_old - 
    //      (derivada_erro_em_relacao_a_fucao_de_ativacao*
    //      derivada_da_fucao_ativacao_em_relacao_a_saida_do_neuronio*
    //      derivada_da_saida_em_relacao_ao_peso)
            rede->neuro[    rede->n_camadas - 1 ][i]->pesos[j] = 
            rede->neuro[    rede->n_camadas - 1 ][i]->pesos[j] +
            -1*
            taxa*
            temp->derivada[ rede->n_camadas - 1 ][i]*
            (*rede->neuro[  rede->n_camadas - 1 ][i]->entradas[j]);
        }
    }
    //temp->med = temp->med/rede->neuronios_por_camada[ rede->n_camadas - 1];
    //printf("Err:Med:%.2f",temp->med*100);
}
void Atualizar_Pesos( rede_neural* rede, dados_temp* temp, float taxa){
    float        soma_pesos_old;
    // Percorre as camadas
    for( int k = rede->n_camadas -2; k >= 0 ; k--){
    // Percorre os neuronios de cada camada
        for( int i = 0 ; i < rede->neuronios_por_camada[k] ; i++){
            
    // Calculo das derivadas parciais do erro em relacao aos pesos das camadas ocultas
            soma_pesos_old = 0;
            for( int u = 0 ; u < rede->neuronios_por_camada[ k + 1 ]; u++){
                soma_pesos_old += 
                rede->neuro[     k+1][u]->pesos[i]*
                temp->derivada[  k+1][u];
            }
            temp->derivada[ k][i] = -1*Derivar_Sigmoid(rede->neuro[    k ][i]->v)*soma_pesos_old;
            //temp->derivada[ k][i] = soma_pesos_old;

            for(int j = 0 ; j < rede->neuronios_por_camada[k] ; j++){
    // Percorre e soma os pesos da camada anterior ligados os neronio
               // Atualização dos pesos
                rede->neuro[    k ][i]->pesos[j] = 
                rede->neuro[    k ][i]->pesos[j] +
                -1*
                taxa*
                (*rede->neuro[  k ][i]->entradas[j])*
                temp->derivada[ k ][i]*
                Derivar_Sigmoid(rede->neuro[k][i]->v);
            }
    // Atualizacao dos pesos do bias
            rede->neuro[        k ][i]->p_bias = 
            rede->neuro[        k ][i]->p_bias +
            -1*
            taxa*
            rede->neuro[        k ][i]->bias*
            temp->derivada[     k ][i]*
            Derivar_Sigmoid(rede->neuro[k][i]->v);
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
    int epoca = 0;
    float erro_total_med = 0;
    rede_neural * rede;
    dados* flores;
    dados_temp* temp;
    rede = Criar_Rede(3);
    
    Adicionar_Camada(rede,4,0,4);
    Adicionar_Camada(rede,4,1,4);
    Adicionar_Camada(rede,4,2,2);

    temp = Alocar_Memoria_Pesos_old_temp(rede);

    flores = Abrir_Arquivo("iris.txt","rt",4,150,2);
    
    Conectar_Rede(rede,flores->valores[0]);
    Calcular_Saidas(rede);
    while(  temp->med >= 0.05 && epoca < 200000){
        
        for( int i = 0 ; i < 150 ; i++){
            //printf("\tExemplo:%d\n",i);
            //Printar_Rede(rede);
            Calcular_Erro(rede,flores->classe[i],0.01,temp);
            Atualizar_Pesos(rede,temp,0.01);
            Calcular_Saidas(rede);
            Atualizar_entradas(rede,flores->valores[i]);
            erro_total_med += temp->med;
        }
        erro_total_med = erro_total_med/150;
        printf("\t\tEpoca:%d erro:%f Erro Total Medio:%f\n",epoca,temp->med*100, erro_total_med*100);
        epoca++;
    }
}