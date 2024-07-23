#ifndef __CHIEFTAIN_H__
#define __CHIEFTAIN_H__

    #include "config.h"
    #include "valhalla.h"
    #include "semaphore.h"

    /*============================================================================*
     * DESCRIÇÃO: O chieftain é o chefe da horda de vikings. É através dele que   *
     * os vikings pedem cadeiras e pratos (e os devolvem) e também solicitam      *
     * deuses para realizar as preces.                                            *
    *============================================================================*/

    /**
     * @brief Define os atributos do chieftain.
     */
    typedef struct chieftain
    {
        int initialized;
        valhalla_t *valhalla;
        sem_t *sem_valhalla;
        sem_t termina_comer;
        pthread_mutex_t *controla_busca,*pratos, organ_comida, controla_valhalla;
        int *buffer_mesa,*buffer_pratos;
        int soma_comidos,qtd_vikins_reza; 
        
        /* TODO: Adicione aqui os atributos que achar necessários para implementar o
        comportamento do chieftain. Esses atributos deverão ser usados pelas funções
        do chieftain. */
    } chieftain_t;

    /*============================================================================*
     * Funções utilizadas em arquivos que incluem esse .h                         *
     *============================================================================*/

    /**
     * @brief Inicializa o chieftain (o chefe da horda). Ele controlará
     * uma horda com inicialmente config.horde_size guerreiros e uma mesa com config.table_size
     * cadeiras.
     * 
     * @param self O chieftain.
     */
    extern void chieftain_init(chieftain_t *self, valhalla_t *valhalla);
    
    /**
     * @brief Finaliza o chieftain (o chefe da horda).
     * 
     * @param self O chieftain.
    */
    extern void chieftain_finalize(chieftain_t *self);

    /**
     * @brief Adquire uma cadeira e dois pratos de comida. 
     * 
     * @param self O chieftain.
     * @param berserker Se igual a 1 indica que o viking que chamou este método é um
     * berserker. Por razões de segurança, guerreiros normais e berserkers jamais podem
     * ser atribuídos pelo chieftain a cadeiras adjacentes.
     *
     * @returns Índice da cadeira a ser ocupada pelo guerreiro, como um número no
     * intervalo [0, config.table_size)
     */
    extern int chieftain_acquire_seat_plates(chieftain_t *self, int berserker);

    /**
     * @brief Libera uma cadeira e dois pratos previamente adquiridos via
     * chieftain_acquire_seat_plates().
     *
     * @param self O chieftain.
     * @param pos Índice da cadeira ocupada pelo guerreiro, como um número no
     * intervalo [0, config.table_size)
     */
    extern void chieftain_release_seat_plates(chieftain_t *self, int pos);

    /**
     * @brief Indica ao guerreiro para qual deus viking ele deve fazer suas preces. O deus
     * deve ser escolhido aleatóriamente, mas de forma a respeitar as regras dispostas nas
     * escrituras (ver enunciado).
     *
     * @param self O chieftain.
     * 
     * @returns O Deus atribuído ao guerreiro.
     */
    extern god_t chieftain_get_god(chieftain_t *self);

    /*============================================================================*
    * ATENCÃO: Insira aqui funções que você quiser adicionar a interface para    *
    * serem usadas em arquivos que incluem esse header.                          *
    *============================================================================*/
    extern int posicao_disponivel(int buffer[],int pos, int berseker);
    /**
     * @brief recebe como entrada o buffer, a posicao desejada de insersao, e se o viking e um berseker
     * retorna 1 caso a posicao seja valida para insercao, respeitando as regras de distanciamento entre bersekers
     * e nao bersekers, e retorna 0 caso nao seja possivel inserir aquele viking naquela posicao
     * 
     * 
     * 
     * 
     * 
     **/


    extern int buffer_vazio(int buffer[]);

    /**
     * @brief funcao que retorna 1 se o buffer estiver vazio e 0 caso contrario
     * 
     * @param //buffer
     * 
     * 
     * 
     **/

    extern void pegar_prato (pthread_mutex_t *ptr_pratos, int idx_prato, int buffer[]);

    /**
    * @brief Diz se pegou os dois pratos
    *  
    * @param idx_prato indice do prato que está a sua frente 
    * 
    * @returns 1 se conseguiu pegar os dois pratos e zero se nao consegiu
    */
    extern void soltar_prato (pthread_mutex_t *ptr_pratos, int idx_prato);
    /**
     * @brief libera os semaforos dos pratos
     * 
     * @param idx_prato indice do prato que esta a sua frente
     * 
     * 
     * 
     */
#endif /*__CHIEFTAIN_H__*/