# -*- coding: utf-8 -*-

### Universidade de São Paulo
### Instituto de Física de São Carlos
### *Programação Paralela*
#### Prof. Gonzalo Travieso
##### Aluno Vilson Vieira

## Implementação do Jogo Reversi

# Segue a listagem de código (ao lado direito) acompanhado da
# documentação (ao lado esquerdo, nessas linhas seguintes) do jogo de
# tabuleiro *Reversi* em linguagem *Python*.

### Instalação e uso

# Para instalar e executar, basta possuir um interpretador
# *Python*. Aqui utilizamos a versão *2.7*.
#
# O uso deste *script* se dá através da chamada à função *joga(n)*,
# onde *n* é o nível máximo que o algoritmo *minimax* deverá
# avaliar. Para facilitar, fizemos a chamada dessa função ao final
# desse *script* para *n = 3*.
#
# Portanto, para realizar um jogo, basta executar, em linha de
# comando, a seguinte chamada:
#
#     $ python reversi.py > jogadas.txt
#
# Todas as jogadas estarão no arquivo jogadas.txt

### Bibliotecas necessárias

# Usada para a função *copy.deepcopy()*, que permite a cópia de
# estruturas de dados como matrizes, usada na representação do
# tabuleiro.
import copy

### Constantes e inicialização

# O caractere (e inteiro) 0 é utilizado para representar as peças
# pretas.
PRETO = '0'
# O caractere 1 para as brancas.
BRANCO = '1'
# O traço para posições vazias do tabuleiro.
VAZIO = '-'
# O asterísco para as bordas (desta forma, temos uma matriz de 10x10
# posições).
BORDA = '*'

# Portanto, considerando uma representação simples baseada em uma
# lista de listas de caracteres, o tabuleiro é representado da
# seguinte forma:
#
#     * * * * * * * * * *
#     * - - - - - - - - *
#     * - - - - - - - - *
#     * - - - - - - - - *
#     * - - - 1 0 - - - *
#     * - - - 0 1 - - - *
#     * - - - - - - - - *
#     * - - - - - - - - *
#     * - - - - - - - - *
#     * * * * * * * * * *

# Uma lista das direções possíveis que um 'traçado' pode
# assumir. Definimos como 'traçado' uma linha entre a peça atual e a
# próxima peça de mesma cor, passando por uma ou mais peças
# adversárias. Inverter (reversi!) as peças adversárias que fazem
# parte desse 'traçado' é o objetivo do jogo.  Portanto, aqui temos
# todas as 8 direções possíveis que a peça poderá 'caminhar' para
# tentar formar o 'traçado'.
DIRS = [(-1, -1), (-1, 0), (-1, 1),
        ( 0, -1),          ( 0, 1),
        ( 1, -1), ( 1, 0), ( 1, 1)]

def novo_tabuleiro():
    """
    Cria um tabuleiro inicial, com as primeiras peças no centro.
    """
    # Usamos a função *str.split()* para separar cada caractere da
    # string, formando uma lista de listas de caracteres (para
    # permitir edição posterior).
    return map(str.split, ["* * * * * * * * * *",
                           "* - - - - - - - - *",
                           "* - - - - - - - - *",
                           "* - - - - - - - - *",
                           "* - - - 1 0 - - - *",
                           "* - - - 0 1 - - - *",
                           "* - - - - - - - - *",
                           "* - - - - - - - - *",
                           "* - - - - - - - - *",
                           "* * * * * * * * * *"])

### Função principal

def joga(nivel):
    """
    Função principal que executa todos os turnos de um jogo,
    terminando quando não há mais possibilidade de movimento para os
    jogadores.
    """
    # Iniciamos um novo tabuleiro.
    tabuleiro = novo_tabuleiro()

    # Contamos as jogadas nesta variável.
    qtd_jogadas = 0
    # O Reversi começa sempre pelas peças pretas.
    jogador = PRETO
    # Mostramos o tabuleiro na tela antes de começar o jogo.
    mostra(jogador, qtd_jogadas-1, tabuleiro)
    # Executa os turnos enquanto for possível. Esse é o *loop*
    # principal deste *script*.
    while True:
        # Planeja uma jogada (aqui será chamado o procedimento
        # *minimax*).
        ganho, jogada = planeja(jogador, tabuleiro, nivel)
        # Executa a jogada.
        executa(jogada, jogador, tabuleiro)
        # Mostra na tela o estado do tabuleiro atual
        mostra(jogador, qtd_jogadas, tabuleiro)
        # Troca de jogador (ou continua no mesmo se o outro passar a
        # vez).
        jogador = proximo(jogador, tabuleiro)
        # Verifica se o jogo terminou. *None* é usado como um
        # marcador, retornado pela função *proximo()* para sinalizar
        # que não há mais movimentos possíveis e, portanto, fim de
        # jogo.
        if jogador is None:
            # Verifica quem ganhou.
            dif_preto = pontos(PRETO, tabuleiro)
            dif_branco = pontos(BRANCO, tabuleiro)
            res = "Houve EMPATE."
            if dif_preto > dif_branco:
                res = 'Jogador PRETO ganhou com %s peças a mais.' % dif_preto
            elif dif_branco > dif_preto:
                res = 'Jogador BRANCO ganhou com %s peças a mais.' % dif_branco

            # Mostra o resultado final do jogo.
            print "O jogo terminou após %s jogadas! %s" % (qtd_jogadas+1, res)
            break
        # Apenas atualizamos o contador de jogadas.
        qtd_jogadas += 1

### Planejamento

# Nesta seção, a heurística *minimax* é utilizada para planejar uma
# jogada 'melhor possível' para um determinado jogador.
#
# O teorema *minimax* é bastante interessante quando tratamos de jogos
# baseados em turnos. De forma bastante prática, o que ela sugere é:
# *o melhor para o jogador atual é minimizar a sua perda*. Posto de
# outra maneira, o *jogador atual deverá escolher qual jogada sua
# levará a um estado onde perderá menos peças*.
#
# Por exemplo, digamos que é a vez do jogador PRETO. O algoritmo
# *minimax* irá avaliar todas as possíveis jogadas (até o nível máximo
# especificado) do oponente. Para permitir essa avaliação, contaremos
# a diferença de peças entre os jogadores. Digamos que essa diferença,
# para um determinado nível, seja equivalente a *[2, 1, 3]*. Ou seja,
# se a jogada 1 for escolhida, o oponente terá ganho 2 peças, se a
# jogada 2 for escolhida, o oponente ganhará 1 peça, o mesmo para a
# terceira escolha, onde ele ganhará 3 peças. Para o jogador PRETO é
# melhor escolher o valor mínimo, ou, invertendo os sinais: *max([-2,
# -1, -3]) = -1*. Dessa forma, a segunda opção de jogada é a melhor
# para o jogador PRETO.
#
# A função *minimax()* implementa esse algoritmo descrito acima. A
# implementação foi baseada [neste
# pseudo-código](http://en.wikipedia.org/wiki/Minimax) disponível na
# Wikipedia.

def planeja(jogador, tabuleiro, nivel):
    """
    Função auxiliar, que chama a função recursiva *minimax()*.
    """
    return minimax(jogador, copy.deepcopy(tabuleiro), nivel)

def minimax(jogador, tabuleiro, nivel):
    """
    Planeja a próxima jogada de determinado jogador através do
    algoritmo *minimax*.
    """
    # Critério de parada: se o nível for zero, retorna a diferença de
    # peças que o jogador possui com o oponente no tabuleiro.  Note
    # que o retorno dessa função é sempre o ganho da jogada e a jogada
    # em si (sua posição no tabuleiro).
    if nivel == 0:
        p = pontos(jogador, tabuleiro)
        return (p, None)

    # Vamos executar e analisar todas as jogadas possíveis desse
    # nível. Portanto, encontramos todas as posições possíveis de
    # jogada.
    jogaveis = pos_jogaveis(jogador, tabuleiro)

    # Se não existem jogadas possíveis ou o jogo terminou ou passamos
    # a vez (chamamos minimax para o oponente)
    if jogaveis == []:
        oponente = (int(jogador) + 1) % 2
        if not any(pos_valida(pos, str(oponente), tabuleiro)
                   for pos in pos_validas(tabuleiro)):
            # Não existem mais posições válidas, o jogo terminou,
            # então o ganho será o máximo ou o mínimo possível para o
            # jogador (forçamos isso com um valor muito grande), ou 0
            # caso não há diferença nos pontos.
            ganho = pontos(jogador, tabuleiro)
            if ganho < 0:
                return (-999999, None)
            elif ganho > 0:
                return (999999, None)
            else:
                return (d, None)
        # Caso contrário, passamos a vez para o oponente, chamando
        # minimax para ele.
        return (-minimax(str((int(jogador) + 1) % 2),
                         tabuleiro,
                         nivel - 1)[0], None)

    # Escolhemos o melhor valor possível das jogadas, ou seja, o
    # melhor para o jogador é o menor ganho do oponente, como
    # discutido acima. É aqui que é feita a chamada recursiva, sempre
    # invertendo os sinais dos valores dos tabuleiros e o jogador.
    ganhos = [(-minimax(str((int(jogador) + 1) % 2),
                        executa(pos, jogador, copy.deepcopy(tabuleiro)),
                        nivel - 1)[0],
               pos) for pos in jogaveis]

    # Retornamos o melhor valor possível de todas as jogadas do nível
    # atual até 0.
    return max(ganhos)

def pontos(jogador, tabuleiro):
    """
    Calcula a pontuação do jogador baseando-se em seu total de peças
    menos as peças do oponente.
    """
    # Encontramos a quantidade de peças do jogador e seu oponente,
    # armazenando nessas variáveis.
    pontos_jogador = 0
    pontos_oponente = 0
    oponente = (int(jogador) + 1) % 2;
    # Percorremos todas as posições válidas do tabuleiro e somamos 1
    # quando encontramos uma peça do jogador ou do oponente (0 ou 1).
    for pos in pos_validas(tabuleiro):
        peca = tabuleiro[pos[0]][pos[1]]
        if peca == str(jogador):
            pontos_jogador += 1
        elif peca == str(oponente):
            pontos_oponente += 1
    # Retornamos a diferença de peças entre os jogadores.
    return pontos_jogador - pontos_oponente

### Procura de jogadas válidas

# Nessa seção temos várias funções necessárias para encontrarmos e
# validarmos posições do tabuleiro. Juntamente com as funções de
# execução de jogadas da próxima seção, elas modelam a *dinâmica do
# jogo*.

def pos_validas(tabuleiro):
    """
    Retorna as casas válidas do tabuleiro (sem as bordas).
    """
    v = []
    # Percorremos todo o tabuleiro e consideramos apenas as posições
    # que tenham peças ou que sejam vazias. Desconsideramos assim as
    # bordas.
    for i in range(len(tabuleiro)):
        for j in range(len(tabuleiro)):
            if tabuleiro[i][j] in [VAZIO, PRETO, BRANCO]:
                v.append((i,j))
    return v

def pos_jogaveis(jogador, tabuleiro):
    """
    Retorna as posições 'jogáveis'.
    """
    # Para uma posição ser válida, ela precisa não ser borda e
    # corresponder a uma posição onde podemos fazer um
    # 'traçado'. Portanto, percorremos todas as posições válidas
    # (selecionadas pela função acima descrita) e procuraremos a
    # partir de cada uma dessas posições, uma posição em que possamos
    # jogar uma peça.
    return [pos for pos in pos_validas(tabuleiro)
            if pos_valida(pos, jogador, tabuleiro)]

def pos_valida(pos, jogador, tabuleiro):
    """
    Testa se a posição é válida ou não. O jogador só pode jogar em uma
    posição que forme um traçado válido!
    """
    # Função auxiliar que tenta encontrar uma posição para jogar em
    # determinada direção.
    def busca(dir):
        return pos_jogavel(pos, jogador, tabuleiro, dir)
    
    # Para cada direção a partir da posição atual, procuramos uma
    # posição para jogar que forme um 'traçado'. Somente podemos jogar
    # nessa posição se ela estiver vazia, portanto, também fazemos
    # essa checagem.
    if tabuleiro[pos[0]][pos[1]] == VAZIO:
        return any(map(busca, DIRS))
    return False

def pos_jogavel(pos, jogador, tabuleiro, direcao):
    """
    Retorna uma posição onde podemos jogar a peça (há um traçado até
    ela) ou *None* caso contrário.
    """
    # Começamos pelo vizinho do canto superior esquerdo.
    jogavel = [pos[0] + direcao[0], pos[1] + direcao[1]]

    # Se a próxima posição a partir da atual é uma de nossas peças,
    # ela não é uma posição válida para jogarmos. Apenas posições
    # vizinhas que tenham uma peça oponente nos interessam.
    if tabuleiro[jogavel[0]][jogavel[1]] == jogador:
        return None

    # Cálculo simples para sabermos quem é nosso oponente (utilizamos
    # esse cálculo várias vezes no código).
    oponente = (int(jogador) + 1) % 2;

    # Vamos seguindo as posições onde há oponentes, a última delas é a
    # válida!
    while tabuleiro[jogavel[0]][jogavel[1]] == str(oponente):
        jogavel[0] += direcao[0]
        jogavel[1] += direcao[1]

    # Encontramos a posição válida em *jogavel*! Porém, se essa
    # posição for uma borda, não poderemos jogar.
    if tabuleiro[jogavel[0]][jogavel[1]] in [BORDA, VAZIO]:
        return None
    else:
        return jogavel

### Execução de jogadas

# Cada vez que o jogador executa uma jogada, devemos atualizar o
# tabuleiro considerando essa nova jogada e ao mesmo tempo, inverter
# todas as peças adversárias que fazem parte do 'traçado'. Nessa seção
# descrevemos as duas funções responsáveis por isso: *executa()* e
# *inverte()*.
                
def executa(pos, jogador, tabuleiro):
    """
    Executa a jogada na posição especificada.
    """
    # Colocamos a peça da jogada atual no tabuleiro.
    tabuleiro[pos[0]][pos[1]] = jogador
    # Atualizamos todas as direções possíveis a partir dessa peça,
    # virando as peças adversárias.
    for direcao in DIRS:
        inverte(pos, jogador, tabuleiro, direcao)
    # Retornamos o tabuleiro atualizado (é importante notar que
    # nenhuma das funções -- a não ser a *inverte()*, que é uma função
    # apenas chamada por *executa* -- têm *side-effects*, ou seja, os
    # estados passados por parâmetro não são alterados. Pensamos assim
    # para facilitar paralelismo em seguida (talvez aplicando dividir
    # para conquistar?).
    return tabuleiro

def inverte(pos, jogador, tabuleiro, direcao):
    """
    Inverte todas as peças adversárias em um determinada direção.
    """
    # Usamos a mesma função que utilizamos anteriormente para
    # encontrar uma posição jogável, mas agora a usamos para encontrar
    # o final do 'traçado'.
    pos_final = pos_jogavel(pos, jogador, tabuleiro, direcao)

    # Se não existe uma posição, simplesmente retornamos sem fazer
    # nada.
    if not pos_final:
        return

    # Vamos percorrer todas as peças do traçado, tornando-as todas
    # nossas.
    pos_tracado = [pos[0] + direcao[0], pos[1] + direcao[1]]
    while pos_tracado != pos_final:
        tabuleiro[pos_tracado[0]][pos_tracado[1]] = jogador
        pos_tracado[0] += direcao[0]
        pos_tracado[1] += direcao[1]

### Critério de parada

def proximo(jogador, tabuleiro):
    """
    Testa se o jogo terminou. O jogo termina quando nenhum jogador
    mais puder se mover. Se houver um jogador livre para jogar, ele
    irá jogar, caso contrário, retorna None sinalizando que o jogo
    terminou.
    """
    oponente = (int(jogador) + 1) % 2;

    # Se o oponente pode se mover, ele joga.
    if any(pos_valida(pos, oponente, tabuleiro)
           for pos in pos_validas(tabuleiro)):
        return str(oponente)
    # Senão, o jogador atual joga (o oponente passou a vez pois está
    # 'preso').
    elif any(pos_valida(pos, jogador, tabuleiro)
             for pos in pos_validas(tabuleiro)):
        return str(jogador)
    # Caso contrário, todos os jogadores estarão 'presos' e o jogo
    # então termina. Sinalizamos, como já comentado, através de
    # *None*.
    return None

### Funções Auxiliares

def mostra(jogador, qtd_jogadas, tabuleiro):
    """
    Mostra na tela o número do turno atual, o jogador e sua jogada,
    formatadas de uma maneira 'amigável'.
    """
    s = ""
    for linha in range(1, 9):
        s += "%s\n" % " ".join(tabuleiro[linha][1:9])
        
    cor = 'PRETO'
    if jogador is '1':
        cor = 'BRANCO'
        
    print "#%s Jogador: %s. Jogada:\n\n%s" % (qtd_jogadas+1, cor, s)

# Fazemos uma chamada padrão à função joga, como exemplo. 
joga(3)
