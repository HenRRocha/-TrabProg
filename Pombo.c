#include <stdlib.h> // Cabecalho de Biblioteca padrao
#include <stdio.h> // Cabecalho de Biblioteca de I/O padrao
#include <pthread.h> // Cabecalho especifico para threads POSIX
#include <semaphore.h> //biblioteca para o uso de semafaros 
#include <unistd.h> // ??? ...

#define N 20

//sem_t semaf_MSG;//
sem_t semaf_LiberaPombo;// 
sem_t semaf_LiberaUsuario;//

int contPostIt = 0;
int VEZES = 0;

void DormeAcordaPombo (){
     puts("Pombo dormindo zZZzzZZzzZZ");
     sleep(3);
}

void ColaPostIt (){
    puts("Post it colado");
}

void levaMochilaAteB (){
    puts("Pombo acordado ! ");
    puts ("Pombo indo para B");
    puts("---->");
    sleep(3);
    puts("Pombo voltou ao A");
    puts("<----");
}       

void *usuario (void* arg){
    while(1){
        sem_wait(&semaf_LiberaUsuario);
        contPostIt++;
        printf("Post it colado %d\n", contPostIt);
        if (contPostIt == N){
            printf("mochila cheia \n");
           // sem_wait(&semaf_LiberaPombo);
        }
    }
}

void *Pombo (void* arg){
    while(1){
       // sem_post(&semaf_LiberaUsuario);
        levaMochilaAteB();
        contPostIt = 0; 
        for (int i=1; i<=N; i++){
            printf("Adic. msg %d\n", i);
        }
    }
}

int main (void){

    int NumeroMensagem = 0; 
    DormeAcordaPombo();
    printf("Digite o numero de usuarios que iram escrever mensagens:");
    scanf("%d", &NumeroMensagem);
    VEZES = NumeroMensagem;

    pthread_t idusuario[N]; 
    pthread_t idpombo;

    register unsigned int i;

   // sem_init(&semaf_MSG, 0, 0);
    sem_init(&semaf_LiberaPombo, 0, 0);
    sem_init(&semaf_LiberaUsuario, 0, 0);

    pthread_create(&idpombo, NULL, Pombo, NULL);

    for( i = 0; i < N; i++){
        pthread_create(&idusuario[i], NULL, usuario, NULL);
    }

    for (i=0; i< N;i++){
        pthread_join(idusuario[i],NULL);
    }

    pthread_join(idpombo, NULL);

    //sem_destroy(&semaf_MSG);
    sem_destroy(&semaf_LiberaPombo);
    sem_destroy(&semaf_LiberaUsuario);

    pthread_exit(NULL);

    return 0;
}
