#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//estrutura para o nó
typedef struct HuffmanNode {
    char character;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

//fila de prioridade
typedef struct PriorityQueue {
    HuffmanNode *nodes[256];
    int size;
} PriorityQueue;

//cria um novo nó
HuffmanNode* createNode(char character, int frequency) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->character = character;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

//inicia a fila
void initPriorityQueue(PriorityQueue *pq) {
    pq->size = 0;
}

//insere um nó na fila
void insertPriorityQueue(PriorityQueue *pq, HuffmanNode *node) {
    pq->nodes[pq->size++] = node; //insere o nó passado no ultimo lugar da fila
    int i = pq->size - 1; //i é a posição que acabamos de inserir o nó
    //fila de prioridade ordenada pela frequência e desempatar pelo caractere
    while (i && pq->nodes[(i - 1) / 2]->frequency >= node->frequency) { //enquanto a freq do novo nó for menor que a do pai
        if (pq->nodes[(i - 1) / 2]->frequency > node->frequency || //se a freq do pai for maior ou igual a do novo nó
            (pq->nodes[(i - 1) / 2]->frequency == node->frequency && pq->nodes[(i - 1) / 2]->character > node->character)) { //se a freq do pai for igual a do novo nó e desempate se o caractere do pai for maior
            pq->nodes[i] = pq->nodes[(i - 1) / 2]; //o pai passa a ser o novo nó
            i = (i - 1) / 2; //i vai para a posição do pai
        } else {
            break;
        }
    }
    pq->nodes[i] = node; //insere o node na posiçao do pai
}

//remove o nó com a menor frequência da fila
HuffmanNode* removeMinPriorityQueue(PriorityQueue *pq) {
    HuffmanNode *minNode = pq->nodes[0]; //pega o primeiro nó da fila (menor freq)
    pq->nodes[0] = pq->nodes[--pq->size]; //diminui o tamanho da fila e coloca o ultimo nó na primeira posição
    int i = 0;
    while (i * 2 + 1 < pq->size) { //enquanto o nó não for uma folha(tiver filhos)
        int j = i * 2 + 1; //j é o indice do filho esquerdo
        if (j + 1 < pq->size && //se o nó tiver dois filhos
            (pq->nodes[j + 1]->frequency < pq->nodes[j]->frequency || 
             (pq->nodes[j + 1]->frequency == pq->nodes[j]->frequency && pq->nodes[j + 1]->character < pq->nodes[j]->character))) { 
            j++; 
        }
        if (pq->nodes[i]->frequency > pq->nodes[j]->frequency ||
            (pq->nodes[i]->frequency == pq->nodes[j]->frequency && pq->nodes[i]->character > pq->nodes[j]->character)) {
            HuffmanNode *temp = pq->nodes[i];
            pq->nodes[i] = pq->nodes[j];
            pq->nodes[j] = temp;
            i = j;
        } else {
            break;
        }
    }
    return minNode;
}

//constroi a arvore
HuffmanNode* buildHuffmanTree(int frequencies[]) {
    PriorityQueue pq;
    initPriorityQueue(&pq);//inicia a fila
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) { //se a freq do caractere for maior que 0
            insertPriorityQueue(&pq, createNode((char)i, frequencies[i])); //cria um nó com o caractere e a freq na priorityqueue
        }
    }
    while (pq.size > 1) 
    {
        HuffmanNode *left = removeMinPriorityQueue(&pq); //left é setado como o menor número da fila
        HuffmanNode *right = removeMinPriorityQueue(&pq); //right é setado como o segundo menor número da fila
        HuffmanNode *merged = createNode('\0', left->frequency + right->frequency); //cria um nó com a soma das frequencias
        merged->left = left; //left é setado como o filho esquerdo do merged
        merged->right = right; //right também é setado como o filho direito do merged
        insertPriorityQueue(&pq, merged); //insere o merged na fila
    }
    return removeMinPriorityQueue(&pq); //retorna minNode
}

void buildHuffmanCodes(HuffmanNode *root, char *code, int length, char codes[][256]) { //matriz com 256 colunas(caracteres) onde armazenamos os códigos
    if (root->left == NULL && root->right == NULL) { //se for uma folha
        code[length] = '\0'; //adiciona \0 indicando que é o fim do código
        strcpy(codes[(unsigned char)root->character], code); //armazena o code no lugar correspondente na matriz codes
        return;
    }
    if (root->left) {
        code[length] = '0'; //se formos para a esquerda adicionamos 0 na matriz de códigos daquela letra
        buildHuffmanCodes(root->left, code, length + 1, codes); //chamamos a função recursivamente para a esquerda
    }
    if (root->right) {
        code[length] = '1'; //se formos para a direita adicionamos 1 na matriz de códigos daquela letra
        buildHuffmanCodes(root->right, code, length + 1, codes);//funcao recursiva para a direita
    }
}

//codificar o texto usando a arvore de huffman
void encodeText(const char *text, char codes[][256], FILE *outputFile) {
    while (*text) { 
        fputs(codes[(unsigned char)*text], outputFile); //escreve o código correspondente ao caractere no arquivo de saída
        text++;//passa pro próximo
    }
}

void decodeText(HuffmanNode *root, const char *encodedText, FILE *outputFile) {
    HuffmanNode *current = root;
    while (*encodedText) { //enquanto não chegarmos ao fim do texto codificado
        if (*encodedText == '0') { //se o caractere for 0
            current = current->left; //vai pra esquerda
        } else {
            current = current->right; //caso contrário vai pra direita
        }
        if (current->left == NULL && current->right == NULL) {
            fputc(current->character, outputFile); //se chegarmos em uma folha, escrevemos o caractere correspondente
            current = root; //voltamos para a raiz
        }
        encodedText++; //passamos pro próximo caractere
    }
}

//func para ler e contar as frequencias dos caracteres
void readInputFile(const char *filename, int frequencies[], char *text) {
    FILE *file = fopen(filename, "r");
    int contador = 0; //qual letra estamos lendo
    while ((text[contador] = fgetc(file)) != EOF) { //enquanto get character não retornar EOF(fim do arquivo) vai armazenando o caractere lido no vetor text
        frequencies[(unsigned char)text[contador++]]++; //converte o caractere lido para um numero e incrementa a frequência do caractere correspondente no vetor frequencies
    }
    text[contador] = '\0';
    fclose(file);
}

int main() {
    int frequencies[256] = {0};
    char text[9999];
    char codes[256][256] = {{0}}; //preenche tudo com 0
    char encodedText[9999];

    //le o arquivo de entrada e conta as frequencias
    readInputFile("amostra.txt", frequencies, text);

    //constroi a arvore
    HuffmanNode *huffmanTree = buildHuffmanTree(frequencies);

    //constroi a tabela de codigos
    char code[256];
    buildHuffmanCodes(huffmanTree, code, 0, codes);

    //codifica o texto e grava no arquivo
    FILE *encodedFile = fopen("codificado.txt", "w");
    encodeText(text, codes, encodedFile);
    fclose(encodedFile);

    //le o text do codificado.txt
    FILE *encodedInputFile = fopen("codificado.txt", "r");
    int encodedIndex = 0;
    while ((encodedText[encodedIndex] = fgetc(encodedInputFile)) != EOF) {
        encodedIndex++;
    }
    encodedText[encodedIndex] = '\0';
    fclose(encodedInputFile);

    //decodifica o texto e grava no arquivo
    FILE *decodedFile = fopen("decodificado.txt", "w");

    decodeText(huffmanTree, encodedText, decodedFile);
    fclose(decodedFile);

    return 0;
}
