#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Processo
{
    string id;
    int chegada;
    int cpu;

    int termino = -1;
    int primeira_execucao = -1;
    int retorno = 0;
    int espera = 0;
    int resposta = 0;
};

bool ignorarLinha(const string &linha)
{
    for (char c : linha)
    {
        if (c == ' ' || c == '\t')
            continue;
        return c == '#';
    }
    return true;
}

int lerValorDepoisDoIgual(const string &linha)
{
    size_t pos = linha.find('=');

    if (pos == string::npos)
    {
        cerr << "Erro no formato da linha: " << linha << endl;
        exit(1);
    }

    return stoi(linha.substr(pos + 1));
}

void lerArquivoEntrada(const string &nomeArquivo, vector<Processo> &processos)
{
    ifstream arquivo(nomeArquivo);
    string nomeArquivoUsado = nomeArquivo;

    if (!arquivo.is_open())
    {
        if (nomeArquivo.rfind("caso", 0) == 0 && nomeArquivo.find('_') == string::npos)
        {
            string altNome = string("caso_") + nomeArquivo.substr(4);
            arquivo.open(altNome);
            if (arquivo.is_open())
            {
                nomeArquivoUsado = altNome;
            }
        }
    }

    if (!arquivo.is_open())
    {
        cerr << "Erro: nao foi possivel abrir o arquivo: " << nomeArquivoUsado << endl;
        exit(1);
    }

    string linha;
    int n = 0;

    while (getline(arquivo, linha))
    {
        if (ignorarLinha(linha))
        {
            continue;
        }

        if (linha.find("n=") == 0)
        {
            n = lerValorDepoisDoIgual(linha);
        }
        else
        {
            stringstream ss(linha);

            Processo p;
            ss >> p.id >> p.chegada >> p.cpu;

            if (ss.fail())
            {
                cerr << "Erro ao ler linha: " << linha << endl;
                exit(1);
            }

            processos.push_back(p);
        }
    }

    arquivo.close();

    if (n <= 0)
    {
        cerr << "Erro: valor de n invalido ou nao informado.\n";
        exit(1);
    }

    if ((int)processos.size() != n)
    {
        cerr << "Erro: numero de processos diferente de n.\n";
        cerr << "Esperado: " << n << " | Lido: " << processos.size() << endl;
        exit(1);
    }
}

void escreverSaida(
    const string &nomeArquivo,
    const vector<string> &gantt,
    const vector<Processo> &processos,
    double mediaRetorno,
    double mediaEspera,
    double mediaResposta)
{
    ofstream out(nomeArquivo);

    if (!out.is_open())
    {
        cerr << "Erro: nao foi possivel criar o arquivo de saida.\n";
        exit(1);
    }

    out << "GANTT:\n";

    for (const string &linha : gantt)
    {
        out << linha << "\n";
    }

    out << "\nRESULTADOS:\n";
    out << "ID CHEGADA CPU TERMINO RETORNO ESPERA RESPOSTA\n";

    for (const Processo &p : processos)
    {
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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
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
        Ordenar os processos por tempo de CHEGADA.
        Isso organiza a linha do tempo e facilita encontrar o proximo
        processo em caso de CPU ociosa.
        Em caso de empate na chegada, preserve a ordem original.
        */
    sort(processos.begin(), processos.end(), comp);

    /*
    TODO 2:
    Implementar a simulacao dinamica do algoritmo SJF (Nao-Preemptivo).
    Sugestao de estrutura:
    int tempoAtual = 0;
    int processosConcluidos = 0;
    int n = processos.size();
    Enquanto (processosConcluidos < n):
    1. Encontrar o indice do processo elegivel:
    - Deve ter (chegada <= tempoAtual)
    - Deve ainda nao ter terminado (termino == -1)
    - Entre os elegiveis, escolher o de MENOR CPU.
    - Em caso de empate na CPU, o criterio de desempate ja estara
    resolvido se voce iterar sequencialmente (pois a lista ja
    esta ordenada por chegada do TODO 1).
*/
    
    int tempoAtual =0;
    int processosDone =0;
    int processos = 0;



/*
    2. Se ENCONTROU um processo:
    - Atualizar primeira_execucao (se ainda for -1)
    - Calcular inicio = tempoAtual
    - Calcular termino = tempoAtual + cpu do processo
    - Registrar no Gantt: "inicio-termino ID"
    - Atualizar tempoAtual = termino
    - Incrementar processosConcluidos
    3. Se NAO ENCONTROU nenhum processo elegivel (CPU Ociosa):
    - A CPU ficara IDLE.
    - Encontrar qual o tempo de chegada do *proximo* processo
    que ainda nao executou.
    - Registrar no Gantt: "tempoAtual-proximaChegada IDLE"
    - Atualizar tempoAtual = proximaChegada
    */




    /*
    TODO 3:
    Calcular as metricas de cada processo (apos a simulacao concluir):
    retorno = termino - chegada
    espera = retorno - cpu
    resposta = primeira_execucao - chegada
    Nota: No SJF nao-preemptivo, assim como no FCFS, espera e resposta
    serao iguais, mas calcule ambas para manter a estrutura.
    */


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
        mediaResposta);
    return 0;
}