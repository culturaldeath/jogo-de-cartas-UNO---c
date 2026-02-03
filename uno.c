#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TUDO 108

typedef enum {placeholder, vermelho, verde, amarelo, azul} cores;
typedef enum {nenhum, bloqueio, inversao, trocacor, m2, m4} poderes;
typedef enum {zero, um, dois, tres, quatro, cinco, seis, sete, oito, nove} valores;
typedef enum {normal, especial, curinga} tipos;

typedef struct { //cria as cartas
    tipos tipo; // ----> sempre presentes
    cores cor;
    union { // -----> pras cartas especiais
        valores valor;
        poderes poder;
    };
} carta;
    
typedef struct { //cria o baralho
    int topo;
    carta cartas[TUDO];
} stack;

typedef struct { //cria os jogadores
	char nome[20];
	int id;
	stack mao;
} jogador;

// utilitários
int vazio(stack *s) {
    return s->topo == -1;    
}

int cheio(stack *s) {
    return s->topo == TUDO - 1;
}
void pop(stack *s, carta *c) {
    if (vazio(s)) {
        printf("baralho vazio\n");
        return;
    }
    *c = s->cartas[s->topo];
    s->topo--;
}

void push(stack *s, carta c) {
    if (cheio(s)) {
        printf("baralho cheio\n");
        return;
    }
    s->topo++;
    s->cartas[s->topo] = c;
}

// pra nao ler o enum e sim os nomes mesmo
const char* nomeCor(cores c);
const char* nomePoder(poderes p);
const char* nomeCor(cores c) {
    switch (c) {
        case vermelho: return "vermelho";
        case verde: return "verde";
        case amarelo: return "amarelo";
        case azul: return "azul";
        case placeholder: return "placeholder";
        default: return "desconhecido";
    }
}

const char* nomePoder(poderes p) {
    switch (p) {
        case nenhum: return "nenhum";
        case bloqueio: return "bloqueio";
        case inversao: return "inversao";
        case trocacor: return "trocacor";
        case m2: return "+2";
        case m4: return "+4";
        default: return "desconhecido";
    }
}


void iniciaStack(stack *s) { //inicializa a stack
    s->topo = -1;
}

void iniciaJogador (jogador jogadores[], int indexJogadores) { //inicializa os jogadores
	for (int i = 0; i < indexJogadores; i++) {
		printf("nome:\n");
		scanf("%s", jogadores[i].nome);
		jogadores[i].id = i + 1;
		iniciaStack(&jogadores[i].mao);
	}
}

void distribuir(stack *baralho, jogador jogadores[], int indexJogadores) { //distribui as cartas pros jogadores
	for (int i = 0; i < indexJogadores; i++) {
		for (int j = 0; j < 7; j++) {
		carta c;
		pop(baralho, &c);
		push(&jogadores[i].mao, c);
		}
	}
}


void criarBaralho(stack *s) { // 76 normais e 32 c poder
    iniciaStack(s);
    
    //76 normais
    for (int cor = vermelho; cor <= azul; cor++) { //um 0 de cada cor
        carta carta_zero;
        carta_zero.tipo = normal;
        carta_zero.cor = cor;
        carta_zero.valor = zero;
        push(s, carta_zero);
        
        for (int i = um; i <= nove; i++) {
            for (int y = 0; y < 2; y++) { //2 cartas iguais de um a 9
                carta carta_normal;
                carta_normal.tipo = normal;
                carta_normal.cor = cor;
                carta_normal.valor = i;
                push(s, carta_normal);
            }
        }
    }
    
    //24 especiais c cor
    poderes especiais_com_cor[] = {bloqueio, inversao, m2};
    for (int cor = vermelho; cor <= azul; cor++) {
        for (int i = 0; i < 3; i++) {
            for (int y = 0; y < 2; y++) {
                carta carta_especial;
                carta_especial.tipo = especial;
                carta_especial.cor = cor;
                carta_especial.poder = especiais_com_cor[i];
                push(s, carta_especial);
            }
        }
    }
    
    for (int y = 0; y < 4; y++) { //4 +4
        carta carta_plus4;
        carta_plus4.tipo = curinga;
        carta_plus4.cor = placeholder;
        carta_plus4.poder = m4;
        push(s, carta_plus4);
    }
    
    for (int y = 0; y < 4; y++) { //4 de trocar a cor
        carta carta_troca;
        carta_troca.tipo = curinga;
        carta_troca.cor = placeholder;
        carta_troca.poder = trocacor;
        push(s, carta_troca);
    }
}

void embaralhar(stack *s) { //embaralho c fisher yates
	srand(time(NULL));
	
	for (int i = s->topo; i > 0; i--) {
		int j = rand() % (i + 1);
		carta temp = s->cartas[i];
		s->cartas[i] = s->cartas[j];
		s->cartas[j] = temp;
	}
}

void compraCarta (jogador *j, stack *baralho, stack *mesa, carta cartaMesa) {
    if (!vazio(baralho)) {
        carta c;
        pop(baralho, &c);
        push (&j->mao, c);
        printf("%s comprou\n", j->nome);
    } else {
        printf("baralho vazio reciclando\n");
		for (int i = 0; i <= mesa->topo; i++) { 
		push(baralho, mesa->cartas[i]); 
		}
        embaralhar(baralho);
        iniciaStack(mesa);
        push(mesa, cartaMesa);
        if (!vazio(baralho)) {
            carta c;
            pop(baralho, &c);
            push (&j->mao, c);
            printf("%s comprou\n", j->nome);
        } else {
            printf("nao ha cartas para reciclar. %s nao comprou.\n", j->nome);
        }
    }
}
void removeCartaMao (jogador *j, int index) {
	if (index < 0 || index > j->mao.topo) return;
    for (int i = index; i < j->mao.topo; i++) {
        j->mao.cartas[i] = j->mao.cartas[i + 1];
    }
    j->mao.topo--;
}

void jogarCarta (jogador *j, stack *mesa, carta jogada, int index) { //precisa de uma funcao pra remover uma carta especifica da mao
	push (mesa, jogada);
	removeCartaMao(j, index);
}

int jogadaValida (carta mesa, carta jogada) {
	if (jogada.tipo == curinga) return 1;  // curingas sempre validos
	if (jogada.cor == mesa.cor) return 1;  // mesma cor
	if (jogada.tipo == normal && mesa.tipo == normal && jogada.valor == mesa.valor) return 1;  //normais tem mesmo valor
	return 0;
}


int main() {
	
	stack baralho;
	criarBaralho (&baralho);
	embaralhar(&baralho);
	
	int indexJogadores = 0;
	do {
		printf("quantos jogadores vao jogar (minimo 2)\n");
		scanf("%d", &indexJogadores);
		if (indexJogadores < 2) {
			printf("nao se joga uno sozinho digita dnv\n");
		}
	} while (indexJogadores < 2);
	
	jogador jogadores[indexJogadores];
	iniciaJogador(jogadores, indexJogadores);
	distribuir(&baralho, jogadores, indexJogadores);
	
	int atualJogador = 0;
	int direcao = 1; // fazer assim e usar -1 pra inversao
	
 	carta cartaMesa;
 	
 	do {
 		pop(&baralho, &cartaMesa);
 		if (cartaMesa.tipo != normal) {
 			push (&baralho, cartaMesa);
 			embaralhar (&baralho);
 			printf("carta especial retirada embaralhando\n");
		 }
	 } while (cartaMesa.tipo != normal);
	 printf("carta inicial: cor %s, valor %d\n", nomeCor(cartaMesa.cor), cartaMesa.valor);
 	
 	int jogo_terminado = 0;
	stack mesa;
	iniciaStack(&mesa);

	int punicao_acumulada = 0;

	while (!jogo_terminado) {
	    jogador *atual = &jogadores[atualJogador];
	    if (punicao_acumulada > 0) {
	        printf("\npenalidade pendente: %d cartas pra comprar\n", punicao_acumulada);
	        printf("vez de %s (ID %d). carta na mesa: ", atual->nome, atual->id);
	        if (cartaMesa.tipo == normal) {
	            printf("normal, cor %s, valor %d\n", nomeCor(cartaMesa.cor), cartaMesa.valor);
	        } else {
	            printf("especial, poder %s\n", nomePoder(cartaMesa.poder));
	        }
	        
	        printf("sua mao:\n");
	        for (int i = 0; i <= atual->mao.topo; i++) {
	            carta c = atual->mao.cartas[i];
	            printf("%d: ", i);
	            if (c.tipo == normal) {
	                printf("normal, cor %s, valor %d\n", nomeCor(c.cor), c.valor);
	            } else if (c.tipo == especial) {
	                printf("especial, cor %s, poder %s\n", nomeCor(c.cor), nomePoder(c.poder));
	            } else {
	                printf("curinga, poder %s\n", nomePoder(c.poder));
	            }
	        }
	        
	        int escolha;
	        printf("escolha o indice de uma carta +2 ou +4 para jogar e acumular (ou -1 para comprar %d cartas): ", punicao_acumulada);
	        scanf("%d", &escolha);
	        
	        if (escolha == -1) {
	            for (int k = 0; k < punicao_acumulada; k++) {
	                compraCarta(atual, &baralho, &mesa, cartaMesa);
	            }
	            printf("%s comprou %d cartas e perdeu o turno!\n", atual->nome, punicao_acumulada);
	            punicao_acumulada = 0;
	            atualJogador = (atualJogador + direcao + indexJogadores) % indexJogadores;
	            continue;
	        } else if (escolha >= 0 && escolha <= atual->mao.topo) {
	            carta jogada = atual->mao.cartas[escolha];
	            if ((jogada.poder == m2 || jogada.poder == m4) && jogadaValida(cartaMesa, jogada)) {
	                jogarCarta(atual, &mesa, jogada, escolha);
	                cartaMesa = jogada;
	                printf("carta jogada para acumular penalidade!\n");
	                
	                if (jogada.poder == m2) {
	                    punicao_acumulada += 2;
	                    printf("penalidade acumulada agora: %d cartas.\n", punicao_acumulada);
	                } else if (jogada.poder == m4) {
	                    int cor_escolhida;
	                    printf("escolha a cor nova (1-vermelho, 2-verde, 3-amarelo, 4-azul): ");
	                    scanf("%d", &cor_escolhida);
	                    if (cor_escolhida >= 1 && cor_escolhida <= 4) {
	                        cartaMesa.cor = cor_escolhida;
	                        printf("+4 jogado! cor mudou para %s.\n", nomeCor(cartaMesa.cor));
	                    } else {
	                        printf("Cor invalida, cor permanece.\n");
	                    }
	                    punicao_acumulada += 4;
	                    printf("penalidade acumulada agora: %d cartas.\n", punicao_acumulada);
	                }
	                atualJogador = (atualJogador + direcao + indexJogadores) % indexJogadores;
	                continue;
	            } else {
	                printf("carta invalida para acumular penalidade (deve ser +2 ou +4 valida).\n");
	                continue;
	            }
	        } else {
	            printf("escolha invalida.\n");
	            continue;
	        }
	    }
	
	    printf("\nvez de %s (ID %d). carta na mesa: ", atual->nome, atual->id);
    if (cartaMesa.tipo == normal) {
        printf("normal, cor %s, valor %d\n", nomeCor(cartaMesa.cor), cartaMesa.valor);
    } else {
        printf("especial, poder %s\n", nomePoder(cartaMesa.poder));
    }
    
    printf("sua mao:\n");
    for (int i = 0; i <= atual->mao.topo; i++) {
        carta c = atual->mao.cartas[i];
        printf("%d: ", i);
        if (c.tipo == normal) {
            printf("normal, cor %s, valor %d\n", nomeCor(c.cor), c.valor);
        } else if (c.tipo == especial) {
            printf("especial, cor %s, poder %s\n", nomeCor(c.cor), nomePoder(c.poder));
        } else {
            printf("curinga, poder %s\n", nomePoder(c.poder));
        }
    }
    
    int tem_carta_valida = 0;
    for (int i = 0; i <= atual->mao.topo; i++) {
        if (jogadaValida(cartaMesa, atual->mao.cartas[i])) {
            tem_carta_valida = 1;
            break;
        }
    }
    
    if (!tem_carta_valida) {
        printf("Nenhuma carta valida para jogar! Comprando uma carta automaticamente.\n");
        compraCarta(atual, &baralho, &mesa, cartaMesa);
        atualJogador = (atualJogador + direcao + indexJogadores) % indexJogadores;
        continue;
    }
    
    int escolha;
    printf("escolha o indice da carta para jogar (0-%d) ou -1 para comprar: ", atual->mao.topo);
    scanf("%d", &escolha);
    
    if (escolha == -1) {
        compraCarta(atual, &baralho, &mesa, cartaMesa);
    } else if (escolha >= 0 && escolha <= atual->mao.topo) {
        carta jogada = atual->mao.cartas[escolha];
        
        if (jogadaValida(cartaMesa, jogada)) {
            jogarCarta(atual, &mesa, jogada, escolha);
            cartaMesa = jogada;
            printf("carta jogada\n");
    
            if (vazio(&atual->mao)) {
                if (jogada.tipo == normal) {
                    printf("%s venceu!\n", atual->nome);
                    jogo_terminado = 1;
                } else {
                    printf("voce nao pode vencer com uma carta especial ou curinga! compre uma carta.\n");
                    compraCarta(atual, &baralho, &mesa, cartaMesa);
                    continue;
                }
            }
            
            if (jogada.tipo == especial || jogada.tipo == curinga) {
                if (jogada.poder == m2) {
                    punicao_acumulada += 2;
                    printf("+2 jogado! penalidade acumulada: %d cartas para o proximo.\n", punicao_acumulada);
                    
                } else if (jogada.poder == m4) {
                    int cor_escolhida;
                    printf("escolha a cor nova (1-vermelho, 2-verde, 3-amarelo, 4-azul): ");
                    scanf("%d", &cor_escolhida);
                    
                    if (cor_escolhida >= 1 && cor_escolhida <= 4) {
                        cartaMesa.cor = cor_escolhida;
                        printf("+4 jogado! cor mudou para %s.\n", nomeCor(cartaMesa.cor));
                    } else {
                        printf("cor invalida, cor permanece.\n");
                    }
                    punicao_acumulada += 4;
                    printf("penalidade acumulada: %d cartas para o proximo.\n", punicao_acumulada);
                    
                } else if (jogada.poder == bloqueio) {
                    printf("bloqueio jogado! proximo perde o turno.\n");
                    atualJogador = (atualJogador + direcao + indexJogadores) % indexJogadores;
                    continue;
                    
                } else if (jogada.poder == inversao) {
                    direcao = -direcao;
                    printf("inversao jogada! direcao do jogo invertida.\n");
                    
                } else if (jogada.poder == trocacor) {
                    int cor_escolhida;
                    printf("escolha a cor nova (1-vermelho, 2-verde, 3-amarelo, 4-azul): ");
                    scanf("%d", &cor_escolhida);
                    
                    if (cor_escolhida >= 1 && cor_escolhida <= 4) {
                        cartaMesa.cor = cor_escolhida;
                        printf("troca cor jogada! cor mudou para %s.\n", nomeCor(cartaMesa.cor));
                    } else {
                        printf("cor invalida, cor permanece.\n");
                    }
                }
            }
        } else {
            printf("carta invalida\n");
            continue;  
        }
    } else {
        printf("escolha invalida.\n");
        continue;
    }
    
    atualJogador = (atualJogador + direcao + indexJogadores) % indexJogadores;	    
	}
	return 0;
}
