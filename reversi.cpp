//// Universidade de São Paulo
//// Instituto de Física de São Carlos
//// *Programação Paralela*
//// Prof. Gonzalo Travieso
//// Aluno Vilson Vieira

//// Implementação do Jogo Reversi (em C++) ///////////////////////////////////

//// Instalação e uso /////////////////////////////////////////////////////////

// Para instalar e executar, basta possuir um compilar C++. Aqui
// utilizamos GNU C++ Compiler.
//
// O uso deste *programa* se dá através da chamada à função *joga(n)*,
// onde *n* é o nível máximo que o algoritmo *minimax* deverá
// avaliar. Esta está sendo chamada na função *main()*.
//
// Portanto, para realizar um jogo, basta executar, em linha de
// comando, a seguinte chamada:
//
//     $ g++ -o reversi reversi.cpp
//     $ ./reversi > jogadas.txt
//
// Todas as jogadas estarão no arquivo jogadas.txt

//// Bibliotecas necessárias //////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

//// Constantes e estruturas //////////////////////////////////////////////////

// O caractere (e inteiro) 0 é utilizado para representar as peças
// pretas.
char PRETO = '0';
// O caractere 1 para as brancas.
char BRANCO = '1';
// O traço para posições vazias do tabuleiro.
char VAZIO = '-';
// O asterísco para as bordas (desta forma, temos uma matriz de 10x10
// posições).
char BORDA = '*';

// Portanto, considerando uma representação simples baseada em uma
// lista de listas de caracteres, o tabuleiro é representado da
// seguinte forma:
//
//     * * * * * * * * * *
//     * - - - - - - - - *
//     * - - - - - - - - *
//     * - - - - - - - - *
//     * - - - 1 0 - - - *
//     * - - - 0 1 - - - *
//     * - - - - - - - - *
//     * - - - - - - - - *
//     * - - - - - - - - *
//     * * * * * * * * * *

// Uma lista das direções possíveis que um 'traçado' pode
// assumir. Definimos como 'traçado' uma linha entre a peça atual e a
// próxima peça de mesma cor, passando por uma ou mais peças
// adversárias. Inverter (reversi!) as peças adversárias que fazem
// parte desse 'traçado' é o objetivo do jogo.  Portanto, aqui temos
// todas as 8 direções possíveis que a peça poderá 'caminhar' para
// tentar formar o 'traçado'.
int DIRS[][2] = {{-1, -1}, {-1, 0}, {-1, 1},
                 { 0, -1},          { 0, 1},
                 { 1, -1}, { 1, 0}, { 1, 1}};

// Definimos uma estrutura para facilitar armazenar as posições no
// tabuleiro.
struct Posicao {
  int linha;
  int coluna;
};

// Essa estrutura é utilizada pela função *planeja()* e *minimax()*,
// pois estas retornam (recursivamente, no caso de *minimax()*) o
// ganho da jogada e sua posição no tabuleiro.
struct GanhoPos {
  float ganho;
  Posicao *pos;
};

//// Assinaturas das funções utilizadas ///////////////////////////////////////

// Poderia ter incluído em um header, mas preferi deixar todo o código
// em um só arquivo.
void joga(int nivel, int tam_tabuleiro);
void mostra(char jogador, Posicao *jda, int qtd_jogadas, string **tabuleiro);
Posicao *planeja(char jogador, string **tabuleiro, int nivel);
GanhoPos minimax(char jogador, string **tabuleiro, int nivel);
int pontos(char jogador, string **tabuleiro);
vector<Posicao *> pos_validas(string **tabuleiro);
vector<Posicao *> pos_jogaveis(char jogador, string **tabuleiro);
bool pos_valida(Posicao *pos, char jogador, string **tabuleiro);
Posicao *pos_jogavel(Posicao *pos, char jogador, string **tabuleiro, int d[]);
string **executa(Posicao *pos, char jogador, string **tabuleiro);
void inverte(Posicao *pos, char jogador, string **tabuleiro, int d[]);
char proximo(char jogador, string **tabuleiro);

//// Função principal /////////////////////////////////////////////////////////

// Função principal que executa todos os turnos de um jogo, terminando
// quando não há mais possibilidade de movimento para os jogadores.
void joga(int nivel, int tam_tabuleiro) {
  // Iniciamos um novo tabuleiro de qualquer tamanho.
  string **tabuleiro = new string*[tam_tabuleiro+2];
  int i, j, qtd_jogadas;
  char jogador;
  Posicao *jogada;

  for (i=0; i<tam_tabuleiro+2; i++) {
    tabuleiro[i] = new string[tam_tabuleiro+2];
    string foo (tam_tabuleiro+2, VAZIO);
    *tabuleiro[i] = foo;
  }

  (*tabuleiro[tam_tabuleiro/2])[tam_tabuleiro/2] = BRANCO;
  (*tabuleiro[tam_tabuleiro/2+1])[tam_tabuleiro/2+1] = BRANCO;
  (*tabuleiro[tam_tabuleiro/2+1])[tam_tabuleiro/2] = PRETO;
  (*tabuleiro[tam_tabuleiro/2])[tam_tabuleiro/2+1] = PRETO;

  for (i=0; i<tam_tabuleiro+2; i++) {
    for (j=0; j<tam_tabuleiro+2; j++) {
      if ((i==0) || (i==tam_tabuleiro+1)) {
        (*tabuleiro[i])[j] = BORDA;
      }
      if ((j==0) || (j==tam_tabuleiro+1)) {
        (*tabuleiro[i])[j] = BORDA;
      }
    }
  }
  
  // Contamos as jogadas nesta variável.
  qtd_jogadas = 0;
  // O Reversi começa sempre pelas peças pretas.
  jogador = PRETO;
  // Mostramos o tabuleiro na tela antes de começar o jogo.
  // mostra(jogador, qtd_jogadas, tabuleiro);
  // Executa os turnos enquanto for possível. Esse é o *loop*
  // principal deste *script*.
  while (true) {
    // Planeja uma jogada (aqui será chamado o procedimento
    // *minimax*).
    jogada = planeja(jogador, tabuleiro, nivel);

    if ((jogada->linha != -1) && (jogada->coluna != -1)) {
      // Executa a jogada.    
      executa(jogada, jogador, tabuleiro);
      // Mostra na tela o estado do tabuleiro atual
      mostra(jogador, jogada, qtd_jogadas, tabuleiro);
    }
    // Troca de jogador (ou continua no mesmo se o outro passar a
    // vez).
    jogador = proximo(jogador, tabuleiro);
    // Verifica se o jogo terminou. O caractere '9' é usado como um
    // marcador, retornado pela função *proximo()* para sinalizar que
    // não há mais movimentos possíveis e, portanto, fim de jogo.
    if (jogador == '9') {
      // Verifica quem ganhou e mostra o resultado final do jogo.
      /*
      int dif_preto = pontos(PRETO, tabuleiro);
      int dif_branco = pontos(BRANCO, tabuleiro);
      cout << "O jogo terminou após " << qtd_jogadas+1 << " jogadas! ";
      if (dif_preto > dif_branco)
        cout << "Jogador PRETO ganhou com " << dif_preto << " peças a mais.";
      else if (dif_branco > dif_preto)
        cout << "Jogador BRANCO ganhou com " << dif_branco << " peças a mais.";      cout << endl;
      */
      break;
    }
    // Apenas atualizamos o contador de jogadas.
    qtd_jogadas++;
  }

}

//// Planejamento /////////////////////////////////////////////////////////////

// Nesta seção, a heurística *minimax* é utilizada para planejar uma
// jogada 'melhor possível' para um determinado jogador.
//
// O teorema *minimax* é bastante interessante quando tratamos de jogos
// baseados em turnos. De forma bastante prática, o que ela sugere é:
// *o melhor para o jogador atual é minimizar a sua perda*. Posto de
// outra maneira, o *jogador atual deverá escolher qual jogada sua
// levará a um estado onde perderá menos peças*.
//
// Por exemplo, digamos que é a vez do jogador PRETO. O algoritmo
// *minimax* irá avaliar todas as possíveis jogadas (até o nível máximo
// especificado) do oponente. Para permitir essa avaliação, contaremos
// a diferença de peças entre os jogadores. Digamos que essa diferença,
// para um determinado nível, seja equivalente a *[2, 1, 3]*. Ou seja,
// se a jogada 1 for escolhida, o oponente terá ganho 2 peças, se a
// jogada 2 for escolhida, o oponente ganhará 1 peça, o mesmo para a
// terceira escolha, onde ele ganhará 3 peças. Para o jogador PRETO é
// melhor escolher o valor mínimo, ou, invertendo os sinais: *max([-2,
// -1, -3]) = -1*. Dessa forma, a segunda opção de jogada é a melhor
// para o jogador PRETO.
//
// A função *minimax()* implementa esse algoritmo descrito acima. A
// implementação foi baseada [neste
// pseudo-código](http://en.wikipedia.org/wiki/Minimax) disponível na
// Wikipedia.

// Função auxiliar, que chama a função recursiva *minimax()*.
Posicao *planeja(char jogador, string **tabuleiro, int nivel) {
  static GanhoPos aux;

  aux = minimax(jogador, tabuleiro, nivel);
  return aux.pos;
}

// Planeja a próxima jogada de determinado jogador através do
// algoritmo *minimax*.
GanhoPos minimax(char jogador, string **tabuleiro, int nivel) {
  vector<Posicao *> jogaveis;
  vector<GanhoPos> ganhos;
  int i, j, ganho;
  GanhoPos aux, maior;
  Posicao *posNula = new Posicao;
  int oponente = (jogador + 1) % 2;
  bool fim;
  vector<Posicao *> validas;
  int tam_tabuleiro = (*tabuleiro[0]).size();
  string **copia_tabuleiro = new string*[tam_tabuleiro];

  // Critério de parada: se o nível for zero, retorna a diferença de
  // peças que o jogador possui com o oponente no tabuleiro.  Note
  // que o retorno dessa função é sempre o ganho da jogada e a jogada
  // em si (sua posição no tabuleiro).
  if (nivel == 0) {
    aux.ganho = pontos(jogador, tabuleiro);
    posNula->linha = -1;
    posNula->coluna = -1;
    aux.pos = posNula;
    return aux;
  }

  // Vamos executar e analisar todas as jogadas possíveis desse
  // nível. Portanto, encontramos todas as posições possíveis de
  // jogada.
  jogaveis = pos_jogaveis(jogador, tabuleiro);

  // Se não existem jogadas possíveis ou o jogo terminou ou passamos
  // a vez (chamamos minimax para o oponente)
  if (jogaveis.size() == 0) {
    fim = false;
    
    for (i=0; i<validas.size(); i++) {
      if (pos_valida(validas.at(i), '0' + oponente, tabuleiro)) {
        fim = true;
      }
    }
    // Não existem mais posições válidas, o jogo terminou,
    // então o ganho será o máximo ou o mínimo possível para o
    // jogador (forçamos isso com um valor muito grande), ou 0
    // caso não há diferença nos pontos.
    if (fim) {
      ganho = pontos(jogador, tabuleiro);
      if (ganho < 0) {
        aux.ganho = -999999;
        posNula->linha = -1;
        posNula->coluna = -1;
        aux.pos = posNula;
        return aux;
      } else if (ganho > 0) {
        aux.ganho = 999999;
        posNula->linha = -1;
        posNula->coluna = -1;
        aux.pos = posNula;
        return aux;
      } else {
        aux.ganho = ganho;
        posNula->linha = -1;
        posNula->coluna = -1;
        aux.pos = posNula;
        return aux;
      }
    }

    // Caso contrário, passamos a vez para o oponente, chamando
    // minimax para ele.
    aux.ganho = -minimax('0' + oponente, tabuleiro, nivel-1).ganho;
    posNula->linha = -1;
    posNula->coluna = -1;
    aux.pos = posNula;
    return aux;
  }

  // Escolhemos o melhor valor possível das jogadas, ou seja, o
  // melhor para o jogador é o menor ganho do oponente, como
  // discutido acima. É aqui que é feita a chamada recursiva, sempre
  // invertendo os sinais dos valores dos tabuleiros e o jogador.
  for (i=0; i<jogaveis.size(); i++) {
    for (j=0; j<tam_tabuleiro; j++) {
      copia_tabuleiro[j] = new string[tam_tabuleiro];
      *copia_tabuleiro[j] = *tabuleiro[j];
    }

    ganho = -minimax('0' + oponente,
                     executa(jogaveis.at(i), jogador, copia_tabuleiro),
                     nivel-1).ganho;
    aux.ganho = ganho;
    aux.pos = jogaveis.at(i);

    ganhos.push_back(aux);
  }

  // Encontramos o maior (máximo).

  maior.ganho = -9999999;
  for (i=ganhos.size()-1; i>=0; i--) {
    if (ganhos.at(i).ganho > maior.ganho) {
      maior.ganho = ganhos.at(i).ganho;
      maior.pos = ganhos.at(i).pos;
    }
  }

  // Retornamos o melhor valor possível de todas as jogadas do nível
  // atual até 0.

  return maior;
}

// Calcula a pontuação do jogador baseando-se em seu total de peças
// menos as peças do oponente.
int pontos(char jogador, string **tabuleiro) {
  // Encontramos a quantidade de peças do jogador e seu oponente,
  // armazenando nessas variáveis.
  int pontos_jogador = 0;
  int pontos_oponente = 0;
  vector<Posicao *> validas;
  int oponente = (jogador + 1) % 2;
  int i;
  Posicao *pos;
  char peca;

  validas = pos_validas(tabuleiro);

  // Percorremos todas as posições válidas do tabuleiro e somamos 1
  // quando encontramos uma peça do jogador ou do oponente (0 ou 1).
  for (i=0; i<validas.size(); i++) {
    pos = validas.at(i);
    peca = (*tabuleiro[pos->linha])[pos->coluna];

    if (peca == jogador) {
      pontos_jogador++;
    } else if (peca == '0' + oponente) {
      pontos_oponente++;
    }
  }

  // Retornamos a diferença de peças entre os jogadores.
  return pontos_jogador - pontos_oponente;
}

//// Procura de jogadas válidas ///////////////////////////////////////////////

// Nessa seção temos várias funções necessárias para encontrarmos e
// validarmos posições do tabuleiro. Juntamente com as funções de
// execução de jogadas da próxima seção, elas modelam a *dinâmica do
// jogo*.

// Retorna as casas válidas do tabuleiro (sem as bordas).
vector<Posicao *> pos_validas(string **tabuleiro) {
  vector<Posicao *> v;
  int i, j;
  Posicao *pos;

  // Percorremos todo o tabuleiro e consideramos apenas as posições
  // que tenham peças ou que sejam vazias. Desconsideramos assim as
  // bordas.
  for (i=0; i<(*tabuleiro[0]).length(); i++) {
    for (j=0; j<(*tabuleiro[0]).length(); j++) {
      if (((*tabuleiro[i])[j] == VAZIO) || 
          ((*tabuleiro[i])[j] == PRETO) || 
          ((*tabuleiro[i])[j] == BRANCO)) {
        pos = new Posicao;
        pos->linha = i;
        pos->coluna = j;
        v.push_back(pos);
      }
    }
  }

  return v;
}

// Retorna as posições 'jogáveis'.
vector<Posicao *> pos_jogaveis(char jogador, string **tabuleiro) {
  vector<Posicao *> v;
  vector<Posicao *> validas;
  int i;
  Posicao *pos;

  // Para uma posição ser válida, ela precisa não ser borda e
  // corresponder a uma posição onde podemos fazer um
  // 'traçado'. Portanto, percorremos todas as posições válidas
  // (selecionadas pela função acima descrita) e procuraremos a
  // partir de cada uma dessas posições, uma posição em que possamos
  // jogar uma peça.

  validas = pos_validas(tabuleiro);
  
  for (i=0; i<validas.size(); i++) {
    pos = validas.at(i);
    pos_valida(pos, jogador, tabuleiro);

    if (pos_valida(pos, jogador, tabuleiro)) {
      v.push_back(pos);
    }
  }
  
  return v;
}

// Testa se a posição é válida ou não. O jogador só pode jogar em uma
// posição que forme um traçado válido!
bool pos_valida(Posicao *pos, char jogador, string **tabuleiro) {
  int i;
  Posicao *pos_v;

  // Para cada direção a partir da posição atual, procuramos uma
  // posição para jogar que forme um 'traçado'. Somente podemos jogar
  // nessa posição se ela estiver vazia, portanto, também fazemos
  // essa checagem.
  for (i=0; i<8; i++) {
    pos_v = pos_jogavel(pos, jogador, tabuleiro, DIRS[i]);
    
    if ((*tabuleiro[pos->linha])[pos->coluna] == VAZIO) {
      if (pos_v->linha != 42) {
        return true;
      }
    }
  }
  
  return false;
}

// Retorna uma posição onde podemos jogar a peça (há um traçado até
// ela) ou *None* caso contrário.
Posicao *pos_jogavel(Posicao *pos, char jogador, string **tabuleiro, int d[]) {
  Posicao *jogavel = new Posicao;
  int oponente = (jogador + 1) % 2;

  // Começamos pelo vizinho do canto superior esquerdo.
  jogavel->linha = pos->linha + d[0];
  jogavel->coluna = pos->coluna + d[1];

  // Se a próxima posição a partir da atual é uma de nossas peças,
  // ela não é uma posição válida para jogarmos. Apenas posições
  // vizinhas que tenham uma peça oponente nos interessam.
  //cout << jogavel->linha << "," << jogavel->coluna << endl;
  if ((*tabuleiro[jogavel->linha])[jogavel->coluna] == jogador) {
    // Usamos 42 como marcador ;-)
    jogavel->linha = 42;
    jogavel->coluna = 42;
    return jogavel;
  }

  // Vamos seguindo as posições onde há oponentes, a última delas é a
  // válida!
  while ((*tabuleiro[jogavel->linha])[jogavel->coluna] == '0' + oponente) {
    jogavel->linha += d[0];
    jogavel->coluna += d[1];
  }

  // Encontramos a posição válida em *jogavel*! Porém, se essa
  // posição for uma borda, não poderemos jogar.
  if (((*tabuleiro[jogavel->linha])[jogavel->coluna] == BORDA) ||
      ((*tabuleiro[jogavel->linha])[jogavel->coluna] == VAZIO)) {
    jogavel->linha = 42;
    jogavel->coluna = 42;
    return jogavel;
  } else {
    return jogavel;
  }
}

//// Execução de jogadas //////////////////////////////////////////////////////

// Cada vez que o jogador executa uma jogada, devemos atualizar o
// tabuleiro considerando essa nova jogada e ao mesmo tempo, inverter
// todas as peças adversárias que fazem parte do 'traçado'. Nessa seção
// descrevemos as duas funções responsáveis por isso: *executa()* e
// *inverte()*.

// Executa a jogada na posição especificada.
string **executa(Posicao *pos, char jogador, string **tabuleiro) {
  int i;

  // Colocamos a peça da jogada atual no tabuleiro.
  (*tabuleiro[pos->linha])[pos->coluna] = jogador;

  // Atualizamos todas as direções possíveis a partir dessa peça,
  // virando as peças adversárias.
  for (i=0; i<8; i++) {
    inverte(pos, jogador, tabuleiro, DIRS[i]);
  }

  // Retornamos o tabuleiro atualizado (é importante notar que
  // nenhuma das funções -- a não ser a *inverte()*, que é uma função
  // apenas chamada por *executa* -- têm *side-effects*, ou seja, os
  // estados passados por parâmetro não são alterados. Pensamos assim
  // para facilitar paralelismo em seguida (talvez aplicando dividir
  // para conquistar?).
  return tabuleiro;
}

// Inverte todas as peças adversárias em um determinada direção.
void inverte(Posicao *pos, char jogador, string **tabuleiro, int d[]) {
  Posicao *pos_tracado = new Posicao;

  // Usamos a mesma função que utilizamos anteriormente para
  // encontrar uma posição jogável, mas agora a usamos para encontrar
  // o final do 'traçado'.
  Posicao *pos_final = pos_jogavel(pos, jogador, tabuleiro, d);

  // Se não existe uma posição, simplesmente retornamos sem fazer
  // nada. Importante notar que usamos o número 42 para marcar uma
  // posição nula, visto que C++ não permite tipagem dinâmica.
  if (pos_final->linha == 42) {
    return;
  }

  // Vamos percorrer todas as peças do traçado, tornando-as todas
  // nossas.
  pos_tracado->linha = pos->linha + d[0];
  pos_tracado->coluna = pos->coluna + d[1];

  while ((pos_tracado->linha != pos_final->linha) ||
         (pos_tracado->coluna != pos_final->coluna)) {
    (*tabuleiro[pos_tracado->linha])[pos_tracado->coluna] = jogador;
    pos_tracado->linha += d[0];
    pos_tracado->coluna += d[1];
  }
}


//// Critério de parada ///////////////////////////////////////////////////////

// Testa se o jogo terminou. O jogo termina quando nenhum jogador mais
// puder se mover. Se houver um jogador livre para jogar, ele irá
// jogar, caso contrário, retorna None sinalizando que o jogo
// terminou.
char proximo(char jogador, string **tabuleiro) {
  int oponente = (jogador + 1) % 2;
  vector<Posicao *> validas = pos_validas(tabuleiro);
  int i;

  for (i=0; i<validas.size(); i++) {
    if (pos_valida(validas.at(i), oponente, tabuleiro)) {
      // Se o oponente pode se mover, ele joga.
      return '0' + oponente;
    } else if (pos_valida(validas.at(i), jogador, tabuleiro)) {
      // Senão, o jogador atual joga (o oponente passou a vez pois
      // está 'preso').
      return jogador;
    }
  }
  // Caso contrário, todos os jogadores estarão 'presos' e o jogo
  // então termina. Sinalizamos, como já comentado, através do
  // caractere '9'.
  return '9';
}

//// Funções auxiliares ///////////////////////////////////////////////////////

// Mostra na tela o número do turno atual, o jogador e sua jogada,
// formatadas de uma maneira 'amigável'.
void mostra(char jogador, Posicao *jda, int qtd_jogadas, string **tabuleiro) {
  string s = "";
  string cor = "black";
  int tam_tabuleiro = (*tabuleiro[0]).size();
  int i;

  for (i=1; i<tam_tabuleiro-1; i++) {
    s += (*tabuleiro[i]).substr(1, tam_tabuleiro-2) + "\n";
  }

  if (jogador == '1')
    cor = "white";

  cout << cor << 
    " " << 
    tam_tabuleiro-2-jda->linha << 
    " " << 
    jda->coluna-1 << endl; 

  // Descomente essas linhas para mostrar também o tabuleiro
  // cout << "#" << qtd_jogadas << " Jogador: " << cor << ". Jogada:\n\n" << s;
  // cout << endl;
}

// Fazemos uma chamada padrão à função joga, como exemplo. 
int main() {
  // Lemos o tamanho do tabuleiro e a quantidade máxima de níveis de um
  // arquivo de configuração.
  ifstream conf_file("reversi.conf");
  int nivel, tam_tabuleiro;
  conf_file >> tam_tabuleiro;
  conf_file >> nivel;

  joga(nivel, tam_tabuleiro);

  return 0;
}
