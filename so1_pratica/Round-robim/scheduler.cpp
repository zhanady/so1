#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <queue>

using namespace std;

struct Processo {
    string id;
    int chegada;
    int cpu;

    int tempo_restante;

    int termino = -1;
    int primeira_execucao = -1;
    int retorno = 0;
    int espera = 0;
    int resposta = 0;
};

bool ignorarLinha(const string& linha) {
    for (char c : linha) {
        if (c == ' ' || c == '\t') continue;
        return c == '#';
    }
    return true;
}

int lerValorDepoisDoIgual(const string& linha) {
    size_t pos = linha.find('=');

    if (pos == string::npos) {
        cerr << "Erro no formato da linha: " << linha << endl;
        exit(1);
    }

    return stoi(linha.substr(pos + 1));
}

void lerArquivoEntrada(const string& nomeArquivo, vector<Processo>& processos) {
    ifstream arquivo(nomeArquivo);
    string nomeArquivoUsado = nomeArquivo;

    if (!arquivo.is_open()) {
        if (nomeArquivo.rfind("caso", 0) == 0 && nomeArquivo.find('_') == string::npos) {
            string altNome = string("caso_") + nomeArquivo.substr(4);
            arquivo.open(altNome);
            if (arquivo.is_open()) {
                nomeArquivoUsado = altNome;
            }
        }
    }

    if (!arquivo.is_open()) {
        cerr << "Erro: nao foi possivel abrir o arquivo: " << nomeArquivoUsado << endl;
        exit(1);
    }

    string linha;
    int n = 0;

    while (getline(arquivo, linha)) {
        if (ignorarLinha(linha)) {
            continue;
        }

        if (linha.find("n=") == 0) {
            n = lerValorDepoisDoIgual(linha);
        } else {
            stringstream ss(linha);

            Processo p;
            ss >> p.id >> p.chegada >> p.cpu;

            if (ss.fail()) {
                cerr << "Erro ao ler linha: " << linha << endl;
                exit(1);
            }

            processos.push_back(p);
        }
    }

    arquivo.close();

    if (n <= 0) {
        cerr << "Erro: valor de n invalido ou nao informado.\n";
        exit(1);
    }

    if ((int)processos.size() != n) {
        cerr << "Erro: numero de processos diferente de n.\n";
        cerr << "Esperado: " << n << " | Lido: " << processos.size() << endl;
        exit(1);
    }
}

void escreverSaida(
    const string& nomeArquivo,
    const vector<string>& gantt,
    const vector<Processo>& processos,
    double mediaRetorno,
    double mediaEspera,
    double mediaResposta
) {
    ofstream out(nomeArquivo);

    if (!out.is_open()) {
        cerr << "Erro: nao foi possivel criar o arquivo de saida.\n";
        exit(1);
    }

    out << "GANTT:\n";

    for (const string& linha : gantt) {
        out << linha << "\n";
    }

    out << "\nRESULTADOS:\n";
    out << "ID CHEGADA CPU TERMINO RETORNO ESPERA RESPOSTA\n";

    for (const Processo& p : processos) {
        out << p.id << " "
            << p.chegada << " "
            << p.cpu << " "
            << p.termino << " "
            << p.retorno << " "
            << p.espera << " "
            << p.resposta << "\n";
    }

    out << "\nMEDIAS:\n";
    out << fixed << setprecision(2);
    out << "RETORNO=" << mediaRetorno << "\n";
    out << "ESPERA=" << mediaEspera << "\n";
    out << "RESPOSTA=" << mediaResposta << "\n";

    out.close();
}

bool comp (Processo a, Processo b){
    return a.chegada < b.chegada;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso correto: " << argv[0] << " caso_1.txt saida.txt\n";
        return 1;
    }

    string arquivoEntrada = argv[1];
    string arquivoSaida = argv[2];

    vector<Processo> processos;
    vector<string> gantt;

    lerArquivoEntrada(arquivoEntrada, processos);

    /*
        TODO 1:
        Preparação inicial.
        1. Ordenar os processos por tempo de chegada (mantém a ordem cronológica).
        2. Inicializar o 'tempo_restante' de cada processo com o valor da sua 'cpu'.
    */

    sort(processos.begin(), processos.end(), comp);

    for (auto& proc : processos){
        proc.tempo_restante = proc.cpu;
    }

    /*
        TODO 2:
        Implementar a simulação dinâmica do algoritmo Round-Robin.

        Sugestão de estrutura e variáveis:
        
        int tempoAtual = 0;
        int processosConcluidos = 0;
        int n = processos.size();
        int quantum = 2; // Você pode alterar este valor ou ler como parâmetro
        
        queue<int> filaProntos; // Fila guardando o índice do processo no vector
        vector<bool> entrouNaFila(n, false); // Controle para não adicionar o mesmo processo duas vezes

        // Função auxiliar mental: Adicionar recém-chegados à fila
        // Sempre que o tempo avança, verifique quem chegou (chegada <= tempoAtual) 
        // e que ainda não entrouNaFila, e dê um filaProntos.push(indice).

        Enquanto (processosConcluidos < n):
            1. Atualizar a fila de prontos com processos que chegaram até o 'tempoAtual'.

            2. Se a fila estiver VAZIA (nenhum processo chegou ainda):
               - Encontrar o tempo de chegada do próximo processo futuro.
               - Registrar "IDLE" no Gantt (do tempoAtual até a próxima chegada).
               - Atualizar o tempoAtual para essa próxima chegada.
               - Adicionar esse novo processo à fila.
            
            3. Se a fila NÃO estiver vazia:
               - Retirar o primeiro processo da fila (filaProntos.front() e pop()).
               
               - Se for a primeira vez que ele executa (primeira_execucao == -1), 
                 registrar primeira_execucao = tempoAtual.
                 
               - Determinar quanto tempo ele vai rodar agora: 
                 tempo_execucao = min(quantum, tempo_restante)
                 
               - Registrar no Gantt: "tempoAtual-(tempoAtual + tempo_execucao) ID"
               
               - Avançar o tempoAtual: tempoAtual += tempo_execucao
               - Subtrair o tempo consumido: tempo_restante -= tempo_execucao
               
               - IMPORTANTE NO ROUND-ROBIN: Antes de devolver este processo inacabado 
                 para o final da fila, você precisa verificar se NOVOS processos 
                 chegaram durante essa 'fatia' de tempo e adicioná-los à fila primeiro!
                 (Repita a lógica do passo 1 aqui).
                 
               - Se o processo atual AINDA tem tempo_restante > 0:
                   Volta para o final da fila (filaProntos.push).
               - Se o processo TERMINOU (tempo_restante == 0):
                   Registrar termino = tempoAtual.
                   processosConcluidos++.
    */
    int tempoAtual = 0;
    int processosConcluidos = 0;
    int n = processos.size();
    int quantum = 2;

    queue<int> filaProntos;
    vector<bool> entrouNaFila(n, false);

    while(processosConcluidos < n){
        for(int i = 0; i < n; i++){
            if(processos[i].chegada <= tempoAtual){
                filaProntos.push(i);
            }
        }
        if (filaProntos.size() <= 0){
            int chegada = 123121231;
            int indiceProx = -1;
            for(int i = 0; i < n; i++){
                if(processos[i].chegada < chegada){
                    chegada = processos[i].chegada;
                    indiceProx = i;
                }
            } 
            gantt.push_back(to_string(tempoAtual) + "-" + to_string(processos[indiceProx].chegada) + " IDLE");
            filaProntos.push(indiceProx);  
        }
        else{
            int processo = filaProntos.front();
            filaProntos.pop();
            int tempo_execucao = min(quantum, processos[processo].tempo_restante);
            gantt.push_back(to_string(tempoAtual) + "-" + to_string(tempoAtual + tempo_execucao) + " " + processos[processo].id);
            tempoAtual += processos[processo].cpu;
            processos[processo].tempo_restante -= tempo_execucao;
            for(int i = 0; i < n; i++){
                if(processos[i].chegada <= tempoAtual){
                    filaProntos.push(i);
                }
            }
            if(processos[processo].tempo_restante > 0){
                filaProntos.push(processo);
            }
            else{
                processos[processo].termino = tempoAtual;
                processosConcluidos++;
            }
        }
    }
    

    /*
        TODO 3:
        Calcular as métricas de cada processo (após o loop do Round-Robin terminar):

        retorno = termino - chegada
        espera = retorno - cpu
        resposta = primeira_execucao - chegada
        
        Nota: Diferente do FCFS, no Round-Robin preemptivo, os tempos de 
        ESPERA e RESPOSTA geralmente terão valores diferentes!
    */
   for(int i = 0; i < processos.size(); i++){
        processos[i].retorno = processos[i].termino - processos[i].chegada; 
        processos[i].espera = processos[i].retorno - processos[i].cpu; 
        processos[i].resposta = processos[i].primeira_execucao - processos[i].chegada; 
    } 

    /*
        TODO 4:
        Calcular as médias gerais:

        mediaRetorno
        mediaEspera
        mediaResposta
    */
    
    /*
        TODO 5:
        Chamar a função escreverSaida com os resultados finais.
        (A chamada original já existente no final do main serve perfeitamente)
    */

    double mediaRetorno = 0.0;
    double mediaEspera = 0.0;
    double mediaResposta = 0.0;
    for(int i = 0; i < processos.size(); i++){
        mediaRetorno += processos[i].retorno; 
        mediaEspera += processos[i].espera; 
        mediaResposta += processos[i].resposta; 
    }
    
    mediaRetorno = mediaRetorno / processos.size();
    mediaEspera = mediaEspera / processos.size();
    mediaResposta = mediaResposta / processos.size();
    /*
        TODO 5:
        Chamar a funcao escreverSaida com os resultados finais.
    */

    escreverSaida(
        arquivoSaida,
        gantt,
        processos,
        mediaRetorno,
        mediaEspera,
        mediaResposta
    );

    return 0;
}