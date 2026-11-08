
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TEST_COUNT 6

#define FIRST 0
#define RANDOM 1
#define MEDIAN3 2

int sizes[TEST_COUNT] = {
    1000, 2000, 4000, 8000, 16000, 20000
};

long long comparisons;

// 재귀 스택 메모리 계산용 구조체
typedef struct {
    int left;
    int mid;
    int right;
} MergeFrame;

typedef struct {
    int low;
    int high;
} QuickFrame;

// 측정 결과 저장
typedef struct {
    long long comparisons;
    double time;
    size_t memory;
} Result;

// 두 값 교환
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 배열 복사
void copyArray(int source[], int dest[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = source[i];
    }
}

// 정렬 확인
int isSorted(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return 0;
        }
    }
    return 1;
}

// 무작위 데이터 생성
void generateData(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000000;
    }
}

// ============================================
// Merge Sort
// ============================================

int maxMergeDepth;

void merge(int arr[], int temp[], int left,
    int mid, int right) {

    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {

        comparisons++;

        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        }
        else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }
}

void mergeSortRecursive(int arr[], int temp[],
    int left, int right, int depth) {

    if (depth > maxMergeDepth) {
        maxMergeDepth = depth;
    }

    if (left < right) {

        int mid = left + (right - left) / 2;

        mergeSortRecursive(
            arr, temp, left, mid, depth + 1
        );

        mergeSortRecursive(
            arr, temp, mid + 1, right, depth + 1
        );

        merge(arr, temp, left, mid, right);
    }
}

void mergeSort(int arr[], int n) {

    int* temp = (int*)malloc(sizeof(int) * n);

    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    maxMergeDepth = 1;

    mergeSortRecursive(arr, temp, 0, n - 1, 1);

    free(temp);
}

// ============================================
// Quick Sort
// ============================================

// Median-of-Three 피벗 선택
int medianOfThree(int arr[], int low, int high) {

    int a = low;
    int b = low + (high - low) / 2;
    int c = high;

    comparisons++;
    if (arr[a] > arr[b]) {
        int t = a;
        a = b;
        b = t;
    }

    comparisons++;
    if (arr[b] > arr[c]) {
        int t = b;
        b = c;
        c = t;
    }

    comparisons++;
    if (arr[a] > arr[b]) {
        int t = a;
        a = b;
        b = t;
    }

    return b;
}

// 피벗 위치 선택
int choosePivot(int arr[], int low, int high,
    int method) {

    if (method == FIRST) {
        return low;
    }

    else if (method == RANDOM) {
        return low + rand() % (high - low + 1);
    }

    else {
        return medianOfThree(arr, low, high);
    }
}

// 분할 함수
int partition(int arr[], int low, int high,
    int method) {

    int pivotIndex = choosePivot(
        arr, low, high, method
    );

    swap(&arr[pivotIndex], &arr[high]);

    int pivot = arr[high];
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

// 최대 재귀 깊이
int maxQuickDepth;

// 재귀적 퀵 정렬
void quickSortRecursive(int arr[], int low,
    int high, int method, int depth) {

    if (depth > maxQuickDepth) {
        maxQuickDepth = depth;
    }

    while (low < high) {

        int pi = partition(
            arr, low, high, method
        );

        // 작은 구간을 먼저 재귀 호출
        if (pi - low < high - pi) {

            quickSortRecursive(
                arr, low, pi - 1,
                method, depth + 1
            );

            low = pi + 1;
        }
        else {

            quickSortRecursive(
                arr, pi + 1, high,
                method, depth + 1
            );

            high = pi - 1;
        }
    }
}

void quickSort(int arr[], int n, int method) {

    maxQuickDepth = 1;

    quickSortRecursive(
        arr, 0, n - 1, method, 1
    );
}

// ============================================
// 성능 측정
// ============================================

Result measureMerge(
    int original[], int arr[], int n
) {

    Result result;

    copyArray(original, arr, n);

    comparisons = 0;

    clock_t start = clock();

    mergeSort(arr, n);

    clock_t end = clock();

    result.comparisons = comparisons;

    result.time =
        (double)(end - start) * 1000.0
        / CLOCKS_PER_SEC;

    // 임시 배열 + 재귀 호출 스택 추정 메모리
    result.memory =
        sizeof(int) * n
        + sizeof(MergeFrame) * maxMergeDepth;

    if (!isSorted(arr, n)) {
        printf("Merge Sort Error!\n");
        exit(1);
    }

    return result;
}

Result measureQuick(
    int original[], int arr[],
    int n, int method
) {

    Result result;

    copyArray(original, arr, n);

    comparisons = 0;

    clock_t start = clock();

    quickSort(arr, n, method);

    clock_t end = clock();

    result.comparisons = comparisons;

    result.time =
        (double)(end - start) * 1000.0
        / CLOCKS_PER_SEC;

    // 재귀 호출 스택 추정 메모리
    result.memory =
        sizeof(QuickFrame) * maxQuickDepth;

    if (!isSorted(arr, n)) {
        printf("Quick Sort Error!\n");
        exit(1);
    }

    return result;
}

// ============================================
// 결과 출력
// ============================================

void printResult(
    const char* name, Result result
) {

    printf("%-16s %12lld %12.4f %14zu\n",
        name,
        result.comparisons,
        result.time,
        result.memory
    );
}

// ============================================
// Main
// ============================================

int main() {

    srand((unsigned int)time(NULL));

    printf("============================================\n");
    printf(" Merge Sort vs Quick Sort Comparison\n");
    printf("============================================\n");

    for (int s = 0; s < TEST_COUNT; s++) {

        int n = sizes[s];

        int* original =
            (int*)malloc(sizeof(int) * n);

        int* arr =
            (int*)malloc(sizeof(int) * n);

        if (original == NULL || arr == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }

        generateData(original, n);

        printf("\nData Size : %d\n", n);

        printf("%-16s %12s %12s %14s\n",
            "Algorithm",
            "Comparisons",
            "Time(ms)",
            "Memory(Bytes)"
        );

        printf("------------------------------------------------------------\n");

        // Merge Sort
        Result mergeResult =
            measureMerge(original, arr, n);

        printResult("Merge Sort", mergeResult);

        // Quick Sort - First
        Result firstResult =
            measureQuick(original, arr, n, FIRST);

        printResult("Quick (First)", firstResult);

        // Quick Sort - Random
        Result randomResult =
            measureQuick(original, arr, n, RANDOM);

        printResult("Quick (Random)", randomResult);

        // Quick Sort - Median-of-Three
        Result medianResult =
            measureQuick(original, arr, n, MEDIAN3);

        printResult("Quick (Median3)", medianResult);

        free(original);
        free(arr);
    }

    printf("\n============================================\n");
    printf(" All Sorting Tests Completed Successfully\n");
    printf("============================================\n");

    return 0;
}
