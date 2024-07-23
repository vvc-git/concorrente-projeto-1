# INE5410 - Trabalho 1 - 2021-1

Este repositório contém o código base do T1 de INE510 do semestre 2021-1.

## Enunciado

![WhatsApp Image 2024-07-23 at 11 36 30](https://github.com/user-attachments/assets/d08d1c41-eb7d-48ec-af32-d3254cf6cea9)

## Solução

# README

## Sincronização de Vikings

### Tabela de Implementação

| Função/Componente            | Descrição                                                                                     |
|------------------------------|-----------------------------------------------------------------------------------------------|
| **Buffer de Mesa**           | Utiliza um buffer (`buffer_mesa`) para representar a mesa. Cada posição pode ter três valores: |
|                              | - `-1`: Espaço vazio                                                                          |
|                              | - `0`: Viking normal                                                                          |
|                              | - `1`: Viking berserker                                                                       |
| **Mutexes**                  | Controlam o acesso ao buffer para garantir que múltiplos vikings não alterem o valor simultaneamente. |
|                              | Impedem mudanças no buffer durante a verificação da disponibilidade do lugar.                 |

### Função `adquire_seats_plates`

| Situação                     | Descrição                                                                                     |
|------------------------------|-----------------------------------------------------------------------------------------------|
| **Mesa com uma cadeira**     | - Trava o mutex na posição 0.                                                                 |
|                              | - Verifica se a posição está vazia e a preenche com `1` ou `0`, conforme o tipo de viking.    |
| **Mesa com várias cadeiras** | - Um laço tenta bloquear uma cadeira verificando `trylock(posicao_mesa) == 0`.                |
|                              | - Chama a função auxiliar `posicao_disponivel` que retorna `1` se a posição é válida, ou `0` se não é. |
|                              | - Se a posição é válida, o buffer é atualizado, e a função `pega_prato` é chamada para obter dois pratos. |
|                              | - O mutex `controla_busca` é destravado, e a posição da cadeira é retornada.                 |

### Espera pelo Término do Banquete

| Componente                   | Descrição                                                                                     |
|------------------------------|-----------------------------------------------------------------------------------------------|
| **Semáforo e Mutex**         | - `termina_comer`: Semáforo para esperar o fim do banquete.                                   |
|                              | - `organ_comida`: Mutex para sincronizar o contador `soma_comidos`.                           |
| **Contador**                 | - Inicia em zero e é incrementado toda vez que um viking libera o lugar.                      |
|                              | - Verifica se todos os vikings terminaram de comer (`soma_comidos == horde_size`).            |
|                              | - Libera o semáforo `termina_comer` com `2*horde_size` posts para permitir que todos os vikings façam suas preces. |

### Escolha de Deuses para as Preces

| Componente                   | Descrição                                                                                     |
|------------------------------|-----------------------------------------------------------------------------------------------|
| **Semáforos**                | Usados para limitar o número de preces a cada deus, garantindo que deuses rivais não recebam um número desigual de preces. |
| **Função `chieftain_get_god`** | - Gera um número aleatório para selecionar um deus.                                           |
|                              | - Verifica se o semáforo do deus tem espaço (`sem_trywait(semaforo[num_rand]) == 0`).         |
|                              | - Se sim, faz posts no semáforo do deus rival e nos semáforos dos super deuses (ODIN e THOR). |
|                              | - Se não, gera um novo número e tenta novamente até encontrar um deus disponível.            |

