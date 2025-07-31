/*
## Universidade Federal da Bahia
## Docente: Danilo Azevedo
## Bacharelado em Ciencia da Computacao - Estrutura de Dados II

chaves testadas: 20,40,10,30,15,35,7,26,18,22,
5,42,13,46,27,8,32,38,24,45,25

 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define ORDEM 2
#define namefile "_arquivo.dat"


int cont = -1, count = 0;

typedef struct
{
    int chave;
    char nome[20];
    int idade;
    int ApPage[2];
    int rank;
} Registro;

#define ORDEM ((1024 - (5 * sizeof(long) + sizeof(Registro)) ) /  (sizeof(Registro) + sizeof(long)))

typedef struct Pagina_str *Apontador;
typedef struct Pagina_str
{
    int n;
    int pageNum;
    int num;
    Registro r[2*ORDEM];
    Apontador p[2*ORDEM+1];
} Pagina;

struct Node{
    Registro info;
    struct Node *prox;
};
typedef struct Node node;

//##########################################################
void Antecessor(Apontador Ap, int Ind, Apontador ApPai, int *Diminuiu);
void Busca(Registro Reg, Apontador Ap);void em_ordem(Apontador raiz);
void buscainFile(Registro Reg, Apontador pagina);int isleaf(Apontador a);
int file_exists(const char *filename);void Insere(Registro Reg, Apontador *Ap);
void InsereNaPagina(Apontador Ap, Registro Reg, Apontador ApDir);
void Ins(Registro Reg, Apontador Ap, int *Cresceu, Registro *RegRetorno, Apontador *ApRetorno);
int Imprime2(Apontador p, int Nivel, int aux, int n);int Imprime(Apontador p, int N, int aux, int n);
void InsertPosFile(Registro Reg, Apontador *Ap);node *busca(Registro info, node *LISTA);
void InsPosFile(Registro Reg, Apontador Ap, int *Cresceu, Registro *RegRetorno, Apontador *ApRetorno);
void pos_ordem(Apontador raiz);void Retira(int Ch, Apontador *Ap);void Ret(int Ch, Apontador *Ap, int *Diminuiu);
void Reconstitui(Apontador ApPag, Apontador ApPai, int PosPai, int *Diminuiu);
void saveAux(Apontador p, int Nivel);void salvar(Apontador pagina, Registro Reg[]);
void recuperarReg(Apontador *arv, node *LISTA);void insereInicio(Registro info, node *LISTA);
void execut(Registro info, node *LISTA);void exibe(node *LISTA);

typedef Apontador TipoDicionario;
void Inicializa(TipoDicionario *Dicionario)
{
    *Dicionario = NULL;
}  /* Inicializa a estrutura */

void inicia(node *LISTA)
{
    LISTA->prox = NULL;
}

//############################### Estrutura 1 Btree

void Antecessor(Apontador Ap, int Ind, Apontador ApPai, int *Diminuiu)
{
    if (ApPai->p[ApPai->n] != NULL)
    {
        Antecessor(Ap, Ind, ApPai->p[ApPai->n], Diminuiu);
        if (*Diminuiu)
            Reconstitui(ApPai->p[ApPai->n], ApPai, ApPai->n, Diminuiu);
        return;
    }
    Ap->r[Ind - 1] = ApPai->r[ApPai->n - 1];
    ApPai->n--;
    *Diminuiu = ApPai->n < ORDEM;
}  /* Antecessor */

void Busca(Registro Reg, Apontador Ap)
{
    int i;

    if (Ap == NULL) //
    {
        printf("chave nao encontrada: %d\n", Reg.chave);
        return;
    }
    i = 1;
    while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave)
        i++;
    if (Reg.chave == Ap->r[i - 1].chave)
    {
        printf("chave: %d \n", Reg.chave);
        buscainFile(Ap->r[i-1],Ap);
        return;
    }
    if (Reg.chave < Ap->r[i - 1].chave)
        Busca(Reg, Ap->p[i-1]);
    else
        Busca(Reg, Ap->p[i]);
}

void buscainFile(Registro Reg, Apontador pagina)
{
    Registro reg[2*ORDEM];
    int i;
    FILE *arq = fopen(namefile,"rb");
    if (arq == NULL)
        exit(1);
    fseek(arq, pagina->pageNum*(2*ORDEM*sizeof(Registro)), SEEK_SET);
    fread(reg, (2*ORDEM*sizeof(Registro)),1,arq);
    fclose(arq);
    for(i = 0; i < 2*ORDEM; i++){
        if (Reg.chave == reg[i].chave)
            printf("%s%d\n",reg[i].nome,reg[i].idade);
    }
}

void em_ordem(Apontador raiz)
{
    int i;
    if (raiz != NULL)
    {
        for (i = 0; i < raiz->n; i++)
        {
            em_ordem(raiz->p[i]);
            printf("%d ", raiz->r[i].chave);
            printf("\n");
        }
        em_ordem(raiz->p[i]);
    }
}

int file_exists(const char *filename)
{
    FILE *arquivo;

    if((arquivo = fopen(filename, "rb")))
    {
        fclose(arquivo);
        return 1;
    }
    return 0;
}

int isleaf(Apontador a)
{
    if (a->p[0] == NULL)
    return 1;
    else
    return 0;
}

void InsereNaPagina(Apontador Ap, Registro Reg, Apontador ApDir)
{

    int k;
    int NaoAchouPosicao;

    k = Ap->n;
    NaoAchouPosicao = k > 0;
    while (NaoAchouPosicao)
    {
        if (Reg.chave >= Ap->r[k - 1].chave)
        {
            NaoAchouPosicao = 0;
            break;
        }

        Ap->r[k] = Ap->r[k - 1];

        Ap->p[k + 1] = Ap->p[k];

        k--;
        if (k < 1)
            NaoAchouPosicao = 0;
    }
    Ap->r[k] = Reg;
    Ap->p[k + 1] = ApDir;
    Ap->n++;
}

void Ins(Registro Reg, Apontador Ap, int *Cresceu, Registro *RegRetorno, Apontador *ApRetorno)
{
    Apontador ApTemp;
    Registro Aux;
    int i, j;

    if (Ap == NULL)
    {
        *Cresceu = 1;
        *RegRetorno = Reg;
        *ApRetorno = NULL;
        return;
    }
    i = 1;
    while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave)
        i++;
    if (Reg.chave == Ap->r[i - 1].chave)
    {
        printf("chave ja existente: %d \n", Reg.chave);
        *Cresceu = 0;
        return;
    }
    if (Reg.chave < Ap->r[i - 1].chave)
        Ins(Reg, Ap->p[i-1], Cresceu, RegRetorno, ApRetorno);
    else
        Ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno);
    if (!*Cresceu)
        return;
    if (Ap->n < 2*ORDEM)
    {  /* Verificando se a pagina tem espaco */
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
        *Cresceu = 0;
        return;
    }
    /* Split: Pagina tem que ser dividida */
    ApTemp = (Apontador) malloc(sizeof(Pagina));
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;
    cont++;
    ApTemp->pageNum = cont;

    if (i <= ORDEM + 1)
    {
        InsereNaPagina(ApTemp, Ap->r[2*ORDEM - 1], Ap->p[2*ORDEM]);
        Ap->n--;
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
    }
    else {
        InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno);
    }
    for (j = ORDEM + 2; j <= 2*ORDEM; j++)
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);


    Ap->n = ORDEM;
    ApTemp->p[0] = Ap->p[ORDEM + 1];
    *RegRetorno = Ap->r[ORDEM];
    *ApRetorno = ApTemp;
    for (j = ORDEM; j < Ap->n+2; j++){
        Aux.chave = 0;
        Aux.rank = 0;
        Ap->r[j] = Aux;
    }
}  /*Ins*/

void Insere(Registro Reg, Apontador *Ap)
{
    int Cresceu;
    Registro RegRetorno;
    Apontador ApRetorno;
    Apontador ApTemp;

    Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);
    if (Cresceu) { /* Se arvore cresce na altura pela raiz */
        ApTemp = (Apontador) malloc(sizeof(Pagina));
        ApTemp->n = 1;
        ApTemp->r[0] = RegRetorno;
        ApTemp->p[1] = ApRetorno;
        cont++;
        ApTemp->pageNum = cont;
        ApTemp->p[0] = *Ap;
        *Ap = ApTemp;
        //save in file
    }
    saveAux(*Ap, 2*ORDEM);
}  /*Insercao*/

int Imprime2(Apontador p, int Nivel, int aux, int n)
{
    int i;

    if (p == NULL)
        return 0;
    if (Nivel == aux) {
        n++;
        printf("No: %d : ",p->pageNum);
        for (i = 0; i < p->n; i++){
            if (p->p[i] !=NULL)
                printf("Apontador: %d ", p->p[i]->pageNum);
            else
                printf("Apontador: null ");
            printf("chave: %d ", p->r[i].chave);
        }
        printf("\n");
        return n;
    }else {
        aux++;
        for (i = 0; i <= p->n; i++)
        Imprime2(p->p[i], Nivel, aux, n);
    }
    return 0;
}

int Imprime(Apontador p, int N, int aux, int n)
{
    int i;

    if (p == NULL)
        return 0;
    if (p->pageNum != aux) {
        p->num = N;
        aux = p->pageNum;
        return aux;
    } else{
        aux++;
        for (i = 0; i <= p->n; i++)
        Imprime(p->p[i], N, aux, n);
    }
    return 0;
}

void InsertPosFile(Registro Reg, Apontador *Ap)
{
    int Cresceu;
    Registro RegRetorno;
    Apontador ApRetorno;
    Apontador ApTemp;

    InsPosFile(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);
    if (Cresceu) { /* Se arvore cresce na altura pela raiz */
        ApTemp = (Apontador) malloc(sizeof(Pagina));
        ApTemp->n = 1;
        ApTemp->r[0] = RegRetorno;
        ApTemp->p[1] = ApRetorno;
        cont++;
        ApTemp->pageNum = cont;
        ApTemp->p[0] = *Ap;
        *Ap = ApTemp;
    }
}

void InsPosFile(Registro Reg, Apontador Ap, int *Cresceu, Registro *RegRetorno, Apontador *ApRetorno)
{
    Apontador ApTemp;
    Registro Aux;
    int i, j;

    if (Ap == NULL) //
    {
        *Cresceu = 1;
        *RegRetorno = Reg;
        *ApRetorno = NULL;
        return;
    }
    i = 1;
    while (i < Ap->n && Reg.chave > Ap->r[i - 1].chave)
        i++;
    if (Reg.chave == Ap->r[i - 1].chave)
    {
        //printf("chave ja existente: %d \n", Reg.chave);
        *Cresceu = 0;
        return;
    }
    if (Reg.chave < Ap->r[i - 1].chave)
        InsPosFile(Reg, Ap->p[i-1], Cresceu, RegRetorno, ApRetorno);
    else
        InsPosFile(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno);
    if (!*Cresceu)
        return;
    if (Ap->n < 2*ORDEM)
    {  /* Verificando se a pagina tem espaco */
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
        *Cresceu = 0;
        return;
    }
    /* Split: Pagina tem que ser dividida */
    ApTemp = (Apontador) malloc(sizeof(Pagina));
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;
    cont++;
    ApTemp->pageNum = cont;

    if (i <= ORDEM + 1)
    {
        InsereNaPagina(ApTemp, Ap->r[2*ORDEM - 1], Ap->p[2*ORDEM]);
        Ap->n--;
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
    }
    else{
        InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno);
        }
    for (j = ORDEM + 2; j <= 2*ORDEM; j++)
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);


    Ap->n = ORDEM;
    ApTemp->p[0] = Ap->p[ORDEM + 1];
    *RegRetorno = Ap->r[ORDEM];
    *ApRetorno = ApTemp;
    for (j = ORDEM; j < Ap->n+2; j++){
        Aux.chave = 0;
        Aux.rank = 0;
        Ap->r[j] = Aux;
    }
}

void pos_ordem(Apontador raiz)
{
    int i;
    if (isleaf(raiz)){
        for (i = raiz->n-1; i >=0; i--){
            printf("%d\n", raiz->r[i].chave);

        }
    }
    else{
        pos_ordem(raiz->p[raiz->n]);
        for (i = raiz->n-1; i >=0; i--){
            printf("%d\n", raiz->r[i].chave);
        pos_ordem(raiz->p[i]);
            }
    }
}

void Pesquisa(Registro *x, Apontador Ap)
{
    int i;

    if (Ap == NULL)
    {
        //printf("Erro: Registro nao existe\n");

        return;
    }
    i = 1;
    while (i < Ap->n && x->chave > Ap->r[i - 1].chave)
        i++;
    if (x->chave == Ap->r[i - 1].chave)
    {
        *x = Ap->r[i - 1];
        return;
    }
    if (x->chave < Ap->r[i - 1].chave)
        Pesquisa(x, Ap->p[i - 1]);
    else
        Pesquisa(x, Ap->p[i]);
} /* Pesquisa */

void Reconstitui(Apontador ApPag, Apontador ApPai, int PosPai, int *Diminuiu)
{
    Apontador Aux;
    int DispAux, j;

    if (PosPai < ApPai->n) {  /* Aux = Pagina a direita de ApPag */
        Aux = ApPai->p[PosPai + 1];
        DispAux = (Aux->n - ORDEM + 1) / 2;
        ApPag->r[ApPag->n] = ApPai->r[PosPai];
        ApPag->p[ApPag->n + 1] = Aux->p[0];
        ApPag->n++;
        if (DispAux > 0) {  /* Existe folga: transfere de Aux para ApPag */
            for (j = 1; j < DispAux; j++)
                InsereNaPagina(ApPag, Aux->r[j - 1], Aux->p[j]);
            ApPai->r[PosPai] = Aux->r[DispAux - 1];
            Aux->n -= DispAux;
            for (j = 0; j < Aux->n; j++)
                Aux->r[j] = Aux->r[j + DispAux];
            for (j = 0; j <= Aux->n; j++)
                Aux->p[j] = Aux->p[j + DispAux];
            *Diminuiu = 0;
        }
        else
        { /* Fusao: intercala Aux em ApPag e libera Aux */
            for (j = 1; j <= ORDEM; j++)
                InsereNaPagina(ApPag, Aux->r[j - 1], Aux->p[j]);
            free(Aux);
            for (j = PosPai + 1; j < ApPai->n; j++)
            {   /* Preenche vazio em ApPai */
                ApPai->r[j - 1] = ApPai->r[j];
                ApPai->p[j] = ApPai->p[j + 1];
            }
            ApPai->n--;
            if (ApPai->n >= ORDEM)
                *Diminuiu = 0;
        }
    }
    else
    { /* Aux = Pagina a esquerda de ApPag */
        Aux = ApPai->p[PosPai - 1];
        DispAux = (Aux->n - ORDEM + 1) / 2;
        for (j = ApPag->n; j >= 1; j--)
            ApPag->r[j] = ApPag->r[j - 1];
        ApPag->r[0] = ApPai->r[PosPai - 1];
        for (j = ApPag->n; j >= 0; j--)
            ApPag->p[j + 1] = ApPag->p[j];
        ApPag->n++;
        if (DispAux > 0) {  /* Existe folga: transfere de Aux para ApPag */
            for (j = 1; j < DispAux; j++)
                InsereNaPagina(ApPag, Aux->r[Aux->n - j], Aux->p[Aux->n - j + 1]);
            ApPag->p[0] = Aux->p[Aux->n - DispAux + 1];
            ApPai->r[PosPai - 1] = Aux->r[Aux->n - DispAux];
            Aux->n -= DispAux;
            *Diminuiu = 0;
        }
        else
        {  /* Fusao: intercala ApPag em Aux e libera ApPag */
            for (j = 1; j <= ORDEM; j++)
                InsereNaPagina(Aux, ApPag->r[j - 1], ApPag->p[j]);
            free(ApPag);
            ApPai->n--;
            if (ApPai->n >= ORDEM)
                *Diminuiu = 0;
        }
    }
}  /* Reconstitui */

void Ret(int Ch, Apontador *Ap, int *Diminuiu)
{
    int Ind, j;
    Apontador WITH;
    Registro Reg;

    if (*Ap == NULL)
    {
        printf("chave nao encontrada: %i\n", Ch);
        *Diminuiu = 0;
        return;
    }
    WITH = *Ap;
    Ind = 1;
    while (Ind < WITH->n && Ch > WITH->r[Ind - 1].chave)
        Ind++;
    if (Ch == WITH->r[Ind - 1].chave)
    {
        Reg.chave = 0;
        Reg.rank = 0;
        WITH->r[Ind -1] = Reg;
        if (WITH->p[Ind - 1] == NULL) {  /* Pagina folha */
            WITH->n--;
            *Diminuiu = WITH->n < ORDEM;
            for (j = Ind; j <= WITH->n; j++)
            {
                WITH->r[j - 1] = WITH->r[j];
                WITH->p[j] = WITH->p[j + 1];
            }
            return;
        }
        Antecessor(*Ap, Ind, WITH->p[Ind - 1], Diminuiu);
        if (*Diminuiu)
            Reconstitui(WITH->p[Ind - 1], *Ap, Ind - 1, Diminuiu);
        return;
    }
    if (Ch > WITH->r[Ind - 1].chave)
        Ind++;
    Ret(Ch, &WITH->p[Ind - 1], Diminuiu);
    if (*Diminuiu)
        Reconstitui(WITH->p[Ind - 1], *Ap, Ind - 1, Diminuiu);
}  /* Ret */

void Retira(int Ch, Apontador *Ap)
{
    int Diminuiu;
    Apontador Aux;

    Ret(Ch, Ap, &Diminuiu);
    if (Diminuiu && (*Ap)->n == 0) { /* Arvore diminui na altura */
        Aux = *Ap;
        *Ap = Aux->p[0];
        free(Aux);

    }

}  /* Retira */

void recuperarReg(Apontador *arv, node *LISTA)
{

    FILE* arq;
    Registro Reg[2*ORDEM];
    node *tmp;
    int i=0,j=0, tam;
        arq = fopen(namefile,"rb");
        if (arq == NULL)
            exit(1);
        fseek(arq, 0, SEEK_END);
        tam = ftell(arq);
        rewind(arq);
        fclose(arq);

        arq = fopen(namefile,"rb");
        if (arq == NULL)
            exit(1);
        while (j*(2*ORDEM*sizeof(Registro)) < tam){
            fseek(arq, j*(2*ORDEM*sizeof(Registro)), SEEK_SET);
            fread(Reg, 2*ORDEM*sizeof(Registro),1,arq);

        for(i = 0; i < 2*ORDEM; i++){
           if (Reg[i].chave > 0)
           execut(Reg[i], LISTA);
        }
    j++;
    }
    fclose(arq);
    tmp = LISTA->prox;
    while( tmp != NULL){
        InsertPosFile(tmp->info, arv);
        tmp = tmp->prox;
    }
    free(tmp);
    free(LISTA);
    //Imprime(*arv, 2*ORDEM, 0);
    }

void saveAux(Apontador p, int Nivel)
{
    int i,j;

    if (p == NULL)
        return;
    for (i = 0; i < p->n; i++)
        salvar(p, p->r);
    for (j = 0; j <= p->n; j++)
        saveAux(p->p[j], Nivel + 1);
}

void salvar(Apontador pagina, Registro Reg[])
{

    FILE* arq;
    if (!file_exists(namefile)){
        arq = fopen(namefile,"wb");
        if (arq == NULL)
            exit(1);
        fseek(arq, pagina->pageNum*(2*ORDEM*sizeof(Registro)), SEEK_SET);
        fwrite(Reg,(2*ORDEM*sizeof(Registro)),1,arq);
        fclose(arq);
    }

    else{
        arq = fopen(namefile,"r+b");
        if (arq == NULL)
            exit(1);
        fseek(arq, pagina->pageNum*(2*ORDEM*sizeof(Registro)), SEEK_SET);
        fwrite(Reg, (2*ORDEM*sizeof(Registro)),1,arq);
        fclose(arq);
        }
    }

//############################### //Estrutura 1 Btree

//############################### //Estrutura 2 lista encadeada
void insereInicio(Registro info, node *LISTA)
{
    node *novo=(node *) malloc(sizeof(node));
    node *tmp = busca(info, LISTA);
    if(LISTA->prox == NULL){
        novo->info = info;
        novo->prox = LISTA->prox;
        LISTA->prox = novo;
    } else {
        novo->prox = tmp->prox;
        novo->info = info;
        tmp->prox = novo;
    }
//exibe(LISTA);
}

node *busca(Registro info, node *LISTA)
{
    node *atual,*antNode;
    atual = LISTA->prox;
    antNode = LISTA;
    while(atual != NULL && info.rank > atual->info.rank) {
        antNode = atual;
        atual = atual->prox;
    }
    return antNode;
}

void exibe(node *LISTA)
{
    if(!LISTA){
        return ;
    }
    node *tmp;
    tmp = LISTA->prox;

    while( tmp != NULL){
        printf(" %d ", tmp->info.chave);
        tmp = tmp->prox;
    }
    printf("\n");
}

void execut(Registro info, node *LISTA)
{
    insereInicio(info, LISTA);
}

//############################# //Estrutura 2 lista encadeada

// int main()
// {
//     printf("INSTRUCOES: LEIA TUDO ISSO ANTES DE EXECUTAR \n");
//     printf("Para INSERIR entre com a letra 'i' <ENTER> \n ");
//     printf("Na linha seguinte entre com uma 'chave' (numero > 0) <ENTER> \n");
//     printf("Na linha seguinte entre com um 'nome' <ENTER> \n ");
//     printf("Na linha seguinte entre com uma 'idade' <ENTER> \n");
//     printf("Para CONSULTAR ou REMOVER registros entre com: \n");
//     printf("--- consultar - 'c', na linha seguinte a chave. \n");
//     printf("--- remover - 'r', na linha seguinte a chave. \n");
//     printf("Para IMPRIMIR a estrutura entre com a letra 'p' ou 'o' <ENTER> \n");



//     Apontador *arv;
//     Registro reg;
//     char tecla;
//     int chave, i, num = 0;
//     arv=(Apontador*) malloc(sizeof(Apontador));
//     node *LISTA = (node *) malloc(sizeof(node));
//     Inicializa(arv);
//     inicia(LISTA);
//     if (file_exists(namefile)){
//         recuperarReg(arv, LISTA);
//     }

//     while(1)
//     {
//         scanf("%c", &tecla);
//         if (tecla=='e')
//         break;
//         switch(tecla)
//         {
//             case 'i':
//             //printf("insercao \n");
//                 scanf("%d", &chave);
//                 reg.chave = chave;
//                 count++;
//                 reg.rank = count;
//                 fflush(stdin);
//                 getchar();
//                 fgets(reg.nome, sizeof(reg.nome), stdin);
//                 scanf("%d", &reg.idade);
//                 if (reg.chave <= 0){
//                     count--;
//                     break;
//                 }
//                 Insere(reg,arv);
//                 break;
//             case 'r':
//                 scanf("%d", &chave);
//                 reg.chave = chave;
//                 Retira(reg.chave, arv);
//                 remove(namefile);
//                     saveAux(*arv,2*ORDEM);
//                 break;
//             case 'o':
//                     em_ordem(*arv);
//                 break;
//             case 'p':
//                 for(i=0; i < 2*ORDEM; i++){
//                     num = Imprime2(*arv, i, 0, num);
//                 }
//                 break;
//             case 'c':
//                 getchar();
//                 scanf("%d", &chave);
//                 reg.chave = chave;
//                 Busca(reg,*arv);
//                 break;
//         }

//         fflush(stdin);
//     }
//     return 0;
//     }

//##############################################
