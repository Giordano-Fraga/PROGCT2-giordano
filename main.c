#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LINHAS 6
#define COLUNAS 7

// esse vetor de 3 dimensoes eh a base da logica do fim de jogo e ia pensando que: {x,y} e {{lado 1},{lado 2}}
int direcoes[4][2][2]={
        {{0,-1},{0,1}},    // horizontal
        {{-1,0},{1,0}},    // vertical
        {{-1,-1},{1,1}},   // primeira diagonal
        {{-1,1},{1,-1}}    // segunda diagonal
    };

// check de fim de jogo
bool verificarVitoria(int tabuleiro[LINHAS][COLUNAS], int linha, int coluna, int jogador){
    for(int d = 0; d < 4; d++){
        int cont = 1;
        for(int lado = 0; lado < 2; lado++){
            int dx = direcoes[d][lado][0];
            int dy = direcoes[d][lado][1];
            int x = linha + dx;
            int y = coluna + dy;

            while(x >= 0 && x < LINHAS && y >= 0 && y < COLUNAS && tabuleiro[x][y] == jogador){
                cont++;
                x += dx;
                y += dy;
            }
        }
        if(cont >= 4) return true;
    }
    return false;
}

// check de empate
bool empate(int tabuleiro[LINHAS][COLUNAS]){
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
            if(tabuleiro[i][j] == 0) return false;
        }
    }
    return true;
}

// conta as fichas para o uso do ia
int contFichas(int tabuleiro[LINHAS][COLUNAS], int linha, int coluna, int dx, int dy, int jogador){
    int cont = 0;
    for (int i = 1; i < 4; i++) {
        int x = linha + dx * i;
        int y = coluna + dy * i;
        if (x >= 0 && x < LINHAS && y >= 0 && y < COLUNAS && tabuleiro[x][y] == jogador){
            cont++;
        } else{
            break;
        }
    }
    return cont;
}

// check de coluna valida
bool colunaValida(int tabuleiro[LINHAS][COLUNAS], int coluna) {
    return coluna >= 0 && coluna < COLUNAS && tabuleiro[0][coluna] == 0;
}

// o turno do computador
int jogadaIA(int tabuleiro[LINHAS][COLUNAS]) {
    int melhor_coluna = -1;
    int prioridade = -1;

    // detecta prioridade
    int jogador_humano = 1;
    int jogador_ia = 2;

    for (int coluna = 0; coluna < COLUNAS; coluna++){
        if (!colunaValida(tabuleiro, coluna)) continue; // continue serve para ignorar o resto do codigo e rodar direto a proxima iteracao

        // simular onde a ficha cairia
        int linha_destino = -1;
        for (int i = LINHAS - 1; i >= 0; i--){
            if (tabuleiro[i][coluna] == 0){
                linha_destino = i;
                break;
            }
        }

        // PRIORIDADES DE IA
        // prioridade 1: se o ia tiver 3 fichas em serie, tentar ganhar
        for (int d = 0; d < 4; d++){
            int dx = direcoes[d][0][0];
            int dy = direcoes[d][0][1];
            int serie_ia = contFichas(tabuleiro, linha_destino, coluna, dx, dy, jogador_ia) + contFichas(tabuleiro, linha_destino, coluna, -dx, -dy, jogador_ia);
            if (serie_ia >= 3 && prioridade < 3){
                melhor_coluna = coluna;
                prioridade = 3;
            }
        }

        // prioridade 2: bloquear 3 fichas em serie do jogador
        for (int d = 0; d < 4; d++){
            int dx = direcoes[d][0][0];
            int dy = direcoes[d][0][1];
            int serie_humano = contFichas(tabuleiro, linha_destino, coluna, dx, dy, jogador_humano) + contFichas(tabuleiro, linha_destino, coluna, -dx, -dy, jogador_humano);
            if (serie_humano >= 3 && prioridade < 2){
                melhor_coluna = coluna;
                prioridade = 2;
            }
        }

        // prioridade 3: bloquear 2 fichas em serie do jogador
        for (int d = 0; d < 4; d++){
            int dx = direcoes[d][0][0];
            int dy = direcoes[d][0][1];
            int serie_humano = contFichas(tabuleiro, linha_destino, coluna, dx, dy, jogador_humano) + contFichas(tabuleiro, linha_destino, coluna, -dx, -dy, jogador_humano);
            if (serie_humano == 2 && prioridade < 1){
                melhor_coluna = coluna;
                prioridade = 1;
            }
        }
    }

    // prioridade 4: se nenhuma prioridade previa for necessaria, escolher aleatoriamente
    if (melhor_coluna == -1){
        srand(time(NULL));
        do {
            melhor_coluna = rand() % COLUNAS;
        } while (!colunaValida(tabuleiro, melhor_coluna));
    }

    return melhor_coluna;
}

// menu, nao tenho muito mais a dizer
int menu(SDL_Renderer* renderer){
    SDL_Texture *vs_jogador = IMG_LoadTexture(renderer, "vs_player.png");
    SDL_Texture *vs_computador = IMG_LoadTexture(renderer, "vs_ia.png");

    SDL_Rect rect_vs_jogador = {600, 300, 300, 130};
    SDL_Rect rect_vs_computador = {600, 500, 300, 130};

    SDL_Event event;
    bool running = true;
    while (running){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, vs_jogador, NULL, &rect_vs_jogador);
        SDL_RenderCopy(renderer, vs_computador, NULL, &rect_vs_computador);
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (mouse_x >= rect_vs_jogador.x && mouse_x <= rect_vs_jogador.x + rect_vs_jogador.w &&
                    mouse_y >= rect_vs_jogador.y && mouse_y <= rect_vs_jogador.y + rect_vs_jogador.h) {
                    SDL_DestroyTexture(vs_jogador);
                    SDL_DestroyTexture(vs_computador);
                    return 1; // pvp
                }

                if (mouse_x >= rect_vs_computador.x && mouse_x <= rect_vs_computador.x + rect_vs_computador.w &&
                    mouse_y >= rect_vs_computador.y && mouse_y <= rect_vs_computador.y + rect_vs_computador.h) {
                    SDL_DestroyTexture(vs_jogador);
                    SDL_DestroyTexture(vs_computador);
                    return 2; // pve
                }
            }
        }
    }
    return 0;
}

// tela final, nada para dizer alem que foi chato fazer
int tela_final(SDL_Renderer* renderer) {
    SDL_Texture *fim = IMG_LoadTexture(renderer, "fim.png");
    SDL_Texture *retry = IMG_LoadTexture(renderer, "retry.png");
    SDL_Texture *quit = IMG_LoadTexture(renderer, "quit.png");

    SDL_Rect rect_fim = {600, 200, 300, 130};
    SDL_Rect rect_retry = {600, 400, 300, 130};
    SDL_Rect rect_quit = {600, 600, 300, 130};

    SDL_Event event;
    bool running = true;
    while (running) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fim, NULL, &rect_fim);
        SDL_RenderCopy(renderer, retry, NULL, &rect_retry);
        SDL_RenderCopy(renderer, quit, NULL, &rect_quit);
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (mouse_x >= rect_retry.x && mouse_x <= rect_retry.x + rect_retry.w &&
                    mouse_y >= rect_retry.y && mouse_y <= rect_retry.y + rect_retry.h) {
                    SDL_DestroyTexture(fim);
                    SDL_DestroyTexture(retry);
                    SDL_DestroyTexture(quit);
                    return 1; // retry
                }

                if (mouse_x >= rect_quit.x && mouse_x <= rect_quit.x + rect_quit.w &&
                    mouse_y >= rect_quit.y && mouse_y <= rect_quit.y + rect_quit.h) {
                    SDL_DestroyTexture(fim);
                    SDL_DestroyTexture(retry);
                    SDL_DestroyTexture(quit);
                    return 0; // quit
                }
            }
        }
    }
    return 0;
}

// logica principal do jogo
void jogo(SDL_Renderer* renderer, int gamemode) {
    printf("<Novo jogo criado>\n");

    SDL_Texture *tabuleiro = IMG_LoadTexture(renderer, "jogo_tabuleiro.png");
    SDL_Texture *ficha_vermelha = IMG_LoadTexture(renderer, "ficha_vermelha.png");
    SDL_Texture *ficha_amarela = IMG_LoadTexture(renderer, "ficha_amarela.png");

    SDL_Rect quad1 = {200, 150, 1108, 887};
    int tabuleiro_pecas[LINHAS][COLUNAS] = {0}; // 0 = vazio, 1 = vermelho, 2 = amarelo

    int posicaoY = 50;
    int posicaoX = 275;
    int coluna_selecionada = -1;
    bool caindo = false;
    int jogador = 1; // 1 = vermelho, 2 = amarelo
    int linha_destino = -1;
    int offsetX = 80;

    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (mouse_y < 150 && !caindo) { // selecao de coluna para colocar a ficha
                    coluna_selecionada = -1;
                    if (mouse_x > 200 + offsetX && mouse_x < 331 + offsetX) coluna_selecionada = 0;
                    else if (mouse_x > 331 + offsetX && mouse_x < 462 + offsetX) coluna_selecionada = 1;
                    else if (mouse_x > 462 + offsetX && mouse_x < 593 + offsetX) coluna_selecionada = 2;
                    else if (mouse_x > 593 + offsetX && mouse_x < 724 + offsetX) coluna_selecionada = 3;
                    else if (mouse_x > 724 + offsetX && mouse_x < 855 + offsetX) coluna_selecionada = 4;
                    else if (mouse_x > 855 + offsetX && mouse_x < 986 + offsetX) coluna_selecionada = 5;
                    else if (mouse_x > 986 + offsetX && mouse_x < 1117 + offsetX) coluna_selecionada = 6;

                    if (coluna_selecionada != -1 && !caindo) { // check para descobrir espaco livre
                        linha_destino = -1;
                        for (int i = LINHAS - 1; i >= 0; i--) {
                            if (tabuleiro_pecas[i][coluna_selecionada] == 0) {
                                linha_destino = i;
                                break;
                            }
                        }

                        if (linha_destino != -1) {
                            posicaoX = 275 + coluna_selecionada * 131;
                            posicaoY = 50;
                            caindo = true;
                        }
                    }
                }
            }
        }

        if (caindo) { // fazer a ficha cair
            int posicaoY_destino = 50 + linha_destino * 150;
            if (posicaoY < posicaoY_destino) {
                posicaoY += 25;
            } else {
                tabuleiro_pecas[linha_destino][coluna_selecionada] = jogador;
                caindo = false;

                if (verificarVitoria(tabuleiro_pecas, linha_destino, coluna_selecionada, jogador)) { // terminar o jogo se player venceu
                    printf("Jogador %d venceu!\n", jogador);
                    running = false;
                    int escolha = tela_final(renderer);
                    if (escolha == 1) {
                        int gamemode = menu(renderer);
                        jogo(renderer, gamemode);
                    } else {
                        running = false;
                    }
                } else if (empate(tabuleiro_pecas)) { // terminar jogo se empate aconteceu
                    printf("Empate! O tabuleiro está cheio.\n");
                    running = false;
                    int escolha = tela_final(renderer);
                    if (escolha == 1) {
                        int gamemode = menu(renderer);
                        jogo(renderer, gamemode);
                    } else {
                        running = false;
                    }
                }
                jogador = (jogador % 2) + 1; // trocar o turno
                if (gamemode == 2 && jogador == 2) { // se for contra ia, rodar codigo de ia
                coluna_selecionada = jogadaIA(tabuleiro_pecas);
                linha_destino = -1;
                for (int i = LINHAS - 1; i >= 0; i--) {
                    if (tabuleiro_pecas[i][coluna_selecionada] == 0) {
                        linha_destino = i;
                        break;
                    }
                }
                if (linha_destino != -1) { 
                    posicaoX = 275 + coluna_selecionada * 131;
                    posicaoY = 50;
                    caindo = true;
                }
            }

            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < LINHAS; i++) { // renderizacao da ficha estatica depois de ter caido
            for (int j = 0; j < COLUNAS; j++) {
                if (tabuleiro_pecas[i][j] != 0) {
                    SDL_Rect posicao_peca = {277 + j * 133, 185 + i * 133, 132, 132};
                    if (tabuleiro_pecas[i][j] == 1) {
                        SDL_RenderCopy(renderer, ficha_vermelha, NULL, &posicao_peca);
                    } else if (tabuleiro_pecas[i][j] == 2) {
                        SDL_RenderCopy(renderer, ficha_amarela, NULL, &posicao_peca);
                    }
                }
            }
        }

        if (caindo) {
            SDL_Rect posicao_inicial = {posicaoX, posicaoY, 132, 132};
            if (jogador == 1) {
                SDL_RenderCopy(renderer, ficha_vermelha, NULL, &posicao_inicial);
            } else if (jogador == 2) {
                SDL_RenderCopy(renderer, ficha_amarela, NULL, &posicao_inicial);
            }
        }
        SDL_RenderCopy(renderer, tabuleiro, NULL, &quad1);
        SDL_RenderPresent(renderer);
        SDL_Delay(20); // cap de fps
    }

    SDL_DestroyTexture(tabuleiro);
    SDL_DestroyTexture(ficha_vermelha);
    SDL_DestroyTexture(ficha_amarela);
}

// main
int main(int argc, char** argv){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow( // cria janela principal
        "Connect Four",
        100, 100,
        1500, 1024,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    int gamemode = menu(renderer);
    printf("Modo selecionado: ");
    if(gamemode==1) printf ("Jogador VS Jogador\n");
    else printf ("Jogador Vs Computador\n");

    jogo(renderer, gamemode);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("--- END OF THE LINE ---");

    return 0; // fim !
}