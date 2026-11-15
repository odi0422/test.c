
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE_COUNT 6

int sizes[SIZE_COUNT] = {1000, 2000, 4000, 8000, 16000, 20000};

typedef struct {
    long long comparisons;
    long long movements;
    int maxDepth;
} Stats;

typedef struct AVLNode {
    int key;
    int count;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

typedef struct {
    int insert;
    int value;
} Operation;

typedef struct {
    double time;
    long long comparisons;
    long long movements;
    size_t memory;
    int count;
} Result;

/* 실행 시간 측정 */
double getTime(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

/* 데이터 교환 */
void swap(int* a, int* b, Stats* s) {
    int temp = *a;
    *a = *b;
    *b = temp;
    s->movements += 3;
}

/* Median-of-Three 피벗 선택 */
int medianPivot(int a[], int left, int right, Stats* s) {
    int mid = left + (right - left) / 2;

    s->comparisons++;
    if (a[left] > a[mid])
        swap(&a[left], &a[mid], s);

    s->comparisons++;
    if (a[left] > a[right])
        swap(&a[left], &a[right], s);

    s->comparisons++;
    if (a[mid] > a[right])
        swap(&a[mid], &a[right], s);

    return a[mid];
}

/* 퀵 정렬 */
void quickSort(int a[], int left, int right, Stats* s, int depth) {
    if (left >= right)
        return;

    if (depth > s->maxDepth)
        s->maxDepth = depth;

    int pivot = medianPivot(a, left, right, s);
    s->movements++;

    int i = left;
    int j = right;

    while (i <= j) {
        while (1) {
            s->comparisons++;
            if (a[i] < pivot)
                i++;
            else
                break;
        }

        while (1) {
            s->comparisons++;
            if (a[j] > pivot)
                j--;
            else
                break;
        }

        if (i <= j) {
            if (i != j)
                swap(&a[i], &a[j], s);
            i++;
            j--;
        }
    }

    if (left < j)
        quickSort(a, left, j, s, depth + 1);

    if (i < right)
        quickSort(a, i, right, s, depth + 1);
}

/* 정렬된 배열에 삽입 */
void arrayInsert(int a[], int* n, int value, Stats* s) {
    int pos = 0;

    while (pos < *n) {
        s->comparisons++;
        if (a[pos] >= value)
            break;
        pos++;
    }

    for (int i = *n; i > pos; i--) {
        a[i] = a[i - 1];
        s->movements++;
    }

    a[pos] = value;
    s->movements++;
    (*n)++;
}

/* 정렬된 배열에서 삭제 */
void arrayDelete(int a[], int* n, int value, Stats* s) {
    int pos = 0;

    while (pos < *n) {
        s->comparisons++;

        if (a[pos] == value)
            break;

        if (a[pos] > value)
            return;

        pos++;
    }

    if (pos == *n)
        return;

    for (int i = pos; i < *n - 1; i++) {
        a[i] = a[i + 1];
        s->movements++;
    }

    (*n)--;
}

/* AVL 트리 함수 */
int height(AVLNode* node) {
    return node ? node->height : 0;
}

int max(int a, int b) {
    return a > b ? a : b;
}

void updateHeight(AVLNode* node) {
    node->height = 1 + max(height(node->left), height(node->right));
}

int balanceFactor(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

/* 오른쪽 회전 */
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* t = x->right;

    x->right = y;
    y->left = t;

    updateHeight(y);
    updateHeight(x);

    return x;
}

/* 왼쪽 회전 */
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* t = y->left;

    y->left = x;
    x->right = t;

    updateHeight(x);
    updateHeight(y);

    return y;
}

/* AVL 균형 복구 */
AVLNode* rebalance(AVLNode* node) {
    if (!node)
        return NULL;

    updateHeight(node);

    int balance = balanceFactor(node);

    if (balance > 1) {
        if (balanceFactor(node->left) < 0)
            node->left = rotateLeft(node->left);

        return rotateRight(node);
    }

    if (balance < -1) {
        if (balanceFactor(node->right) > 0)
            node->right = rotateRight(node->right);

        return rotateLeft(node);
    }

    return node;
}

/* AVL 삽입 */
AVLNode* avlInsert(AVLNode* node, int value, Stats* s, int depth,
                  int* nodeCount) {
    if (!node) {
        AVLNode* newNode = (AVLNode*)malloc(sizeof(AVLNode));

        if (!newNode) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(1);
        }

        newNode->key = value;
        newNode->count = 1;
        newNode->height = 1;
        newNode->left = NULL;
        newNode->right = NULL;

        s->movements++;
        (*nodeCount)++;

        return newNode;
    }

    if (depth > s->maxDepth)
        s->maxDepth = depth;

    s->comparisons++;

    if (value < node->key) {
        node->left = avlInsert(node->left, value, s,
                               depth + 1, nodeCount);
    }
    else {
        s->comparisons++;

        if (value > node->key) {
            node->right = avlInsert(node->right, value, s,
                                    depth + 1, nodeCount);
        }
        else {
            node->count++;
            s->movements++;
            return node;
        }
    }

    return rebalance(node);
}

/* 가장 작은 노드를 분리 */
AVLNode* detachMin(AVLNode* node, AVLNode** minNode) {
    if (!node->left) {
        *minNode = node;
        return node->right;
    }

    node->left = detachMin(node->left, minNode);

    return rebalance(node);
}

/* AVL 삭제 */
AVLNode* avlDelete(AVLNode* node, int value, Stats* s,
                   int* nodeCount, int* deleted) {
    if (!node)
        return NULL;

    s->comparisons++;

    if (value < node->key) {
        node->left = avlDelete(node->left, value, s,
                               nodeCount, deleted);
    }
    else {
        s->comparisons++;

        if (value > node->key) {
            node->right = avlDelete(node->right, value, s,
                                    nodeCount, deleted);
        }
        else {
            *deleted = 1;

            if (node->count > 1) {
                node->count--;
                s->movements++;
                return node;
            }

            AVLNode* left = node->left;
            AVLNode* right = node->right;

            free(node);
            (*nodeCount)--;

            if (!right)
                return left;

            AVLNode* successor = NULL;
            right = detachMin(right, &successor);

            successor->left = left;
            successor->right = right;

            return rebalance(successor);
        }
    }

    return rebalance(node);
}

/* AVL 중위 순회 */
void inorder(AVLNode* node, int a[], int* index) {
    if (!node)
        return;

    inorder(node->left, a, index);

    for (int i = 0; i < node->count; i++)
        a[(*index)++] = node->key;

    inorder(node->right, a, index);
}

/* AVL 메모리 해제 */
void freeAVL(AVLNode* node) {
    if (!node)
        return;

    freeAVL(node->left);
    freeAVL(node->right);
    free(node);
}

/* 배열과 AVL 결과 비교 */
int verify(AVLNode* root, int a[], int n) {
    int* temp = (int*)malloc((size_t)n * sizeof(int));

    if (!temp)
        return 0;

    int index = 0;
    inorder(root, temp, &index);

    int valid = (index == n);

    for (int i = 0; valid && i < n; i++) {
        if (a[i] != temp[i])
            valid = 0;
    }

    free(temp);
    return valid;
}

/* 원본 데이터 복사 */
void copyData(int source[], int dest[], int n) {
    memcpy(dest, source, (size_t)n * sizeof(int));
}

/* 퀵 정렬 + 삽입 및 삭제 */
Result runQuick(int original[], int n, Operation ops[], int opCount) {
    Result result = {0};
    Stats stats = {0};

    int capacity = n + opCount;

    int* a = (int*)malloc((size_t)capacity * sizeof(int));

    if (!a) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    copyData(original, a, n);

    double start = getTime();

    quickSort(a, 0, n - 1, &stats, 1);

    int current = n;

    for (int i = 0; i < opCount; i++) {
        if (ops[i].insert)
            arrayInsert(a, &current, ops[i].value, &stats);
        else
            arrayDelete(a, &current, ops[i].value, &stats);
    }

    double end = getTime();

    result.time = end - start;
    result.comparisons = stats.comparisons;
    result.movements = stats.movements;
    result.count = current;

    /* 배열 공간 + 퀵 정렬 재귀 스택 추정치 */
    result.memory = (size_t)capacity * sizeof(int)
                  + (size_t)stats.maxDepth * 32;

    free(a);

    return result;
}

/* AVL 트리 정렬 + 삽입 및 삭제 */
Result runAVL(int original[], int n, Operation ops[], int opCount) {
    Result result = {0};
    Stats stats = {0};

    AVLNode* root = NULL;
    int nodeCount = 0;

    double start = getTime();

    for (int i = 0; i < n; i++) {
        root = avlInsert(root, original[i], &stats, 1, &nodeCount);
    }

    int current = n;

    for (int i = 0; i < opCount; i++) {
        if (ops[i].insert) {
            root = avlInsert(root, ops[i].value, &stats, 1,
                             &nodeCount);
            current++;
        }
        else {
            int deleted = 0;

            root = avlDelete(root, ops[i].value, &stats,
                             &nodeCount, &deleted);

            if (deleted)
                current--;
        }
    }

    double end = getTime();

    result.time = end - start;
    result.comparisons = stats.comparisons;
    result.movements = stats.movements;
    result.count = current;

    /* 실제 노드 공간 + AVL 재귀 스택 추정치 */
    result.memory = (size_t)nodeCount * sizeof(AVLNode)
                  + (size_t)stats.maxDepth * 32;

    freeAVL(root);

    return result;
}

/* 메인 함수 */
int main(void) {
    srand((unsigned int)time(NULL));

    printf("Quick Sort vs AVL Tree Sort\n");
    printf("Pivot Method: Median-of-Three\n");
    printf("========================================\n");

    for (int k = 0; k < SIZE_COUNT; k++) {
        int n = sizes[k];
        int opCount = n / 5;

        int* original = (int*)malloc((size_t)n * sizeof(int));
        Operation* ops = (Operation*)malloc(
            (size_t)opCount * sizeof(Operation));

        if (!original || !ops) {
            fprintf(stderr, "Memory allocation failed.\n");
            free(original);
            free(ops);
            return 1;
        }

        /* 원본 데이터 생성 */
        for (int i = 0; i < n; i++)
            original[i] = rand() % 100001;

        /* 동일한 연산 순서와 값 생성 */
        for (int i = 0; i < opCount; i++) {
            int choice = rand();

            ops[i].insert = (choice % 2 == 0);
            ops[i].value = rand() % 100001;
        }

        Result quick = runQuick(original, n, ops, opCount);
        Result avl = runAVL(original, n, ops, opCount);

        /* 최종 결과 검증 */
        int capacity = n + opCount;
        int* check = (int*)malloc((size_t)capacity * sizeof(int));

        if (!check) {
            fprintf(stderr, "Memory allocation failed.\n");
            free(original);
            free(ops);
            return 1;
        }

        copyData(original, check, n);

        for (int i = 0; i < opCount; i++) {
            if (ops[i].insert)
                arrayInsert(check, &n, ops[i].value,
                            &(Stats){0});
            else
                arrayDelete(check, &n, ops[i].value,
                            &(Stats){0});
        }

        int valid = (n == quick.count && n == avl.count);

        /* AVL 결과를 별도로 재구성하여 비교 */
        AVLNode* verifyRoot = NULL;
        Stats verifyStats = {0};
        int verifyNodeCount = 0;

        /* 동일 원본 및 연산 재실행 */
        int originalSize = sizes[k];

        for (int i = 0; i < originalSize; i++) {
            verifyRoot = avlInsert(verifyRoot, original[i],
                                   &verifyStats, 1, &verifyNodeCount);
        }

        int verifyCount = originalSize;

        for (int i = 0; i < opCount; i++) {
            if (ops[i].insert) {
                verifyRoot = avlInsert(verifyRoot, ops[i].value,
                                       &verifyStats, 1,
                                       &verifyNodeCount);
                verifyCount++;
            }
            else {
                int deleted = 0;
                verifyRoot = avlDelete(verifyRoot, ops[i].value,
                                       &verifyStats, &verifyNodeCount,
                                       &deleted);
                if (deleted)
                    verifyCount--;
            }
        }

        valid = valid && verifyCount == n
              && verify(verifyRoot, check, n);

        printf("\nData Size : %d    Operations: %d\n", originalSize,
               opCount);
        printf("------------------------------------------------------\n");
        printf("%-20s %-18s %-18s\n", "", "Quick Sort", "AVL Tree");
        printf("------------------------------------------------------\n");

        printf("%-20s %-18.3f %-18.3f\n",
               "Total Time (ms)", quick.time, avl.time);

        printf("%-20s %-18lld %-18lld\n",
               "Comparisons", quick.comparisons, avl.comparisons);

        printf("%-20s %-18lld %-18lld\n",
               "Data Movements", quick.movements, avl.movements);

        printf("%-20s %-18zu %-18zu\n",
               "Memory Usage (B)", quick.memory, avl.memory);

        printf("%-20s %-18d %-18d\n",
               "Final Data Count", quick.count, avl.count);

        printf("------------------------------------------------------\n");
        printf("Data Verification: %s\n", valid ? "PASS" : "FAIL");

        freeAVL(verifyRoot);
        free(check);
        free(original);
        free(ops);
    }

    printf("\nAll experiments completed.\n");

    return 0;
}
