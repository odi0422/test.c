
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE 20000

typedef long long LL;

int original[MAX_SIZE];

typedef struct {
    LL comparisons;
    double time_ms;
    size_t memory;
} Result;

/* 재귀 호출 스택 메모리 추정용 구조체 */
typedef struct {
    int lo, hi, pivot;
    int lt, i, gt;
} QuickFrame;

typedef struct {
    int left, right, mid;
} MergeFrame;

/* 숫자 비교 횟수 */
LL comparisons;

/* 최대 재귀 깊이 */
int current_depth;
int max_depth;

/* 비교 함수 */
int less_than(int a, int b)
{
    comparisons++;
    return a < b;
}

int greater_than(int a, int b)
{
    comparisons++;
    return a > b;
}

/* 재귀 깊이 관리 */
void enter_depth(void)
{
    current_depth++;

    if (current_depth > max_depth)
        max_depth = current_depth;
}

void leave_depth(void)
{
    current_depth--;
}

/* 정렬 결과 확인 */
int is_sorted(int arr[], int n)
{
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i])
            return 0;
    }

    return 1;
}

/* 데이터 생성 */
void generate_data(int n)
{
    for (int i = 0; i < n; i++)
        original[i] = rand() % 1000001;
}

/* 결과 출력 */
void print_result(const char* name, Result r)
{
    printf("%-18s %-14lld %-12.3f %-14zu\n",
        name, r.comparisons, r.time_ms, r.memory);
}

/* =========================================
   1. Heap Sort
   ========================================= */

/* 최대 힙 속성 복구 */
void heapify(int arr[], int size, int root)
{
    while (1) {
        int largest = root;
        int left = 2 * root + 1;
        int right = 2 * root + 2;

        if (left < size &&
            greater_than(arr[left], arr[largest]))
            largest = left;

        if (right < size &&
            greater_than(arr[right], arr[largest]))
            largest = right;

        if (largest == root)
            break;

        int temp = arr[root];
        arr[root] = arr[largest];
        arr[largest] = temp;

        root = largest;
    }
}

void heap_sort(int arr[], int n)
{
    /* 최대 힙 구성 */
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    /* 최댓값을 마지막으로 이동 */
    for (int i = n - 1; i > 0; i--) {
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        heapify(arr, i, 0);
    }
}

Result run_heap(int arr[], int n)
{
    Result r;

    comparisons = 0;

    clock_t start = clock();

    heap_sort(arr, n);

    clock_t end = clock();

    r.comparisons = comparisons;
    r.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    /* 반복형 힙 정렬: 추가 동적 메모리 없음.
       보조 변수의 크기를 기준으로 계산 */
    r.memory = sizeof(int) * 5;

    return r;
}

/* =========================================
   2. Merge Sort
   ========================================= */

void merge(int arr[], int temp[], int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (!greater_than(arr[i], arr[j]))
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

void merge_sort(int arr[], int temp[], int left, int right)
{
    if (left >= right)
        return;

    enter_depth();

    int mid = left + (right - left) / 2;

    merge_sort(arr, temp, left, mid);
    merge_sort(arr, temp, mid + 1, right);

    merge(arr, temp, left, mid, right);

    leave_depth();
}

Result run_merge(int arr[], int n)
{
    Result r;

    comparisons = 0;
    current_depth = 0;
    max_depth = 0;

    clock_t start = clock();

    int* temp = (int*)malloc(sizeof(int) * n);

    if (temp == NULL) {
        printf("Merge Sort memory allocation failed.\n");
        exit(1);
    }

    merge_sort(arr, temp, 0, n - 1);

    free(temp);

    clock_t end = clock();

    r.comparisons = comparisons;
    r.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    /* 임시 배열 + 재귀 호출 스택 추정 */
    r.memory = sizeof(int) * n
        + sizeof(MergeFrame) * (max_depth + 1);

    return r;
}

/* =========================================
   3. Quick Sort
   ========================================= */

/* 피벗 선택 방법 */
#define FIRST 0
#define RANDOM 1
#define MEDIAN3 2

/* 세 원소의 중앙값 선택 */
int median_of_three(int a, int b, int c)
{
    if (less_than(a, b)) {
        if (less_than(b, c))
            return b;

        if (less_than(a, c))
            return c;

        return a;
    }
    else {
        if (less_than(a, c))
            return a;

        if (less_than(b, c))
            return c;

        return b;
    }
}

/* 피벗 선택 */
int choose_pivot(int arr[], int low, int high, int method)
{
    if (method == FIRST)
        return arr[low];

    if (method == RANDOM) {
        int index = low + rand() % (high - low + 1);
        return arr[index];
    }

    int mid = low + (high - low) / 2;

    return median_of_three(
        arr[low],
        arr[mid],
        arr[high]
    );
}

/* 3-way partition을 이용한 퀵 정렬 */
void quick_sort(int arr[], int low, int high, int method)
{
    if (low >= high)
        return;

    enter_depth();

    int pivot = choose_pivot(arr, low, high, method);

    int lt = low;
    int i = low;
    int gt = high;

    while (i <= gt) {

        if (less_than(arr[i], pivot)) {
            int temp = arr[lt];
            arr[lt] = arr[i];
            arr[i] = temp;

            lt++;
            i++;
        }
        else if (greater_than(arr[i], pivot)) {
            int temp = arr[i];
            arr[i] = arr[gt];
            arr[gt] = temp;

            gt--;
        }
        else {
            i++;
        }
    }

    quick_sort(arr, low, lt - 1, method);
    quick_sort(arr, gt + 1, high, method);

    leave_depth();
}

Result run_quick(int arr[], int n, int method)
{
    Result r;

    comparisons = 0;
    current_depth = 0;
    max_depth = 0;

    clock_t start = clock();

    quick_sort(arr, 0, n - 1, method);

    clock_t end = clock();

    r.comparisons = comparisons;
    r.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    /* 재귀 호출 스택 추정 */
    r.memory = sizeof(QuickFrame) * (max_depth + 1);

    return r;
}

/* =========================================
   Main
   ========================================= */

int main(void)
{
    int sizes[] = {
        1000, 2000, 4000, 8000, 16000, 20000
    };

    int size_count = sizeof(sizes) / sizeof(sizes[0]);

    srand(42);

    printf("===============================================\n");
    printf(" Heap Sort / Merge Sort / Quick Sort Comparison\n");
    printf("===============================================\n");

    for (int s = 0; s < size_count; s++) {

        int n = sizes[s];

        int* arr = (int*)malloc(sizeof(int) * n);

        if (arr == NULL) {
            printf("Memory allocation failed.\n");
            return 1;
        }

        generate_data(n);

        printf("\nData Size : %d\n", n);

        printf("%-18s %-14s %-12s %-14s\n",
            "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");

        printf("--------------------------------------------------------------\n");

        /* Heap Sort */
        memcpy(arr, original, sizeof(int) * n);

        Result heap = run_heap(arr, n);

        if (!is_sorted(arr, n)) {
            printf("Heap Sort Error!\n");
            free(arr);
            return 1;
        }

        print_result("Heap Sort", heap);

        /* Merge Sort */
        memcpy(arr, original, sizeof(int) * n);

        Result merge = run_merge(arr, n);

        if (!is_sorted(arr, n)) {
            printf("Merge Sort Error!\n");
            free(arr);
            return 1;
        }

        print_result("Merge Sort", merge);

        /* Quick Sort - First */
        memcpy(arr, original, sizeof(int) * n);

        Result first = run_quick(arr, n, FIRST);

        if (!is_sorted(arr, n)) {
            printf("Quick First Error!\n");
            free(arr);
            return 1;
        }

        print_result("Quick (First)", first);

        /* Quick Sort - Random */
        memcpy(arr, original, sizeof(int) * n);

        Result random = run_quick(arr, n, RANDOM);

        if (!is_sorted(arr, n)) {
            printf("Quick Random Error!\n");
            free(arr);
            return 1;
        }

        print_result("Quick (Random)", random);

        /* Quick Sort - Median of Three */
        memcpy(arr, original, sizeof(int) * n);

        Result median = run_quick(arr, n, MEDIAN3);

        if (!is_sorted(arr, n)) {
            printf("Quick Median3 Error!\n");
            free(arr);
            return 1;
        }

        print_result("Quick (Median3)", median);

        printf("--------------------------------------------------------------\n");

        free(arr);
    }

    printf("\nAll sorting algorithms completed successfully.\n");

    return 0;
}
