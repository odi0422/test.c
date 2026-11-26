
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 50000
#define MAX_NAME 64
#define MAX_LINE 256
#define MAX_SEARCH 10000

typedef struct {
    int id;
    char name[MAX_NAME];
    char gender[8];
    int korean;
    int english;
    int math;
    int total;
} Student;

Student original[MAX_STUDENTS];
Student sorted[MAX_STUDENTS];
char searchNames[MAX_SEARCH][MAX_NAME];

int studentCount = 0;
long long quickComparisons = 0;
long long sequentialComparisons = 0;
long long binaryComparisons = 0;

int loadCSV(const char* filename) {
    FILE* fp = fopen(filename, "r");
    char line[MAX_LINE];

    if (fp == NULL) {
        printf("CSV 파일을 열 수 없습니다: %s\n", filename);
        return 0;
    }

    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        Student s;

        if (studentCount >= MAX_STUDENTS)
            break;

        if (sscanf(line, "%d,%63[^,],%7[^,],%d,%d,%d",
            &s.id, s.name, s.gender,
            &s.korean, &s.english, &s.math) != 6) {
            continue;
        }

        s.total = s.korean + s.english + s.math;
        original[studentCount++] = s;
    }

    fclose(fp);
    return studentCount;
}

int compareForSort(const char* a, const char* b) {
    quickComparisons++;
    return strcmp(a, b);
}

void swapStudent(Student* a, Student* b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

void quickSort(Student arr[], int left, int right) {
    int i = left;
    int j = right;
    char pivot[MAX_NAME];

    if (left >= right)
        return;

    strcpy(pivot, arr[(left + right) / 2].name);

    while (i <= j) {
        while (compareForSort(arr[i].name, pivot) < 0)
            i++;

        while (compareForSort(arr[j].name, pivot) > 0)
            j--;

        if (i <= j) {
            swapStudent(&arr[i], &arr[j]);
            i++;
            j--;
        }
    }

    if (left < j)
        quickSort(arr, left, j);

    if (i < right)
        quickSort(arr, i, right);
}

void printStudent(const Student* s) {
    printf("학번   : %d\n", s->id);
    printf("이름   : %s\n", s->name);
    printf("성별   : %s\n", s->gender);
    printf("국어   : %d\n", s->korean);
    printf("영어   : %d\n", s->english);
    printf("수학   : %d\n", s->math);
    printf("총점   : %d\n", s->total);
    printf("----------------------------------\n");
}

int sequentialSearch(const char* target, int printResult) {
    int found = 0;

    for (int i = 0; i < studentCount; i++) {
        sequentialComparisons++;

        if (strcmp(original[i].name, target) == 0) {
            found++;

            if (printResult)
                printStudent(&original[i]);
        }
    }

    return found;
}

int binaryLowerBound(const char* target) {
    int left = 0;
    int right = studentCount;

    while (left < right) {
        int mid = left + (right - left) / 2;
        binaryComparisons++;

        if (strcmp(sorted[mid].name, target) < 0)
            left = mid + 1;
        else
            right = mid;
    }

    return left;
}

int binarySearch(const char* target, int printResult) {
    int found = 0;
    int pos = binaryLowerBound(target);

    while (pos < studentCount) {
        binaryComparisons++;

        if (strcmp(sorted[pos].name, target) != 0)
            break;

        found++;

        if (printResult)
            printStudent(&sorted[pos]);

        pos++;
    }

    return found;
}

void makeSearchNames(void) {
    unsigned int seed = 123456789u;

    for (int i = 0; i < MAX_SEARCH; i++) {
        seed = seed * 1664525u + 1013904223u;

        if (i % 10 == 9) {
            sprintf(searchNames[i], "NOT_FOUND_%d", i);
        }
        else {
            int index = (int)(seed % (unsigned int)studentCount);
            strcpy(searchNames[i], original[index].name);
        }
    }
}

void runExperiment(void) {
    const int testK[] = {
        10, 50, 100, 200, 500,
        1000, 2000, 5000, 10000
    };

    const int testCount =
        sizeof(testK) / sizeof(testK[0]);

    int nextSearch = 0;

    sequentialComparisons = 0;
    binaryComparisons = 0;

    printf("\n================ 비교 횟수 실험 ================\n");
    printf("%-10s %-20s %-20s\n",
        "K", "Sequential", "Quick + Binary");
    printf("-------------------------------------------------\n");

    for (int t = 0; t < testCount; t++) {
        int K = testK[t];

        while (nextSearch < K) {
            sequentialSearch(searchNames[nextSearch], 0);
            binarySearch(searchNames[nextSearch], 0);
            nextSearch++;
        }

        long long binaryTotal =
            quickComparisons + binaryComparisons;

        printf("%-10d %-20lld %-20lld",
            K, sequentialComparisons, binaryTotal);

        if (sequentialComparisons < binaryTotal)
            printf(" Sequential 우세");
        else if (sequentialComparisons > binaryTotal)
            printf(" Quick + Binary 우세");
        else
            printf(" 동일");

        printf("\n");
    }
}

void searchAndPrint(const char* target) {
    int sequentialFound;
    int binaryFound;

    printf("\n========== 검색 결과: %s ==========\n", target);

    sequentialComparisons = 0;
    sequentialFound = sequentialSearch(target, 1);

    printf("순차 탐색 결과: %d명 / 비교 횟수: %lld회\n",
        sequentialFound, sequentialComparisons);

    binaryComparisons = 0;
    binaryFound = binarySearch(target, 1);

    printf("이진 탐색 결과: %d명 / 비교 횟수: %lld회\n",
        binaryFound, binaryComparisons);

    if (sequentialFound == binaryFound)
        printf("검색 결과 검증: 일치\n");
    else
        printf("검색 결과 검증: 불일치\n");
}

int main(void) {
    char filename[256] =
        "dataset_id_ascending (1).csv";

    char target[MAX_NAME];

    studentCount = loadCSV(filename);

    if (studentCount == 0) {
        printf("학생 데이터를 불러오지 못했습니다.\n");
        printf("CSV 파일의 위치를 확인하세요.\n");
        return 1;
    }

    printf("CSV 데이터 로드 완료: %d명\n", studentCount);

    for (int i = 0; i < studentCount; i++)
        sorted[i] = original[i];

    quickComparisons = 0;
    quickSort(sorted, 0, studentCount - 1);

    printf("퀵 정렬 완료 / 이름 비교 횟수: %lld회\n",
        quickComparisons);

    printf("\n검색할 학생 이름을 입력하세요: ");

    if (scanf("%63s", target) == 1)
        searchAndPrint(target);

    makeSearchNames();
    runExperiment();

    return 0;
}
