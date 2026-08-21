/*
 * RPG DE TEXTO - FASE 1
 * ----------------------
 * Um RPG simples por menu, rodando no PSP (via PPSSPP ou PSP real).
 * Comentado em detalhe pra quem está começando em C.
 *
 * O que esse jogo faz:
 *  - Mostra um menu de batalha (Atacar / Usar Poção / Fugir)
 *  - O jogador luta contra um Goblin em turnos
 *  - Usa vida (HP), ataque (ATK) e um item (poção)
 *
 * CONCEITOS DE C USADOS AQUI (fica de olho nos comentários):
 *  - variáveis e tipos (int)
 *  - struct (pra agrupar dados de um personagem)
 *  - funções
 *  - laços (while) e condicionais (if/else)
 *  - números aleatórios (rand)
 */

#include <pspkernel.h>   // funções básicas do sistema PSP (iniciar/sair do módulo)
#include <pspdebug.h>    // pspDebugScreenPrintf: "printf" que desenha na tela do PSP
#include <pspctrl.h>     // leitura dos botões (D-pad, X, O, etc)
#include <stdlib.h>      // rand(), srand()
#include <string.h>      // (não usamos muito aqui, mas é comum precisar)

// Isso aqui é obrigatório em todo homebrew de PSP: registra o módulo do jogo.
PSP_MODULE_INFO("RPG_Fase1", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

// atalho pra função de escrever texto na tela
#define printf pspDebugScreenPrintf

/*
 * STRUCT = uma "ficha de personagem".
 * Em vez de ter várias variáveis soltas (hp_jogador, atk_jogador, hp_goblin...),
 * agrupamos tudo que descreve um personagem num único "pacote".
 */
typedef struct {
    char nome[20];
    int hp;
    int hp_max;
    int atk;
} Personagem;

// ---------- FUNÇÕES AUXILIARES ----------

// Fica esperando o jogador soltar todos os botões (evita menu "voando" sozinho)
void esperarSoltarBotoes() {
    SceCtrlData pad;
    do {
        sceCtrlReadBufferPositive(&pad, 1);
    } while (pad.Buttons != 0);
}

// Lê o estado atual dos botões
SceCtrlData lerBotoes() {
    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);
    return pad;
}

/*
 * Desenha o menu de batalha e devolve a opção escolhida pelo jogador.
 * Usa Cima/Baixo (D-pad) pra mover o cursor e X pra confirmar.
 * Retorna: 0 = Atacar, 1 = Usar Poção, 2 = Fugir
 */
int menuBatalha(int pocoes) {
    int opcaoSelecionada = 0;
    const char *opcoes[3] = { "Atacar", "Usar Pocao", "Fugir" };

    esperarSoltarBotoes();

    while (1) {
        pspDebugScreenSetXY(0, 12);
        printf("O que voce vai fazer?\n\n");

        // desenha as 3 opções, marcando a selecionada com ">"
        int i;
        for (i = 0; i < 3; i++) {
            if (i == opcaoSelecionada) {
                printf(" > %s", opcoes[i]);
            } else {
                printf("   %s", opcoes[i]);
            }
            if (i == 1) {
                printf("  (voce tem %d)", pocoes);
            }
            printf("\n");
        }

        SceCtrlData pad = lerBotoes();

        if (pad.Buttons & PSP_CTRL_UP) {
            opcaoSelecionada--;
            if (opcaoSelecionada < 0) opcaoSelecionada = 2;
            esperarSoltarBotoes();
        } else if (pad.Buttons & PSP_CTRL_DOWN) {
            opcaoSelecionada++;
            if (opcaoSelecionada > 2) opcaoSelecionada = 0;
            esperarSoltarBotoes();
        } else if (pad.Buttons & PSP_CTRL_CROSS) {
            esperarSoltarBotoes();
            return opcaoSelecionada;
        }

        sceKernelDelayThread(50000); // pequena pausa (50ms) pra não ler botão rápido demais
    }
}

// ---------- PROGRAMA PRINCIPAL ----------

int main(int argc, char *argv[]) {
    // inicia a tela de debug/texto do PSP (modo mais simples de desenhar coisas)
    pspDebugScreenInit();

    // cria os dois personagens da batalha
    Personagem jogador = { "Heroi", 30, 30, 6 };
    Personagem goblin  = { "Goblin", 18, 18, 4 };
    int pocoes = 2;

    int rodando = 1;
    int fugiu = 0;

    while (rodando) {
        pspDebugScreenClear();
        pspDebugScreenSetXY(0, 0);

        printf("=== BATALHA ===\n\n");
        printf("%s   HP: %d/%d\n", jogador.nome, jogador.hp, jogador.hp_max);
        printf("%s  HP: %d/%d\n\n", goblin.nome, goblin.hp, goblin.hp_max);

        int escolha = menuBatalha(pocoes);

        pspDebugScreenClear();
        pspDebugScreenSetXY(0, 0);
        printf("=== BATALHA ===\n\n");

        if (escolha == 0) {
            // ATACAR: dano com uma variação aleatória
            int dano = jogador.atk + (rand() % 4); // ataque + 0~3 de variação
            goblin.hp -= dano;
            if (goblin.hp < 0) goblin.hp = 0;
            printf("Voce atacou o %s causando %d de dano!\n", goblin.nome, dano);

        } else if (escolha == 1) {
            // USAR POÇÃO
            if (pocoes > 0) {
                pocoes--;
                jogador.hp += 10;
                if (jogador.hp > jogador.hp_max) jogador.hp = jogador.hp_max;
                printf("Voce usou uma pocao e recuperou 10 de HP!\n");
            } else {
                printf("Voce nao tem mais pocoes!\n");
            }

        } else if (escolha == 2) {
            // FUGIR: 50% de chance de sucesso
            if (rand() % 2 == 0) {
                printf("Voce fugiu da batalha!\n");
                fugiu = 1;
                rodando = 0;
            } else {
                printf("Voce tentou fugir, mas nao conseguiu!\n");
            }
        }

        // se o goblin ainda estiver vivo e o jogador não fugiu, ele contra-ataca
        if (rodando && goblin.hp > 0) {
            int danoGoblin = goblin.atk + (rand() % 3);
            jogador.hp -= danoGoblin;
            if (jogador.hp < 0) jogador.hp = 0;
            printf("%s atacou voce causando %d de dano!\n", goblin.nome, danoGoblin);
        }

        // checa fim de jogo
        if (goblin.hp <= 0) {
            printf("\n%s foi derrotado! Voce venceu!\n", goblin.nome);
            rodando = 0;
        } else if (jogador.hp <= 0) {
            printf("\nVoce foi derrotado...\n");
            rodando = 0;
        } else if (!fugiu) {
            printf("\nPressione X para continuar...\n");
            esperarSoltarBotoes();
            while (1) {
                SceCtrlData pad = lerBotoes();
                if (pad.Buttons & PSP_CTRL_CROSS) break;
                sceKernelDelayThread(50000);
            }
        }
    }

    printf("\nPressione HOME para sair.\n");

    sceKernelExitGame(); // encerra o homebrew corretamente
    return 0;
}
