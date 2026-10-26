#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATA_COUNT 10000
#define TRIALS 100
#define KEY_MAX 1000000

typedef struct {
    int key1;
    int key2;
    int originalIndex;
} Data;

typedef struct {
    unsigned long long comparisons;
    unsigned long long movements;
} Stats;

typedef void (*SortFunction)(Data[], int, Stats *);

static void swapData(Data *a, Data *b, Stats *s) {
    Data temp = *a;
    *a = *b;
    *b = temp;
    s->movements += 3; /* three whole-record assignments */
}

static void selectionSort(Data a[], int n, Stats *s) {
    int i, j, minIndex;
    for (i = 0; i < n - 1; i++) {
        minIndex = i;
        for (j = i + 1; j < n; j++) {
            s->comparisons++;
            if (a[j].key1 < a[minIndex].key1)
                minIndex = j;
        }
        if (minIndex != i)
            swapData(&a[i], &a[minIndex], s);
    }
}

static void bubbleSort(Data a[], int n, Stats *s) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            s->comparisons++;
            if (a[j].key1 > a[j + 1].key1)
                swapData(&a[j], &a[j + 1], s);
        }
    }
}

static void bubbleSortEarly(Data a[], int n, Stats *s) {
    int i, j, swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - 1 - i; j++) {
            s->comparisons++;
            if (a[j].key1 > a[j + 1].key1) {
                swapData(&a[j], &a[j + 1], s);
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }
}

/* useGreaterEqual == 0: key1 > current.key1 (stable)
   useGreaterEqual == 1: key1 >= current.key1 (not stable) */
static void insertionSort(Data a[], int n, Stats *s, int useGreaterEqual) {
    int i, j;
    Data current;

    for (i = 1; i < n; i++) {
        current = a[i];
        s->movements++; /* save current record in temporary variable */
        j = i - 1;

        while (j >= 0) {
            s->comparisons++;
            if (useGreaterEqual ? (a[j].key1 >= current.key1)
                                : (a[j].key1 > current.key1)) {
                a[j + 1] = a[j];
                s->movements++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = current;
        s->movements++;
    }
}

static int isSorted(const Data a[], int n) {
    int i;
    for (i = 1; i < n; i++) {
        if (a[i - 1].key1 > a[i].key1)
            return 0;
    }
    return 1;
}

static int isStable(const Data a[], int n) {
    int i;
    for (i = 1; i < n; i++) {
        if (a[i - 1].key1 == a[i].key1 &&
            a[i - 1].originalIndex > a[i].originalIndex)
            return 0;
    }
    return 1;
}

static void generateRandomData(Data a[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        a[i].key1 = rand() % (KEY_MAX + 1);
        a[i].key2 = rand() % (KEY_MAX + 1);
        a[i].originalIndex = i;
    }
}

static int compareDataKey1(const void *pa, const void *pb) {
    const Data *a = (const Data *)pa;
    const Data *b = (const Data *)pb;
    if (a->key1 < b->key1) return -1;
    if (a->key1 > b->key1) return 1;
    return (a->originalIndex > b->originalIndex) -
           (a->originalIndex < b->originalIndex);
}

/* qsort is only used to prepare test inputs, not as a measured algorithm. */
static void prepareSorted(Data out[], const Data base[], int n) {
    memcpy(out, base, sizeof(Data) * n);
    qsort(out, n, sizeof(Data), compareDataKey1);
}

static void prepareReverse(Data out[], const Data base[], int n) {
    int i;
    prepareSorted(out, base, n);
    for (i = 0; i < n / 2; i++) {
        Data temp = out[i];
        out[i] = out[n - 1 - i];
        out[n - 1 - i] = temp;
    }
}

/* Partially sorted input: sort first, then perform random swaps on 5% of n. */
static void preparePartial(Data out[], const Data base[], int n) {
    int i, swaps = n / 20;
    prepareSorted(out, base, n);
    for (i = 0; i < swaps; i++) {
        int x = rand() % n;
        int y = rand() % n;
        Data temp = out[x];
        out[x] = out[y];
        out[y] = temp;
    }
}

static void runAndReport(const char *name, SortFunction sort,
                         const Data input[], int n) {
    Data *work = (Data *)malloc(sizeof(Data) * n);
    Stats s = {0, 0};

    if (!work) {
        printf("Memory allocation failed.\n");
        return;
    }

    memcpy(work, input, sizeof(Data) * n);
    sort(work, n, &s);

    printf("%-32s Comparisons: %llu  Movements: %llu  Sorted: %s\n",
           name, s.comparisons, s.movements,
           isSorted(work, n) ? "YES" : "NO");
    free(work);
}

static void runInsertionReport(const char *name, const Data input[],
                               int n, int useGreaterEqual) {
    Data *work = (Data *)malloc(sizeof(Data) * n);
    Stats s = {0, 0};

    if (!work) {
        printf("Memory allocation failed.\n");
        return;
    }

    memcpy(work, input, sizeof(Data) * n);
    insertionSort(work, n, &s, useGreaterEqual);
    printf("%-32s Comparisons: %llu  Movements: %llu  Sorted: %s\n",
           name, s.comparisons, s.movements,
           isSorted(work, n) ? "YES" : "NO");
    free(work);
}

static void runRandomTrials(void) {
    Data *base = (Data *)malloc(sizeof(Data) * DATA_COUNT);
    Data *work = (Data *)malloc(sizeof(Data) * DATA_COUNT);
    unsigned long long totalComp[4] = {0, 0, 0, 0};
    unsigned long long totalMove[4] = {0, 0, 0, 0};
    int trial, alg;
    const char *names[4] = {
        "Selection Sort",
        "Bubble Sort",
        "Bubble Sort (Early)",
        "Insertion Sort (>)"
    };

    if (!base || !work) {
        printf("Memory allocation failed.\n");
        free(base);
        free(work);
        return;
    }

    for (trial = 0; trial < TRIALS; trial++) {
        Stats s;
        generateRandomData(base, DATA_COUNT);

        /* All algorithms receive copies of the same trial's data. */
        for (alg = 0; alg < 4; alg++) {
            memcpy(work, base, sizeof(Data) * DATA_COUNT);
            s.comparisons = 0;
            s.movements = 0;

            if (alg == 0) selectionSort(work, DATA_COUNT, &s);
            else if (alg == 1) bubbleSort(work, DATA_COUNT, &s);
            else if (alg == 2) bubbleSortEarly(work, DATA_COUNT, &s);
            else insertionSort(work, DATA_COUNT, &s, 0);

            totalComp[alg] += s.comparisons;
            totalMove[alg] += s.movements;

            if (!isSorted(work, DATA_COUNT)) {
                printf("ERROR: %s produced an unsorted result in trial %d.\n",
                       names[alg], trial + 1);
            }
        }
    }

    printf("\n========================================\n");
    printf("Random Data: %d items, %d trials\n", DATA_COUNT, TRIALS);
    printf("========================================\n");
    for (alg = 0; alg < 4; alg++) {
        printf("%s\n", names[alg]);
        printf("  Total Comparisons  : %llu\n", totalComp[alg]);
        printf("  Average Comparisons: %.2f\n",
               (double)totalComp[alg] / TRIALS);
        printf("  Total Movements    : %llu\n", totalMove[alg]);
        printf("  Average Movements  : %.2f\n",
               (double)totalMove[alg] / TRIALS);
    }

    free(base);
    free(work);
}

static void runInputStateTests(void) {
    Data *base = (Data *)malloc(sizeof(Data) * DATA_COUNT);
    Data *test = (Data *)malloc(sizeof(Data) * DATA_COUNT);

    if (!base || !test) {
        printf("Memory allocation failed.\n");
        free(base);
        free(test);
        return;
    }

    generateRandomData(base, DATA_COUNT);

    printf("\n========================================\n");
    printf("Input-State Performance Tests\n");
    printf("========================================\n");

    prepareSorted(test, base, DATA_COUNT);
    printf("\n[Sorted Data]\n");
    runAndReport("Selection Sort", selectionSort, test, DATA_COUNT);
    runAndReport("Bubble Sort", bubbleSort, test, DATA_COUNT);
    runAndReport("Bubble Sort (Early)", bubbleSortEarly, test, DATA_COUNT);
    runInsertionReport("Insertion Sort (>)", test, DATA_COUNT, 0);

    prepareReverse(test, base, DATA_COUNT);
    printf("\n[Reverse-Sorted Data]\n");
    runAndReport("Selection Sort", selectionSort, test, DATA_COUNT);
    runAndReport("Bubble Sort", bubbleSort, test, DATA_COUNT);
    runAndReport("Bubble Sort (Early)", bubbleSortEarly, test, DATA_COUNT);
    runInsertionReport("Insertion Sort (>)", test, DATA_COUNT, 0);

    preparePartial(test, base, DATA_COUNT);
    printf("\n[Partially Sorted Data: 5%% random swaps]\n");
    runAndReport("Selection Sort", selectionSort, test, DATA_COUNT);
    runAndReport("Bubble Sort", bubbleSort, test, DATA_COUNT);
    runAndReport("Bubble Sort (Early)", bubbleSortEarly, test, DATA_COUNT);
    runInsertionReport("Insertion Sort (>)", test, DATA_COUNT, 0);

    free(base);
    free(test);
}

static void runStabilityTests(void) {
    Data sample[] = {
        {5, 100, 0},
        {3, 200, 1},
        {5, 300, 2},
        {2, 400, 3},
        {3, 500, 4},
        {5, 600, 5},
        {2, 700, 6},
        {3, 800, 7}
    };
    const int n = (int)(sizeof(sample) / sizeof(sample[0]));
    Data work[sizeof(sample) / sizeof(sample[0])];
    Stats s;

    printf("\n========================================\n");
    printf("Stability Verification\n");
    printf("========================================\n");
    printf("Test data contains duplicate Key 1 values (2, 3, and 5).\n");

    memcpy(work, sample, sizeof(sample));
    s.comparisons = s.movements = 0;
    selectionSort(work, n, &s);
    printf("Selection Sort       : %s\n",
           isStable(work, n) ? "STABLE" : "NOT STABLE");

    memcpy(work, sample, sizeof(sample));
    s.comparisons = s.movements = 0;
    bubbleSortEarly(work, n, &s);
    printf("Bubble Sort           : %s\n",
           isStable(work, n) ? "STABLE" : "NOT STABLE");

    memcpy(work, sample, sizeof(sample));
    s.comparisons = s.movements = 0;
    insertionSort(work, n, &s, 0);
    printf("Insertion Sort (>)    : %s\n",
           isStable(work, n) ? "STABLE" : "NOT STABLE");

    memcpy(work, sample, sizeof(sample));
    s.comparisons = s.movements = 0;
    insertionSort(work, n, &s, 1);
    printf("Insertion Sort (>=)   : %s\n",
           isStable(work, n) ? "STABLE" : "NOT STABLE");

    printf("\nStable insertion-sort output (key1, key2, originalIndex):\n");
    memcpy(work, sample, sizeof(sample));
    s.comparisons = s.movements = 0;
    insertionSort(work, n, &s, 0);
    for (int i = 0; i < n; i++) {
        printf("(%d, %d, %d)%s", work[i].key1, work[i].key2,
               work[i].originalIndex, (i == n - 1) ? "\n" : " ");
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    printf("Data Count: %d\n", DATA_COUNT);
    printf("Key range: 0 to %d\n", KEY_MAX);

    runRandomTrials();
    runInputStateTests();
    runStabilityTests();

    printf("\nAll tests completed.\n");
    return 0;
}
