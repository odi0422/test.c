
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_COUNT 6

// 삽입 정렬
long long insertionSort(int arr[], int n) {
    long long comparisons = 0;

    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;

        while (j >= 0) {
            comparisons++;  // 데이터 값 비교

            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            else {
                break;
            }
        }

        arr[j + 1] = key;
    }

    return comparisons;
}

// 쉘 정렬 - Original 간격 수열
long long shellSortOriginal(int arr[], int n) {
    long long comparisons = 0;

    for (int gap = n / 2; gap >= 1; gap /= 2) {

        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;

            while (j >= gap) {
                comparisons++;  // 데이터 값 비교

                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    j -= gap;
                }
                else {
                    break;
                }
            }

            arr[j] = temp;
        }
    }

    return comparisons;
}

// 쉘 정렬 - Knuth 간격 수열
long long shellSortKnuth(int arr[], int n) {
    long long comparisons = 0;
    int gap = 1;

    // 데이터 크기보다 작은 가장 큰 Knuth 간격 구하기
    while (gap < n / 3) {
        gap = 3 * gap + 1;
    }

    // 큰 간격부터 1까지 역순으로 적용
    for (; gap >= 1; gap /= 3) {

        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;

            while (j >= gap) {
                comparisons++;  // 데이터 값 비교

                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    j -= gap;
                }
                else {
                    break;
                }
            }

            arr[j] = temp;
        }
    }

    return comparisons;
}

// 오름차순 정렬 확인
int isSorted(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return 0;
        }
    }

    return 1;
}

int main() {
    int sizes[SIZE_COUNT] = {
        1000, 2000, 4000, 8000, 16000, 20000
    };

    srand((unsigned int)time(NULL));

    for (int s = 0; s < SIZE_COUNT; s++) {
        int n = sizes[s];

        // 원본 데이터 및 정렬용 배열 생성
        int* original = (int*)malloc(sizeof(int) * n);
        int* arr1 = (int*)malloc(sizeof(int) * n);
        int* arr2 = (int*)malloc(sizeof(int) * n);
        int* arr3 = (int*)malloc(sizeof(int) * n);

        if (original == NULL || arr1 == NULL ||
            arr2 == NULL || arr3 == NULL) {
            printf("Memory allocation failed.\n");
            free(original);
            free(arr1);
            free(arr2);
            free(arr3);
            return 1;
        }

        // 무작위 데이터 생성
        for (int i = 0; i < n; i++) {
            original[i] = rand() % 100000;
        }

        // 동일한 원본 데이터 복사
        for (int i = 0; i < n; i++) {
            arr1[i] = original[i];
            arr2[i] = original[i];
            arr3[i] = original[i];
        }

        // 각 정렬 수행 및 비교 횟수 측정
        long long insertionCount = insertionSort(arr1, n);
        long long originalCount = shellSortOriginal(arr2, n);
        long long knuthCount = shellSortKnuth(arr3, n);

        // 결과 출력
        printf("Data Size : %d\n\n", n);

        printf("Insertion Sort       : %lld comparisons\n",
               insertionCount);

        printf("Shell Sort (Original): %lld comparisons\n",
               originalCount);

        printf("Shell Sort (Knuth)   : %lld comparisons\n\n",
               knuthCount);

        // 정렬 결과 확인
        if (isSorted(arr1, n) &&
            isSorted(arr2, n) &&
            isSorted(arr3, n)) {
            printf("Sorting Verification : PASS\n");
        }
        else {
            printf("Sorting Verification : FAIL\n");
        }

        printf("----------------------------------------\n");

        // 동적 메모리 해제
        free(original);
        free(arr1);
        free(arr2);
        free(arr3);
    }

    return 0;
}
