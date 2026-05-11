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

    bool comp(Processo a, Processo b)
    {
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
        Ordenar os processos por tempo de chegada.
        Em caso de empate, preservar a ordem original do arquivo.
    */
    sort( processos.begin(), processos.end(), comp);

    for (int i = 0; i < processos.size(); i++)
        cout << processos[i].id << " ";
    
    /*
        TODO 2:
        Implementar o algoritmo FCFS.

        Sugestao de variaveis:

        int tempoAtual = 0;

        Para cada processo:
            - se tempoAtual < chegada do processo:
                registrar IDLE no Gantt
                atualizar tempoAtual para chegada do processo

            - primeira_execucao = tempoAtual
            - inicio = tempoAtual
            - termino = tempoAtual + cpu
            - registrar no Gantt: "inicio-termino ID"
            - atualizar tempoAtual
    */

    int tempoAtual = 0;

    for (auto& umProcesso : processos) {

        if (tempoAtual < umProcesso.chegada )
        {
            gantt.push_back( to_string(tempoAtual) + "-" + to_string(umProcesso.chegada) + " IDLE");
            tempoAtual = umProcesso.chegada;
        }

        umProcesso.primeira_execucao = tempoAtual;
        
        int inicio = tempoAtual;

        umProcesso.termino = tempoAtual + umProcesso.cpu;

        gantt.push_back(to_string(inicio) + "-" + to_string(umProcesso.termino) + " " + (umProcesso.id) );

        tempoAtual = umProcesso.termino;
    }
        

    /*
        TODO 3:
        Calcular as metricas de cada processo:

        retorno = termino - chegada
        espera = retorno - cpu
        resposta = primeira_execucao - chegada
    */

    for (auto& umProcesso : processos){
        umProcesso.retorno = umProcesso.termino - umProcesso.chegada;
        umProcesso.espera = umProcesso.retorno - umProcesso.cpu;
        umProcesso.resposta = umProcesso.primeira_execucao - umProcesso.chegada;
    }

    /*
        TODO 4:
        Calcular as medias:

        mediaRetorno
        mediaEspera
        mediaResposta
    */
    

    double mediaRetorno = 0.0;
    double mediaEspera = 0.0;
    double mediaResposta = 0.0;

    for (auto& umProcesso : processos){
        mediaRetorno += umProcesso.retorno;
        mediaEspera += umProcesso.espera;
        mediaResposta += umProcesso.resposta;
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