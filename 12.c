
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_COUNT 6
#define BONUS_SIZE 10000
#define REPEAT_COUNT 10

int sizes[TEST_COUNT] = {
    1000, 2000, 4000, 8000, 16000, 20000
};

// 피벗 선택 방식
#define FIRST 0
#define RANDOM 1
#define MEDIAN3 2

// 숫자 비교 횟수
long long comparisons;

// 두 값 교환
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 중앙값 피벗 선택
int medianOfThree(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;

    int a = arr[low];
    int b = arr[mid];
    int c = arr[high];

    // 중앙값을 찾기 위한 숫자 비교
    comparisons++;
    if (a > b) {
        swap(&a, &b);
    }

    comparisons++;
    if (b > c) {
        swap(&b, &c);
    }

    comparisons++;
    if (a > b) {
        swap(&a, &b);
    }

    return b;
}

// 피벗 선택
int choosePivot(int arr[], int low, int high, int method) {

    if (method == FIRST) {
        return arr[low];
    }

    else if (method == RANDOM) {
        int index = low + rand() % (high - low + 1);
        return arr[index];
    }

    else {
        return medianOfThree(arr, low, high);
    }
}

// 분할 함수
int partition(int arr[], int low, int high, int method) {

    int pivot = choosePivot(arr, low, high, method);

    // 피벗과 같은 값을 가진 원소 중 하나를 끝으로 이동
    int pivotIndex = low;

    for (int i = low; i <= high; i++) {
        if (arr[i] == pivot) {
            pivotIndex = i;
            break;
        }
    }

    swap(&arr[pivotIndex], &arr[high]);

    int i = low - 1;

    for (int j = low; j < high; j++) {

        comparisons++;

        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[high]);

    return i + 1;
}

// 재귀적 퀵 정렬
void quickSortRecursive(int arr[], int low, int high, int method) {

    while (low < high) {

        int pi = partition(arr, low, high, method);

        // 작은 구간부터 재귀 호출하여 스택 사용량 감소
        if (pi - low < high - pi) {
            quickSortRecursive(arr, low, pi - 1, method);
            low = pi + 1;
        }
        else {
            quickSortRecursive(arr, pi + 1, high, method);
            high = pi - 1;
        }
    }
}

// 반복적 퀵 정렬
void quickSortIterative(int arr[], int low, int high, int method) {

    int* stack = (int*)malloc(sizeof(int) * (high - low + 2));

    if (stack == NULL) {
        printf("Stack allocation failed.\n");
        exit(1);
    }

    int top = -1;

    stack[++top] = low;
    stack[++top] = high;

    while (top >= 0) {

        high = stack[top--];
        low = stack[top--];

        if (low < high) {

            int pi = partition(arr, low, high, method);

            if (pi - 1 > low) {
                stack[++top] = low;
                stack[++top] = pi - 1;
            }

            if (pi + 1 < high) {
                stack[++top] = pi + 1;
                stack[++top] = high;
            }
        }
    }

    free(stack);
}

// 배열 복사
void copyArray(int source[], int dest[], int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = source[i];
    }
}

// 오름차순 정렬 확인
int isSorted(int arr[], int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i - 1] > arr[i]) {
            return 0;
        }
    }

    return 1;
}

// 무작위 데이터 생성
void generateData(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000000;
    }
}

// 일반 과제 실행
void runComparison() {

    int* original;
    int* arr;
    long long results[3];

    printf("============================================\n");
    printf(" Quick Sort Pivot Comparison\n");
    printf("============================================\n\n");

    for (int s = 0; s < TEST_COUNT; s++) {

        int n = sizes[s];

        original = (int*)malloc(sizeof(int) * n);
        arr = (int*)malloc(sizeof(int) * n);

        if (original == NULL || arr == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }

        generateData(original, n);

        for (int method = FIRST; method <= MEDIAN3; method++) {

            copyArray(original, arr, n);

            comparisons = 0;

            quickSortRecursive(arr, 0, n - 1, method);

            results[method] = comparisons;

            if (!isSorted(arr, n)) {
                printf("Sorting Error!\n");
                exit(1);
            }
        }

        printf("Data Size : %d\n", n);

        printf("Quick Sort (First)    : %lld comparisons\n",
            results[FIRST]);

        printf("Quick Sort (Random)   : %lld comparisons\n",
            results[RANDOM]);

        printf("Quick Sort (Median-3) : %lld comparisons\n\n",
            results[MEDIAN3]);

        free(original);
        free(arr);
    }
}

// 보너스 실행시간 측정
void runBonus() {

    int* original;
    int* arr;
    clock_t start, end;

    double recursiveTime = 0;
    double iterativeTime = 0;

    original = (int*)malloc(sizeof(int) * BONUS_SIZE);
    arr = (int*)malloc(sizeof(int) * BONUS_SIZE);

    if (original == NULL || arr == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    generateData(original, BONUS_SIZE);

    // 재귀적 퀵 정렬 평균 실행시간
    for (int i = 0; i < REPEAT_COUNT; i++) {

        copyArray(original, arr, BONUS_SIZE);

        start = clock();

        quickSortRecursive(arr, 0, BONUS_SIZE - 1, MEDIAN3);

        end = clock();

        recursiveTime +=
            (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

        if (!isSorted(arr, BONUS_SIZE)) {
            printf("Recursive Sorting Error!\n");
            exit(1);
        }
    }

    // 반복적 퀵 정렬 평균 실행시간
    for (int i = 0; i < REPEAT_COUNT; i++) {

        copyArray(original, arr, BONUS_SIZE);

        start = clock();

        quickSortIterative(arr, 0, BONUS_SIZE - 1, MEDIAN3);

        end = clock();

        iterativeTime +=
            (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

        if (!isSorted(arr, BONUS_SIZE)) {
            printf("Iterative Sorting Error!\n");
            exit(1);
        }
    }

    recursiveTime /= REPEAT_COUNT;
    iterativeTime /= REPEAT_COUNT;

    printf("\n============================================\n");
    printf(" Bonus: Recursive vs Iterative Quick Sort\n");
    printf("============================================\n");

    printf("Data Size : %d\n", BONUS_SIZE);
    printf("Pivot     : Median-of-Three\n");

    printf("Recursive Quick Sort : %.4f ms\n",
        recursiveTime);

    printf("Iterative Quick Sort : %.4f ms\n",
        iterativeTime);

    printf("Average of %d experiments\n", REPEAT_COUNT);

    free(original);
    free(arr);
}

int main() {

    srand((unsigned int)time(NULL));

    runComparison();

    runBonus();

    return 0;
}
