#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_COUNT 100
#define SEARCH_COUNT 50
#define MIN_VALUE 0
#define MAX_VALUE 1000

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

/* BST 생성 비교 횟수 */
long long bstInsertComparisons = 0;

/* BST 노드 생성 */
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

    return newNode;
}

/* BST에 데이터 삽입 */
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

    return root;
}

/* 순차 탐색 */
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

/* BST 탐색 */
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

/* BST 메모리 해제 */
void freeBST(Node *root)
{
    if (root == NULL) {
        return;
    }

    freeBST(root->left);
    freeBST(root->right);

    free(root);
}

/* 배열 출력 */
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

int main(void)
{
    int data[DATA_COUNT];
    int searchKeys[SEARCH_COUNT];

    int used[MAX_VALUE + 1] = {0};

    Node *root = NULL;

    int i;
    int value;

    long long sequentialTotal = 0;
    long long bstTotal = 0;

    double sequentialAverage;
    double bstAverage;

    srand((unsigned int)time(NULL));

    /*
     * 1. 0~1000 사이의 서로 다른 정수 100개 생성
     */
    for (i = 0; i < DATA_COUNT; i++) {
        do {
            value = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
        } while (used[value]);

        used[value] = 1;
        data[i] = value;
    }

    /*
     * 2. 생성된 데이터를 발생 순서 그대로 BST에 삽입
     */
    for (i = 0; i < DATA_COUNT; i++) {
        root = insertBST(root, data[i]);
    }

    /*
     * 3. 탐색 대상 50개 생성
     *    탐색 대상은 기존 데이터와 중복되어도 허용
     */
    for (i = 0; i < SEARCH_COUNT; i++) {
        searchKeys[i] =
            rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
    }

    /*
     * 4. 생성된 100개 데이터 출력
     */
    printf("============================================\n");
    printf("생성된 100개의 정수\n");
    printf("============================================\n");

    printArray(data, DATA_COUNT);

    /*
     * 5. BST 생성 비교 횟수 출력
     */
    printf("\n============================================\n");
    printf("BST 생성 과정 비교 횟수\n");
    printf("============================================\n");

    printf("BST Creation Total Comparisons : %lld\n",
           bstInsertComparisons);

    /*
     * 6. 탐색 대상 출력
     */
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

    /*
     * 7. 각각의 탐색 수행
     */
    printf("\n============================================\n");
    printf("탐색 결과\n");
    printf("============================================\n");

    printf("%-10s %-20s %-15s %-15s\n",
           "Search Key",
           "Sequential Result",
           "Sequential Comp.",
           "BST Comp.");

    printf("--------------------------------------------------------------\n");

    for (i = 0; i < SEARCH_COUNT; i++) {
        int seqComparisons;
        int bstComparisons;

        int seqResult;
        int bstResult;

        seqResult =
            sequentialSearch(
                data,
                DATA_COUNT,
                searchKeys[i],
                &seqComparisons
            );

        bstResult =
            bstSearch(
                root,
                searchKeys[i],
                &bstComparisons
            );

        sequentialTotal += seqComparisons;
        bstTotal += bstComparisons;

        printf("%-10d %-20s %-15d %-15d\n",
               searchKeys[i],
               seqResult ? "Found" : "Not Found",
               seqComparisons,
               bstComparisons);

        /*
         * BST 탐색 성공/실패도 함께 확인
         */
        if (seqResult != bstResult) {
            printf("※ 탐색 결과 오류 발생\n");
        }
    }

    /*
     * 8. 평균 계산
     */
    sequentialAverage =
        (double)sequentialTotal / SEARCH_COUNT;

    bstAverage =
        (double)bstTotal / SEARCH_COUNT;

    /*
     * 9. 최종 통계 출력
     */
    printf("\n============================================\n");
    printf("탐색 결과 통계\n");
    printf("============================================\n");

    printf("Number of searches : %d\n", SEARCH_COUNT);

    printf("\n[Sequential Search]\n");
    printf("Total Comparisons   : %lld\n", sequentialTotal);
    printf("Average Comparisons : %.2f\n", sequentialAverage);

    printf("\n[BST Search]\n");
    printf("Total Comparisons   : %lld\n", bstTotal);
    printf("Average Comparisons : %.2f\n", bstAverage);

    /*
     * 10. BST 생성 비용을 포함한 전체 비교 비용
     */
    printf("\n============================================\n");
    printf("전체 비용 비교\n");
    printf("============================================\n");

    printf("Sequential Search Total Cost : %lld\n",
           sequentialTotal);

    printf("BST Creation Cost            : %lld\n",
           bstInsertComparisons);

    printf("BST Search Total Cost        : %lld\n",
           bstTotal);

    printf("BST Creation + Search Cost   : %lld\n",
           bstInsertComparisons + bstTotal);

    printf("\n");

    if (sequentialTotal <
        bstInsertComparisons + bstTotal) {

        printf("결과 : 생성 비용까지 고려하면 "
               "순차 탐색의 비교 횟수가 더 적습니다.\n");
    }
    else if (sequentialTotal >
             bstInsertComparisons + bstTotal) {

        printf("결과 : 생성 비용까지 고려해도 "
               "BST 탐색의 비교 횟수가 더 적습니다.\n");
    }
    else {
        printf("결과 : 두 방법의 전체 비교 횟수가 같습니다.\n");
    }

    /*
     * 11. BST 메모리 해제
     */
    freeBST(root);

    return 0;
}
