#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUE 1000
#define GENERATE_COUNT 100
#define SEARCH_COUNT 50

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
    int height;
} Node;

/* 생성 과정 비교 횟수 */
long long arrayInsertComparisons = 0;
long long bstInsertComparisons = 0;
long long avlInsertComparisons = 0;

/* 탐색 비교 횟수 */
long long sequentialTotal = 0;
long long bstSearchTotal = 0;
long long avlSearchTotal = 0;

/* ============================= */
/* 노드 생성 */
/* ============================= */

Node *createNode(int data)
{
    Node *newNode = (Node *)malloc(sizeof(Node));

    if (newNode == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 1;

    return newNode;
}

/* ============================= */
/* 배열 삽입 */
/* ============================= */

int insertArray(int arr[], int *size, int data)
{
    int i;

    for (i = 0; i < *size; i++) {
        arrayInsertComparisons++;

        if (arr[i] == data) {
            return 0;
        }
    }

    arr[*size] = data;
    (*size)++;

    return 1;
}

/* ============================= */
/* BST 삽입 */
/* ============================= */

Node *insertBST(Node *root, int data)
{
    if (root == NULL) {
        return createNode(data);
    }

    bstInsertComparisons++;

    if (data < root->data) {
        root->left = insertBST(root->left, data);
    }
    else if (data > root->data) {
        root->right = insertBST(root->right, data);
    }
    else {
        return root;
    }

    return root;
}

/* ============================= */
/* AVL 관련 함수 */
/* ============================= */

int getHeight(Node *node)
{
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int getBalance(Node *node)
{
    if (node == NULL) {
        return 0;
    }

    return getHeight(node->left) - getHeight(node->right);
}

/* 오른쪽 회전 */
Node *rotateRight(Node *y)
{
    Node *x = y->left;
    Node *temp = x->right;

    x->right = y;
    y->left = temp;

    y->height =
        1 + max(getHeight(y->left), getHeight(y->right));

    x->height =
        1 + max(getHeight(x->left), getHeight(x->right));

    return x;
}

/* 왼쪽 회전 */
Node *rotateLeft(Node *x)
{
    Node *y = x->right;
    Node *temp = y->left;

    y->left = x;
    x->right = temp;

    x->height =
        1 + max(getHeight(x->left), getHeight(x->right));

    y->height =
        1 + max(getHeight(y->left), getHeight(y->right));

    return y;
}

/* AVL 삽입 */
Node *insertAVL(Node *root, int data)
{
    int balance;

    if (root == NULL) {
        return createNode(data);
    }

    avlInsertComparisons++;

    if (data < root->data) {
        root->left = insertAVL(root->left, data);
    }
    else if (data > root->data) {
        root->right = insertAVL(root->right, data);
    }
    else {
        return root;
    }

    root->height =
        1 + max(getHeight(root->left),
                getHeight(root->right));

    balance = getBalance(root);

    /* LL */
    if (balance > 1 && data < root->left->data) {
        return rotateRight(root);
    }

    /* RR */
    if (balance < -1 && data > root->right->data) {
        return rotateLeft(root);
    }

    /* LR */
    if (balance > 1 && data > root->left->data) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    /* RL */
    if (balance < -1 && data < root->right->data) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

/* ============================= */
/* 순차 탐색 */
/* ============================= */

int sequentialSearch(int arr[], int size, int key, int *comparisons)
{
    int i;

    *comparisons = 0;

    for (i = 0; i < size; i++) {
        (*comparisons)++;

        if (arr[i] == key) {
            return 1;
        }
    }

    return 0;
}

/* ============================= */
/* BST 탐색 */
/* ============================= */

int bstSearch(Node *root, int key, int *comparisons)
{
    Node *current = root;

    *comparisons = 0;

    while (current != NULL) {
        (*comparisons)++;

        if (key == current->data) {
            return 1;
        }
        else if (key < current->data) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return 0;
}

/* ============================= */
/* AVL 탐색 */
/* ============================= */

int avlSearch(Node *root, int key, int *comparisons)
{
    Node *current = root;

    *comparisons = 0;

    while (current != NULL) {
        (*comparisons)++;

        if (key == current->data) {
            return 1;
        }
        else if (key < current->data) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return 0;
}

/* ============================= */
/* 트리 높이 */
/* ============================= */

int getTreeHeight(Node *root)
{
    if (root == NULL) {
        return 0;
    }

    int leftHeight = getTreeHeight(root->left);
    int rightHeight = getTreeHeight(root->right);

    return 1 + max(leftHeight, rightHeight);
}

/* ============================= */
/* 트리 메모리 해제 */
/* ============================= */

void freeTree(Node *root)
{
    if (root == NULL) {
        return;
    }

    freeTree(root->left);
    freeTree(root->right);

    free(root);
}

/* ============================= */
/* 배열 출력 */
/* ============================= */

void printArray(int arr[], int size)
{
    int i;

    for (i = 0; i < size; i++) {
        printf("%4d", arr[i]);

        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
        else {
            printf(" ");
        }
    }
}

/* ============================= */
/* 메인 함수 */
/* ============================= */

int main(void)
{
    int array[GENERATE_COUNT];

    int arraySize = 0;

    Node *bstRoot = NULL;
    Node *avlRoot = NULL;

    int searchKeys[SEARCH_COUNT];

    int i;
    int value;

    int duplicateCount = 0;

    srand((unsigned int)time(NULL));

    /* ------------------------- */
    /* 1. 100개의 난수 생성 */
    /* ------------------------- */

    for (i = 0; i < GENERATE_COUNT; i++) {

        value = rand() % (MAX_VALUE + 1);

        /*
         * 같은 난수를 세 자료구조에 동일하게 처리
         */

        if (!insertArray(array, &arraySize, value)) {
            duplicateCount++;
        }

        bstRoot = insertBST(bstRoot, value);

        avlRoot = insertAVL(avlRoot, value);
    }

    /* ------------------------- */
    /* 2. 생성된 데이터 출력 */
    /* ------------------------- */

    printf("============================================\n");
    printf("생성된 100개의 난수 중 저장된 값\n");
    printf("============================================\n");

    printArray(array, arraySize);

    printf("\n============================================\n");
    printf("데이터 저장 결과\n");
    printf("============================================\n");

    printf("Stored values       : %d\n", arraySize);
    printf("Duplicate values    : %d\n", duplicateCount);

    /* ------------------------- */
    /* 3. 생성 과정 비교 횟수 */
    /* ------------------------- */

    printf("\n============================================\n");
    printf("생성 과정 비교 횟수\n");
    printf("============================================\n");

    printf("Array comparisons : %lld\n",
           arrayInsertComparisons);

    printf("BST comparisons   : %lld\n",
           bstInsertComparisons);

    printf("AVL comparisons   : %lld\n",
           avlInsertComparisons);

    /* ------------------------- */
    /* 4. 자료구조 크기와 높이 */
    /* ------------------------- */

    printf("\n============================================\n");
    printf("자료구조 정보\n");
    printf("============================================\n");

    printf("Array length : %d\n", arraySize);

    printf("BST height   : %d\n",
           getTreeHeight(bstRoot));

    printf("AVL height   : %d\n",
           getTreeHeight(avlRoot));

    /* ------------------------- */
    /* 5. 탐색 대상 50개 생성 */
    /* ------------------------- */

    for (i = 0; i < SEARCH_COUNT; i++) {
        searchKeys[i] =
            rand() % (MAX_VALUE + 1);
    }

    printf("\n============================================\n");
    printf("생성된 50개의 탐색 대상\n");
    printf("============================================\n");

    for (i = 0; i < SEARCH_COUNT; i++) {
        printf("%4d", searchKeys[i]);

        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
        else {
            printf(" ");
        }
    }

    /* ------------------------- */
    /* 6. 탐색 수행 */
    /* ------------------------- */

    printf("\n============================================\n");
    printf("탐색 결과\n");
    printf("============================================\n");

    printf("%-10s %-12s %-12s %-12s %-12s\n",
           "Search Key",
           "Seq Result",
           "Seq Comp.",
           "BST Comp.",
           "AVL Comp.");

    printf("----------------------------------------------------------------\n");

    for (i = 0; i < SEARCH_COUNT; i++) {

        int seqComparisons;
        int bstComparisons;
        int avlComparisons;

        int seqResult;
        int bstResult;
        int avlResult;

        seqResult =
            sequentialSearch(
                array,
                arraySize,
                searchKeys[i],
                &seqComparisons
            );

        bstResult =
            bstSearch(
                bstRoot,
                searchKeys[i],
                &bstComparisons
            );

        avlResult =
            avlSearch(
                avlRoot,
                searchKeys[i],
                &avlComparisons
            );

        sequentialTotal += seqComparisons;
        bstSearchTotal += bstComparisons;
        avlSearchTotal += avlComparisons;

        printf("%-10d %-12s %-12d %-12d %-12d\n",
               searchKeys[i],
               seqResult ? "Found" : "Not Found",
               seqComparisons,
               bstComparisons,
               avlComparisons);

        /*
         * 세 자료구조의 탐색 결과가
         * 서로 다른 경우 오류 확인
         */
        if (seqResult != bstResult ||
            seqResult != avlResult) {

            printf("※ 탐색 결과 불일치 오류\n");
        }
    }

    /* ------------------------- */
    /* 7. 탐색 통계 */
    /* ------------------------- */

    printf("\n============================================\n");
    printf("탐색 결과 통계\n");
    printf("============================================\n");

    printf("Number of searches : %d\n",
           SEARCH_COUNT);

    printf("\n[Sequential Search]\n");
    printf("Total comparisons   : %lld\n",
           sequentialTotal);

    printf("Average comparisons : %.2f\n",
           (double)sequentialTotal / SEARCH_COUNT);

    printf("\n[BST Search]\n");
    printf("Total comparisons   : %lld\n",
           bstSearchTotal);

    printf("Average comparisons : %.2f\n",
           (double)bstSearchTotal / SEARCH_COUNT);

    printf("\n[AVL Search]\n");
    printf("Total comparisons   : %lld\n",
           avlSearchTotal);

    printf("Average comparisons : %.2f\n",
           (double)avlSearchTotal / SEARCH_COUNT);

    /* ------------------------- */
    /* 8. 최종 비교 */
    /* ------------------------- */

    printf("\n============================================\n");
    printf("전체 비교 결과\n");
    printf("============================================\n");

    printf("Array Construction : %lld\n",
           arrayInsertComparisons);

    printf("BST Construction   : %lld\n",
           bstInsertComparisons);

    printf("AVL Construction   : %lld\n",
           avlInsertComparisons);

    printf("\nArray Search Total : %lld\n",
           sequentialTotal);

    printf("BST Search Total   : %lld\n",
           bstSearchTotal);

    printf("AVL Search Total   : %lld\n",
           avlSearchTotal);

    /* ------------------------- */
    /* 9. 메모리 해제 */
    /* ------------------------- */

    freeTree(bstRoot);
    freeTree(avlRoot);

    return 0;
}
