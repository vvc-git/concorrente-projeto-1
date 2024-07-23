#include <stdlib.h>
#include "config.h"
#include "chieftain.h"
#include "valhalla.h"



void chieftain_init(chieftain_t *self, valhalla_t *valhalla)
{
    /* TODO: Adicionar código aqui se necessário! */
    self -> valhalla = valhalla;
    self->initialized = 1;
    int tamanho = config.table_size;
    self -> soma_comidos = 0;

    //  Alocacao de memoria para os buffers semaforos e mutexes
    self->buffer_mesa   = (int*)malloc(config.table_size * sizeof(config.table_size));
    self->buffer_pratos = (int*)malloc(config.table_size * 2 * sizeof(config.table_size));
    self->controla_busca  = malloc(config.table_size * sizeof(pthread_mutex_t));
    self -> sem_valhalla = (sem_t*)malloc(14 * sizeof(int));

    //  Inicializacao dos semaforos e mutexes 'simples'
    pthread_mutex_init(&self->organ_comida,NULL);
    pthread_mutex_init(&self->controla_valhalla,NULL);
    sem_init(&self -> termina_comer,0,0);


    // inicializa o semaforo valhalla
    for (int i = 0 ; i < 14 ; i++) {
        sem_init(&self->sem_valhalla[i],0,1);
    }

    //  Coloca -1 em todas as posicoes do buffer_mesa e inicializa os semaforos pratos e os mutexes controla_busca
    if (tamanho == 1) {self->pratos = (pthread_mutex_t*)malloc(3 * sizeof(pthread_mutex_t));}
    else {self->pratos = (pthread_mutex_t*)malloc(config.table_size * sizeof(pthread_mutex_t));}
    
    
    //  Coloca -1 em todas as posicoes do buffer e incia os mutex  
    if (tamanho == 1) {
        for (int i = 0 ; i < config.table_size ; i++) {pthread_mutex_init(&self->pratos[0], NULL);}

    } else {

        for (int i = 0 ; i < config.table_size ; i++) {
            self->buffer_mesa[i] = -1;
            pthread_mutex_init(&self->controla_busca[i], NULL);
            pthread_mutex_init(&self->pratos[i], NULL);
        }
    }

    // toda vez que um viking terminar de comer dar post nesse semaforo.
    // NOTA pegar o status dele e comparar com status.horde_size para ver se todos terminaram.
    plog("[chieftain] Initialized\n");
}

int chieftain_acquire_seat_plates(chieftain_t *self, int berserker)
{   


    if (config.table_size == 1) {
        pthread_mutex_lock(&self->controla_busca[0]); 
        if (self->buffer_mesa[0] == -1) {
            self->buffer_mesa[0] = berserker;
            //pegar_prato(self->pratos, 0, self->buffer_mesa);
        }
        pthread_mutex_unlock(&self->controla_busca[0]);
        return 0;

    } else {

        for(int i = 0; i < config.table_size; i++) {

            // Tenta bloqueiar a cadeira de indice i para que possamos verificar se 
            // as que estao ao seu lado estao de acordo com o ex

            if (pthread_mutex_trylock(&self->controla_busca[i]) == 0) {
                
                // Verifica se a cadeira esta vazia e se do seu lado esta um viking do mesmo tipo
                // ou vazaio

                int disponivel = posicao_disponivel(self->buffer_mesa, i, berserker);
                
                if (disponivel == 1)
                {
                    self->buffer_mesa[i] = berserker;
                    //pegar_prato(self->pratos, i, self->buffer_mesa);
                    pthread_mutex_unlock(&self->controla_busca[i]);
                    //printa_buffer(self->buffer_mesa, config.table_size);
                    return i;
                
                } else {
                
                    // Se estiver na ulimt posicao, precisa voltar pro inicio e 
                    // comeca procura novas cadeiras

                    if (config.table_size-1 == i) {
                        int aux = i; i = 0;
                        pthread_mutex_unlock(&self->controla_busca[aux]);
                    }
                    pthread_mutex_unlock(&self->controla_busca[i]);

                    }

            } else {
                // Se estiver na ultima posicao, precisa voltar pro inicio e 
                // comeca procura novas cadeiras

                if (config.table_size-1 == i) {i = 0;}                
            }    
        }
    }
    return -1;
}

void chieftain_release_seat_plates(chieftain_t *self, int pos)
{

    self->buffer_mesa[pos] = -1;
    soltar_prato(self->pratos, pos);
    pthread_mutex_lock(&self->organ_comida);
    self -> soma_comidos++;
    pthread_mutex_unlock(&self->organ_comida);
    if(self -> soma_comidos == config.horde_size) {
        for (int i = 0 ; i < 2*config.horde_size; i++) {
            sem_post(&self->termina_comer);
        }
        self->soma_comidos++;
    }
 
}

god_t chieftain_get_god(chieftain_t *self)
{
    /* TODO: Implementar! O código abaixo deve ser modificado! */
    // espera todos terminarem de comer
    sem_wait(&self->termina_comer);
    msleep(2);
    int qtd_vik;
    sem_getvalue(&self->termina_comer, &qtd_vik);
    self->qtd_vikins_reza = (config.horde_size*2) - qtd_vik;
    int num_rand;
    god_t god = THOR;
    // loop infinito que gera um numero aleatorio entre 0 e 7 (identificador de cada deus)
    for (;;) {
        num_rand = (rand() % 8);
        // caso ele consiga dar wait no semaforo do deus indicado ele entrara dentro desse if
        if (sem_trywait(&self->sem_valhalla[num_rand]) == 0) {
            int rival = valhalla_get_rival(num_rand);
            // e sera feito um post nos semaforos do deus rival e dos deuses especiais
            sem_post(&self -> sem_valhalla[rival]);
            sem_post(&self -> sem_valhalla[6]);
            sem_post(&self -> sem_valhalla[7]);
            return num_rand;
        }
    }
    return god;
}

void chieftain_finalize(chieftain_t *self)
{   
    int tamanho = config.table_size;

    //  Destroi mtx para controle
    for (int i =0; i < tamanho; i++) {
        pthread_mutex_destroy(&self->controla_busca[i]);
    }

    // mtx para pratos
    if (config.table_size  == 1) {tamanho = 3;}
        
    for (int i = 0 ; i < tamanho ; i++) {
        pthread_mutex_destroy(&self->pratos[i]);
    }

    for (int i = 0 ; i < config.table_size ; i++) {
        pthread_mutex_destroy(&self->controla_busca[i]);
    }
    for (int i = 0 ; i < 14 ; i++) {
        sem_destroy(&self->sem_valhalla[i]);
    }

    free(self->buffer_pratos);
    free(self->buffer_mesa);
    free(self->controla_busca);
    free(self->pratos);
    free(self->sem_valhalla);
    pthread_mutex_destroy(&self->organ_comida);
    sem_destroy(&self -> termina_comer);
    pthread_mutex_destroy(&self->controla_valhalla);
    plog("[chieftain] Finalized\n");
}

// funcao que retorna 1 se o buffer estiver vazio e 0 caso contrario
int buffer_vazio(int buffer[]) {
    for (int i = 0 ; i < config.table_size ; i++) {
        if (buffer[i] != -1) {
            return 0;
        }
    }
    return 1;
}

int posicao_disponivel(int buffer[], int pos, int berserker) {
    if (buffer[pos] == -1) {
        if (config.table_size == 1) {return 1;}
        else {
            if (pos == 0) {
                if (buffer[1] == berserker || buffer[1] == -1) {return 1;}
            } else if (pos == config.table_size-1){
                if (buffer[pos - 1] == berserker || buffer[pos -1] == -1) {return 1;}
            } else {
                if (((buffer[pos - 1] == berserker) && (buffer[pos + 1] == berserker)) ||
                    ((buffer[pos - 1] == berserker) && (buffer[pos + 1] == -1)) ||
                    ((buffer[pos - 1] == -1) && (buffer[pos + 1] == berserker)) )
                    {return 1;}
            }
        }
    }
    return 0;
}

void pegar_prato (pthread_mutex_t *ptr_pratos, int idx_prato, int buffer[]) {
    plog("Posicao recebida ->>  %d \n", idx_prato);
    int prato_fre = idx_prato;
    int prato_esq = prato_fre - 1;
    int prato_dir = (prato_fre + 1) % config.table_size;

    if (buffer_vazio(buffer)) {
        if (config.table_size == 1) {prato_esq = 0; prato_dir = 1;} 
        else if (config.table_size == 2) {prato_esq= prato_dir; prato_dir = prato_fre;}
        else {prato_esq = config.table_size -1;}
        
        pthread_mutex_lock(&ptr_pratos[prato_dir]);
        plog("[1 prato]: %d\n", prato_dir);
        pthread_mutex_lock(&ptr_pratos[prato_esq]);
        plog("[2 prato]: %d\n", prato_esq);


    } else {

        if (config.table_size == 2) {prato_esq= prato_dir; prato_dir = prato_fre;}
        pthread_mutex_lock(&ptr_pratos[prato_esq]);
        plog("[1 prato]: %d\n", prato_esq);
        pthread_mutex_lock(&ptr_pratos[prato_dir]);
        plog("[2 prato]: %d\n", prato_dir);
    }

}

void soltar_prato (pthread_mutex_t *ptr_pratos, int idx_prato) {
    int prato_fre = idx_prato;
    int prato_esq = prato_fre - 1;
    int prato_dir = (prato_fre + 1) % config.table_size;


    if (config.table_size == 1) {prato_esq = 0; prato_dir = 1;} 
    else if (config.table_size == 2) {prato_esq= prato_dir; prato_dir = prato_fre;}
    pthread_mutex_unlock(&ptr_pratos[prato_esq]);
    pthread_mutex_unlock(&ptr_pratos[prato_dir]);
}
