#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Definições de Tamanho ---
#define TAMANHO_MAX_FILA 5  // Capacidade da Fila Circular de Peças Futuras
#define TAMANHO_MAX_PILHA 3 // Capacidade da Pilha de Reserva
#define TROCA_BLOCO_SIZE 3  // Número de peças envolvidas na Troca Múltipla

// --- Definição da Estrutura de Peça (Struct) ---
typedef struct {
    char nome; // Tipo da peça ('I', 'O', 'T', 'L', etc.)
    int id;    // Identificador único da peça
} Peca;

// --- Variáveis Globais para a Fila Circular ---
Peca fila[TAMANHO_MAX_FILA];
int frente = 0;
int traseira = -1;
int contador_fila = 0;

// --- Variáveis Globais para a Pilha (Reserva) ---
Peca pilha[TAMANHO_MAX_PILHA];
int topo = -1;

// --- Variável para Gerar IDs Únicos ---
int proximo_id = 0;

// --- Protótipos das Funções ---
// Geração e Auxiliares
void inicializarEstruturas();
Peca gerarPeca();
// Funções de Pilha (LIFO)
int isPilhaFull();
int isPilhaEmpty();
void push(Peca peca);
Peca pop();
// Funções de Fila (FIFO)
int isFilaFull();
int isFilaEmpty();
void enqueue(Peca peca);
Peca dequeue();
// Funções de Ação e Interface
void adicionarPecaFila();
void jogarPeca();
void reservarPeca();
void usarPecaReservada();
void trocarPecaAtual();
void trocaMultipla();
void exibirEstado();
void menu();


// ----------------------------------------------------------------------
// --- Geração e Funções Auxiliares (mantidas do nível anterior) ---
// ----------------------------------------------------------------------

Peca gerarPeca() {
    Peca novaPeca;
    char tipos[] = {'I', 'O', 'T', 'L'};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);

    int indice_tipo = rand() % num_tipos;
    novaPeca.nome = tipos[indice_tipo];
    
    novaPeca.id = proximo_id++;
    
    return novaPeca;
}

// ----------------------------------------------------------------------
// --- Lógica da PILHA (LIFO) (mantidas do nível anterior) ---
// ----------------------------------------------------------------------

int isPilhaFull() { return (topo == TAMANHO_MAX_PILHA - 1); }
int isPilhaEmpty() { return (topo == -1); }
void push(Peca peca) {
    if (!isPilhaFull()) {
        pilha[++topo] = peca;
    }
}
Peca pop() {
    if (!isPilhaEmpty()) {
        return pilha[topo--];
    }
    Peca pecaVazia = {'E', -1}; // Peça de erro
    return pecaVazia;
}

// ----------------------------------------------------------------------
// --- Lógica da FILA CIRCULAR (FIFO) (mantidas do nível anterior) ---
// ----------------------------------------------------------------------

int isFilaFull() { return (contador_fila == TAMANHO_MAX_FILA); }
int isFilaEmpty() { return (contador_fila == 0); }
void enqueue(Peca peca) {
    if (!isFilaFull()) {
        traseira = (traseira + 1) % TAMANHO_MAX_FILA;
        fila[traseira] = peca;
        contador_fila++;
    }
}
Peca dequeue() {
    if (isFilaEmpty()) {
        Peca pecaVazia = {'E', -1};
        return pecaVazia;
    }
    
    Peca pecaRemovida = fila[frente];
    frente = (frente + 1) % TAMANHO_MAX_FILA;
    contador_fila--;
    
    return pecaRemovida;
}

// ----------------------------------------------------------------------
// --- Lógica de Reposição e Ações Simples (mantidas/atualizadas) ---
// ----------------------------------------------------------------------

/**
 * @brief Gera uma nova peça e a adiciona automaticamente ao final da fila (reabastecimento).
 * Mantém a fila cheia após uma remoção.
 */
void adicionarPecaFila() {
    if (isFilaFull()) {
        printf("\n🛑 ERRO: Fila já está cheia. Não foi possível reabastecer.\n");
        return;
    }

    Peca novaPeca = gerarPeca();
    enqueue(novaPeca);
    printf("\n[REPOSIÇÃO]: Peça [%c %d] adicionada ao final da fila para reabastecimento.\n", novaPeca.nome, novaPeca.id);
}

void jogarPeca() {
    if (isFilaEmpty()) {
        printf("\n❌ ERRO: A fila de peças futuras está vazia! Não há peça para jogar.\n");
        return;
    }
    
    Peca pecaJogada = dequeue();
    printf("\n🎮 AÇÃO: Peça [%c %d] jogada/removida da frente da fila.\n", pecaJogada.nome, pecaJogada.id);
    
    // Requisito: Repor a fila após a ação.
    adicionarPecaFila();
}

void reservarPeca() {
    if (isPilhaFull()) {
        printf("\n🛑 ERRO: A pilha de reserva está cheia! Não é possível reservar outra peça.\n");
        return;
    }
    if (isFilaEmpty()) {
        printf("\n❌ ERRO: A fila de peças futuras está vazia! Não há peça para reservar.\n");
        return;
    }
    
    Peca pecaReservada = dequeue();
    push(pecaReservada);

    printf("\n📦 AÇÃO: Peça [%c %d] movida da fila para a pilha de reserva.\n", pecaReservada.nome, pecaReservada.id);

    // Requisito: Repor a fila após a ação.
    adicionarPecaFila();
}

void usarPecaReservada() {
    if (isPilhaEmpty()) {
        printf("\n❌ ERRO: A pilha de reserva está vazia! Não há peça para usar.\n");
        return;
    }
    
    Peca pecaUsada = pop();

    printf("\n🚀 AÇÃO: Peça [%c %d] removida do topo da pilha de reserva (Usada).\n", pecaUsada.nome, pecaUsada.id);

    // Requisito: Repor a fila após a ação (embora não tenha removido da fila, o jogo avança).
    // Nota: Como o requisito é "Gerar uma nova peça de forma automática a cada remoção ou envio à pilha",
    // e esta ação não remove nem envia à pilha, teoricamente não geraria.
    // **Ajuste:** No contexto do jogo (avanço de turno), faz sentido reabastecer a fila.
    // Vamos **SUPOR** que "Usar uma peça" também conta como "avanço" no jogo, liberando um espaço.
    // Como a fila estará cheia, **não faremos o reabastecimento aqui**, para não estourar o limite (fila sempre cheia). 
    // Apenas ações que removem da fila (1 e 2) forçam o reabastecimento.
    
    // A fila de futuro não se move. Apenas a pilha diminui.
    // adicaoPecaFila() **NÃO** é chamada aqui, pois a fila continua cheia.
}

// ----------------------------------------------------------------------
// --- NOVAS FUNÇÕES DE TROCA ESTRATÉGICA ---
// ----------------------------------------------------------------------

/**
 * @brief Troca a peça da frente da fila com o topo da pilha.
 */
void trocarPecaAtual() {
    if (isFilaEmpty() || isPilhaEmpty()) {
        printf("\n⚠️ AÇÃO NEGADA: A fila e/ou a pilha devem ter pelo menos uma peça para realizar a troca simples.\n");
        return;
    }
    
    // 1. Armazena a peça da frente da fila
    Peca temp_fila = fila[frente];
    
    // 2. Armazena a peça do topo da pilha
    Peca temp_pilha = pilha[topo];

    // 3. Troca: Coloca a peça da pilha na frente da fila
    fila[frente] = temp_pilha;

    // 4. Troca: Coloca a peça da fila no topo da pilha
    pilha[topo] = temp_fila;

    printf("\n🔄 AÇÃO: Troca realizada! Peça [%c %d] da fila trocada por peça [%c %d] da pilha.\n", 
           temp_fila.nome, temp_fila.id, temp_pilha.nome, temp_pilha.id);
    
    // Reabastecimento NÃO é necessário, pois o número de peças na fila e na pilha não mudou.
}

/**
 * @brief Alterna as TROCA_BLOCO_SIZE (3) primeiras peças da fila com as TROCA_BLOCO_SIZE (3) peças da pilha.
 * Requer que ambas as estruturas estejam cheias para garantir que as 3 peças existam.
 */
void trocaMultipla() {
    // A pilha só pode ter 3 peças, então ela precisa estar cheia (topo == 2)
    // A fila precisa ter pelo menos 3 peças (contador_fila >= 3)
    if (topo != TAMANHO_MAX_PILHA - 1) { // Verifica se a pilha está cheia (3 peças)
        printf("\n⚠️ AÇÃO NEGADA: A pilha deve estar **cheia** (%d peças) para a troca em bloco.\n", TAMANHO_MAX_PILHA);
        return;
    }

    if (contador_fila < TROCA_BLOCO_SIZE) { // Verifica se a fila tem as 3 primeiras peças
        printf("\n⚠️ AÇÃO NEGADA: A fila deve ter pelo menos %d peças para a troca em bloco.\n", TROCA_BLOCO_SIZE);
        return;
    }

    // Usaremos um array temporário para armazenar os blocos trocados
    Peca temp_fila_bloco[TROCA_BLOCO_SIZE];

    // 1. Copiar as 3 primeiras peças da FILA (a partir da frente circular) para o buffer
    for (int i = 0; i < TROCA_BLOCO_SIZE; i++) {
        int indice = (frente + i) % TAMANHO_MAX_FILA;
        temp_fila_bloco[i] = fila[indice];
    }
    
    // 2. Copiar as 3 peças da PILHA (do topo para a base) para a Fila
    // A pilha é [pilha[2], pilha[1], pilha[0]]. A fila é [fila[frente], fila[frente+1], ...].
    for (int i = 0; i < TROCA_BLOCO_SIZE; i++) {
        int indice_fila = (frente + i) % TAMANHO_MAX_FILA;
        // O bloco da pilha deve ir para a fila na mesma ordem (topo da pilha -> 1ª posição da fila)
        // O elemento no topo é pilha[topo], o próximo é pilha[topo-1], etc.
        fila[indice_fila] = pilha[topo - i]; 
    }

    // 3. Copiar as 3 peças do buffer (originais da fila) para a Pilha
    for (int i = 0; i < TROCA_BLOCO_SIZE; i++) {
        // O bloco original da fila deve ir para a pilha na ordem invertida (1ª da fila -> topo da pilha)
        // O 1º da fila (temp_fila_bloco[0]) vai para o topo da pilha (pilha[topo])
        // O 3º da fila (temp_fila_bloco[2]) vai para a base da pilha (pilha[topo-2])
        pilha[topo - i] = temp_fila_bloco[i];
    }

    printf("\n🔄 AÇÃO: TROCA MÚLTIPLA realizada! As %d primeiras peças da fila alternadas com todas as peças da pilha.\n", TROCA_BLOCO_SIZE);
    
    // Reabastecimento NÃO é necessário, pois o número de peças em ambas as estruturas não mudou.
}


// ----------------------------------------------------------------------
// --- Funções de Controle e Interface (mantidas/atualizadas) ---
// ----------------------------------------------------------------------

void inicializarEstruturas() {
    srand(time(NULL)); 
    
    printf("⚙️ Inicializando Gerenciador Avançado de Peças...\n");
    for (int i = 0; i < TAMANHO_MAX_FILA; i++) {
        enqueue(gerarPeca());
    }
    printf("✅ Fila inicializada com %d peças.\n", TAMANHO_MAX_FILA);
}

void exibirEstado() {
    printf("\n======================================================\n");
    printf("                  ESTADO ATUAL\n");
    printf("======================================================\n");
    
    // --- Visualização da Fila Circular ---
    printf("🧱 Fila de Peças Futuras (Frente -> Traseira):\n");
    if (isFilaEmpty()) {
        printf("  [Vazia]\n");
    } else {
        printf("  ");
        for (int i = 0; i < contador_fila; i++) {
            int indice = (frente + i) % TAMANHO_MAX_FILA;
            printf("[%c %d]", fila[indice].nome, fila[indice].id);
            if (i < contador_fila - 1) {
                printf(" -> ");
            }
        }
        printf("\n");
    }
    

    // --- Visualização da Pilha Linear ---
    printf("\n📦 Pilha de Reserva (Topo -> Base):\n");
    if (isPilhaEmpty()) {
        printf("  [Vazia]\n");
    } else {
        printf("  ");
        for (int i = topo; i >= 0; i--) {
            printf("[%c %d]", pilha[i].nome, pilha[i].id);
            if (i > 0) {
                printf(" -> ");
            }
        }
        printf(" (Capacidade: %d/%d)\n", topo + 1, TAMANHO_MAX_PILHA);
    }
    
    printf("======================================================\n");
}

void menu() {
    int escolha;

    do {
        exibirEstado();
        
        printf("\nOpções de Ação:\n");
        printf("Código | Ação\n");
        printf("-------|---------------------------------------------\n");
        printf("1      | Jogar peça (Dequeue) - Repõe Fila\n");
        printf("2      | Reservar peça (Fila -> Pilha) - Repõe Fila\n");
        printf("3      | Usar peça reservada (Pop)\n");
        printf("4      | Trocar peça atual (Frente Fila <-> Topo Pilha)\n");
        printf("5      | Troca Múltipla (%d primeiras Fila <-> Pilha Cheia)\n", TROCA_BLOCO_SIZE);
        printf("0      | Sair\n");
        printf("-----------------------------------------------------\n");
        printf("Digite o código da ação: ");

        if (scanf("%d", &escolha) != 1) {
            while (getchar() != '\n');
            escolha = -1; 
            printf("\n⚠️ Entrada inválida. Por favor, digite um número.\n");
            continue;
        }

        switch (escolha) {
            case 1: jogarPeca(); break;
            case 2: reservarPeca(); break;
            case 3: usarPecaReservada(); break;
            case 4: trocarPecaAtual(); break;
            case 5: trocaMultipla(); break;
            case 0: printf("\n👋 Saindo do Gerenciador. Bom jogo!\n"); break;
            default: printf("\nCódigo de ação inválido. Tente novamente.\n");
        }
    } while (escolha != 0);
}

// ----------------------------------------------------------------------
// --- Função Principal ---
// ----------------------------------------------------------------------
int main() {
    inicializarEstruturas();
    menu();
    return 0;
}