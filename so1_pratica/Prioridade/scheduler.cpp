#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Processo {
    string id;
    int chegada;
    int cpu;
    int prioridade; // <-- Nova propriedade adicionada

    int termino = -1;
    int primeira_execucao = -1;
    int retorno = 0;
    int espera = 0;
    int resposta = 0;
    bool executou = false; // Flag util para ajudar na busca de processos
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
        if (ignorarLinha(linha)) continue;

        if (linha.find("n=") == 0) {
            n = lerValorDepoisDoIgual(linha);
        } else {
            stringstream ss(linha);
            Processo p;
            // Leitura atualizada para incluir a prioridade
            ss >> p.id >> p.chegada >> p.cpu >> p.prioridade;

            if (ss.fail()) {
                cerr << "Erro ao ler linha: " << linha << endl;
                exit(1);
            }
            processos.push_back(p);
        }
    }
    arquivo.close();

    if (n <= 0) {
        cerr << "Erro: valor de n invalido.\n";
        exit(1);
    }
    if ((int)processos.size() != n) {
        cerr << "Erro: numero de processos diferente de n.\n";
        exit(1);
    }
}

void escreverSaida(const string& nomeArquivo, const vector<string>& gantt, const vector<Processo>& processos, double mediaRetorno, double mediaEspera, double mediaResposta) {
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
    // Cabeçalho atualizado
    out << "ID CHEGADA CPU PRIO TERMINO RETORNO ESPERA RESPOSTA\n";

    for (const Processo& p : processos) {
        out << p.id << " "
            << p.chegada << " "
            << p.cpu << " "
            << p.prioridade << " "
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

    vector<Processo> processos;
    vector<string> gantt;
    lerArquivoEntrada(argv[1], processos);

    /*
        TODO 1:
        Para facilitar em caso de empate, você pode ordenar inicialmente 
        todos os processos por tempo de chegada. Assim, a ordem FIFO 
        já estará natural no seu vetor.
    */
    stable_sort(processos.begin(), processos.end(), comp);

    /*
        TODO 2:
        Implementar o algoritmo de PRIORIDADES.

        Dica de loop:
        int tempoAtual = 0;
        int processosConcluidos = 0;

        while(processosConcluidos < n) {
            - Buscar em 'processos' qual processo satisfaz:
              1. chegadas <= tempoAtual
              2. executou == false
              3. tem a MAIOR prioridade dentre os disponíveis

            - Se achou um processo:
                - primeira_execucao = tempoAtual
                - termino = tempoAtual + cpu
                - registrar no Gantt
                - tempoAtual += cpu
                - marcar executou = true
                - processosConcluidos++
            - Se NAO achou nenhum processo:
                - A CPU está ociosa. 
                - Avançar o tempoAtual para o tempo de chegada do próximo processo futuro.
                - Registrar IDLE no Gantt.
        }
    */
    int tempoAtual = 0;
    int processosConcluidos = 0;
    int n = processos.size();
    
    vector<bool> executou(n,false);
    while( processosConcluidos < n ){
        int max = -1;
        int max_ind = -1;
        for(int i = 0; i < n; i++){
            
            if(processos[i].chegada <= tempoAtual){
                if(executou[i] == false){
                    if(processos[i].prioridade > max){
                        max = processos[i].prioridade;
                        max_ind = i;
                    }
                }
            }
        }
        if (max != -1){
            processos[max_ind].primeira_execucao = tempoAtual;
            processos[max_ind].termino = tempoAtual + processos[max_ind].cpu;
            gantt.push_back(to_string(tempoAtual) + "-" + to_string(processos[max_ind].termino) + " " + processos[max_ind].id);
            tempoAtual += processos[max_ind].cpu;
            executou[max_ind] = true;
            processosConcluidos++;
        }
        else{
            int prox_ind = -1; 
            for(int i = 0; i < n; i++){
                if(executou[i] == false){
                    tempoAtual = processos[i].chegada;
                    prox_ind = i;
                }
            }
            if(prox_ind != -1){
                gantt.push_back(to_string(tempoAtual) + "-" + to_string(processos[prox_ind].chegada) + " " + "IDLE" );
            }
        }
        
    }

    /* TODO 3 e 4: Calcular métricas e médias como no FCFS */
    
    for(int i = 0; i < processos.size(); i++){
        processos[i].retorno = processos[i].termino - processos[i].chegada; 
        processos[i].espera = processos[i].retorno - processos[i].cpu; 
        processos[i].resposta = processos[i].primeira_execucao - processos[i].chegada; 
    } 

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

    escreverSaida(argv[2], gantt, processos, mediaRetorno, mediaEspera, mediaResposta);

    return 0;
}