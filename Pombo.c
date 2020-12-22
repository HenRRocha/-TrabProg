#include <stdlib.h> // Cabecalho de Biblioteca padrao
#include <stdio.h> // Cabecalho de Biblioteca de I/O padrao
#include <pthread.h> // Cabecalho especifico para threads POSIX
#include <semaphore.h> //biblioteca para o uso de semafaros 
#include <unistd.h> // ??? ...

#define N 20
#define MSG 20
#define RANDOM 20  // entender por que o professor colocou no vetor la em baixo 

int VEZES = 0;

sem_t semaf_MSG;//comtrola o numero de mensagens
sem_t semaf_LiberaPombo;//libera o pombo 
sem_t semaf_Quant_MSG;//contola o acesso a quantidade de mensagens 
sem_t semaf_Espera_MSG;//bloqueia a quantidade das mensagens 
sem_t semaf_Se_Pombo_Dorme;//Controla o sono do Pombo   

//variavel que controla o numero de mensagens 
unsigned int quant_mensagens = 0;

void leva_mochila_ate_B_e_volta();
void dorme_pombo ();
void acorda_pombo ();

//THREAD 1 
void *usuario (void* arg){ //Cria mensagens 
    register unsigned int h;
    while(h++ < VEZES){//VEZES == NUMERO DE USUARIOS
        sem_wait (&semaf_MSG);// Down ->  bloqueia uma mensagem de cada vez/Libera o variavel quant_passageiros
        puts("mensagem na mochila");
        sem_wait (&semaf_Quant_MSG);// para alterar o varialvel quantidade de menagens 
        quant_mensagens++;
        printf("quantidade de mensagens na mochila : %d\n ", quant_mensagens);
        if (quant_mensagens == MSG){ //se numero de mensagens for igual ao numero de vezes estabelecido 
            puts("Mochila cheia !");
            acorda_pombo ();
            sem_post (&semaf_LiberaPombo);// Up -> libera o pombo para viagem
            sem_post (&semaf_Quant_MSG);// Up -> libera para mais umamensagem entrar --> POR QUE ???
            sem_wait(&semaf_Espera_MSG);//Down ->bloqueia as proximas mensagens 
        }else{
            sem_post(&semaf_Quant_MSG);//up -> Liberar para mais uma mensgens entrar na mochila 
        }
        sem_post(&semaf_MSG);
    }
    pthread_exit(NULL);
}

//THREAD 2  
void *Pombo (void* arg){// ATENÇÃO !! falta rever esse thread
    while(1){
        // puts("Pombo aguardando ...");
        dorme_pombo();
        sem_wait(&semaf_LiberaPombo);//Down -> Bloqueia o pombo 
        leva_mochila_ate_B_e_volta();//Pombo viaja 
        sem_wait(&semaf_Quant_MSG);//Down -> Bloqueia o pombo 
        quant_mensagens = 0;    //Zera quantidade de mensagens 
        sem_post(&semaf_Quant_MSG);//Up -> Libera quantidade de mensagens 
        sem_post(&semaf_Espera_MSG);//Up -> Libera o bloqueia mensagens 
    }
    pthread_exit(NULL);
}

//Função Pombo viaja 
void leva_mochila_ate_B_e_volta(){ 
    puts("Pombo liberado ! ");
    puts ("Pombo indo para B");
    puts("---->");
    sleep(3);
    puts("Pombo voltou ao A");
    puts("<----");
    if (quant_mensagens != MSG){
        dorme_pombo();
    }
    
}
//Função Pombo dorme      //ATENÇÃO !! precisa ser usada !!!
void dorme_pombo (){
    puts("Pombo dormindo zZZzzZZzzZZ");
    sem_wait(&semaf_Se_Pombo_Dorme); //Down -> Bloqueia pombo/sono
    sleep(1);
}
void acorda_pombo (){
    puts("Pombo acordado @#@#@#@#@#@");
    sem_post(&semaf_Se_Pombo_Dorme);//Up -> Libera pombo/sono
}

int main (void){
    //entrada do usuario com o numero de VEZES numero de "usuarios"
    int NumeroMensagem = 0; 
    printf ("Pombo no local A\n");
    printf("Digite o numero de usuarios que iram escrever mensagens:");
    scanf("%d", &NumeroMensagem);
    VEZES = NumeroMensagem;
    
    pthread_t idusuario[RANDOM]; // por que ????
    pthread_t idpombo;

    register unsigned int i;

    //inicializa semaforos 
    sem_init(&semaf_MSG, 0, 1);
    sem_init(&semaf_LiberaPombo, 0, 0);
    sem_init(&semaf_Quant_MSG, 0, 1);
    sem_init(&semaf_Espera_MSG, 0, 0);
    sem_init(&semaf_Se_Pombo_Dorme, 0, 0);

    //cria thread do pombo
    pthread_create(&idpombo, NULL, Pombo, NULL);

    //cria varios threads "usuarios" que escrevem mensagens
    for( i = 0; i < N; i++){
        pthread_create(&idusuario[i], NULL, usuario, NULL);
    }

    //aguarda as threads dos usuarios terminar 
    for (i=0; i<N;i++){
        pthread_join(idusuario[i],NULL);
    }

    //aguarda a thread do pombo terminar 
    pthread_join(idpombo, NULL);

    sem_destroy(&semaf_MSG);
    sem_destroy(&semaf_LiberaPombo);
    sem_destroy(&semaf_Quant_MSG);
    sem_destroy(&semaf_Espera_MSG);
    sem_destroy(&semaf_Se_Pombo_Dorme);
    
    pthread_exit(NULL);

    return 0;
}
