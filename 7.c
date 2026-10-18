
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VERTICES 100
#define SPARSE_EDGES 100
#define DENSE_EDGES 4000

/* 간선 정보 */
typedef struct {
    int u;
    int v;
} Edge;

/* 인접 리스트 노드 */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

/* 인접 행렬 그래프 */
typedef struct {
    int vertexCount;
    unsigned char matrix[VERTICES][VERTICES];
} MatrixGraph;

/* 인접 리스트 그래프 */
typedef struct {
    int vertexCount;
    AdjNode *head[VERTICES];
} ListGraph;

/* 연산별 비교 횟수 */
typedef struct {
    long long insert;
    long long delete;
    long long connectivity;
    long long adjacent;
} Statistics;

/* 인접 리스트 노드 생성 */
AdjNode *createNode(int vertex)
{
    AdjNode *node = (AdjNode *)malloc(sizeof(AdjNode));

    if (node == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    node->vertex = vertex;
    node->next = NULL;

    return node;
}

/* 무방향 그래프의 간선 목록 생성 */
void generateGraph(Edge edges[], int edgeCount)
{
    unsigned char used[VERTICES][VERTICES] = {0};
    int count = 0;

    while (count < edgeCount) {
        int u = rand() % VERTICES;
        int v = rand() % VERTICES;

        /* 자기 자신과 연결하거나 중복 간선을 만들지 않음 */
        if (u == v)
            continue;

        if (used[u][v])
            continue;

        used[u][v] = 1;
        used[v][u] = 1;

        edges[count].u = u;
        edges[count].v = v;
        count++;
    }
}

/* 인접 행렬 초기화 */
void initMatrix(MatrixGraph *graph)
{
    graph->vertexCount = VERTICES;

    for (int i = 0; i < VERTICES; i++) {
        for (int j = 0; j < VERTICES; j++) {
            graph->matrix[i][j] = 0;
        }
    }
}

/* 인접 리스트 초기화 */
void initList(ListGraph *graph)
{
    graph->vertexCount = VERTICES;

    for (int i = 0; i < VERTICES; i++) {
        graph->head[i] = NULL;
    }
}

/* 인접 행렬에 간선 삽입 */
long long matrixInsert(MatrixGraph *graph, int u, int v)
{
    long long comparisons = 1;

    if (graph->matrix[u][v] == 0) {
        graph->matrix[u][v] = 1;
        graph->matrix[v][u] = 1;
    }

    return comparisons;
}

/* 인접 리스트에서 특정 정점 탐색 */
int listContains(ListGraph *graph, int u, int v,
                 long long *comparisons)
{
    AdjNode *current = graph->head[u];

    while (current != NULL) {
        (*comparisons)++;

        if (current->vertex == v)
            return 1;

        current = current->next;
    }

    return 0;
}

/* 인접 리스트에 간선 삽입 */
long long listInsert(ListGraph *graph, int u, int v)
{
    long long comparisons = 0;

    /* 이미 연결되어 있는지 확인 */
    if (listContains(graph, u, v, &comparisons))
        return comparisons;

    /* 무방향 그래프이므로 양쪽에 노드 삽입 */
    AdjNode *node1 = createNode(v);
    node1->next = graph->head[u];
    graph->head[u] = node1;

    AdjNode *node2 = createNode(u);
    node2->next = graph->head[v];
    graph->head[v] = node2;

    return comparisons;
}

/* 인접 행렬에서 간선 삭제 */
long long matrixDelete(MatrixGraph *graph, int u, int v)
{
    long long comparisons = 1;

    if (graph->matrix[u][v] == 1) {
        graph->matrix[u][v] = 0;
        graph->matrix[v][u] = 0;
    }

    return comparisons;
}

/* 인접 리스트에서 간선 삭제
   양쪽 인접 리스트의 탐색 횟수를 모두 계산 */
long long listDelete(ListGraph *graph, int u, int v)
{
    long long comparisons = 0;
    AdjNode **current;
    AdjNode *temp;

    /* u의 리스트에서 v 삭제 */
    current = &graph->head[u];

    while (*current != NULL) {
        comparisons++;

        if ((*current)->vertex == v) {
            temp = *current;
            *current = temp->next;
            free(temp);
            break;
        }

        current = &(*current)->next;
    }

    /* v의 리스트에서 u 삭제 */
    current = &graph->head[v];

    while (*current != NULL) {
        comparisons++;

        if ((*current)->vertex == u) {
            temp = *current;
            *current = temp->next;
            free(temp);
            break;
        }

        current = &(*current)->next;
    }

    return comparisons;
}

/* 인접 행렬의 연결 여부 확인 */
int matrixConnected(MatrixGraph *graph, int u, int v,
                    long long *comparisons)
{
    (*comparisons)++;
    return graph->matrix[u][v] == 1;
}

/* 인접 리스트의 연결 여부 확인 */
int listConnected(ListGraph *graph, int u, int v,
                  long long *comparisons)
{
    return listContains(graph, u, v, comparisons);
}

/* 인접 행렬의 인접 정점 출력 */
void matrixAdjacent(MatrixGraph *graph, int u,
                    long long *comparisons)
{
    printf("Adjacent vertices of %d: ", u);

    for (int v = 0; v < VERTICES; v++) {
        (*comparisons)++;

        if (graph->matrix[u][v] == 1)
            printf("%d ", v);
    }

    printf("\n");
}

/* 인접 리스트의 인접 정점 출력 */
void listAdjacent(ListGraph *graph, int u,
                  long long *comparisons)
{
    AdjNode *current = graph->head[u];

    printf("Adjacent vertices of %d: ", u);

    while (current != NULL) {
        (*comparisons)++;
        printf("%d ", current->vertex);
        current = current->next;
    }

    printf("\n");
}

/* 인접 행렬에 간선 목록 반영 */
void buildMatrix(MatrixGraph *graph, Edge edges[], int edgeCount)
{
    initMatrix(graph);

    for (int i = 0; i < edgeCount; i++) {
        int u = edges[i].u;
        int v = edges[i].v;

        graph->matrix[u][v] = 1;
        graph->matrix[v][u] = 1;
    }
}

/* 인접 리스트에 간선 목록 반영 */
void buildList(ListGraph *graph, Edge edges[], int edgeCount)
{
    initList(graph);

    for (int i = 0; i < edgeCount; i++) {
        int u = edges[i].u;
        int v = edges[i].v;

        AdjNode *node1 = createNode(v);
        node1->next = graph->head[u];
        graph->head[u] = node1;

        AdjNode *node2 = createNode(u);
        node2->next = graph->head[v];
        graph->head[v] = node2;
    }
}

/* 인접 행렬 메모리 사용량 */
size_t matrixMemory(MatrixGraph *graph)
{
    (void)graph;
    return sizeof(MatrixGraph);
}

/* 인접 리스트 메모리 사용량 */
size_t listMemory(ListGraph *graph)
{
    size_t memory = sizeof(ListGraph);

    for (int i = 0; i < VERTICES; i++) {
        AdjNode *current = graph->head[i];

        while (current != NULL) {
            memory += sizeof(AdjNode);
            current = current->next;
        }
    }

    return memory;
}

/* 인접 리스트 메모리 해제 */
void freeList(ListGraph *graph)
{
    for (int i = 0; i < VERTICES; i++) {
        AdjNode *current = graph->head[i];

        while (current != NULL) {
            AdjNode *temp = current;
            current = current->next;
            free(temp);
        }

        graph->head[i] = NULL;
    }
}

/* 기존 그래프에 없는 간선 선택 */
void findMissingEdge(Edge edges[], int edgeCount, int *u, int *v)
{
    unsigned char used[VERTICES][VERTICES] = {0};

    for (int i = 0; i < edgeCount; i++) {
        used[edges[i].u][edges[i].v] = 1;
        used[edges[i].v][edges[i].u] = 1;
    }

    for (int i = 0; i < VERTICES; i++) {
        for (int j = i + 1; j < VERTICES; j++) {
            if (!used[i][j]) {
                *u = i;
                *v = j;
                return;
            }
        }
    }
}

/* 측정에 사용할 공통 연산 수행 */
void measureMatrix(MatrixGraph *graph, Edge edges[],
                   int edgeCount, int insertU, int insertV,
                   int deleteU, int deleteV,
                   int queryU, int queryV, int adjacentVertex,
                   Statistics *stats)
{
    stats->insert = matrixInsert(graph, insertU, insertV);
    stats->delete = matrixDelete(graph, deleteU, deleteV);

    stats->connectivity = 0;
    int connected = matrixConnected(
        graph, queryU, queryV, &stats->connectivity);

    stats->adjacent = 0;

    printf("Connectivity (%d, %d): %s\n",
           queryU, queryV, connected ? "Connected" : "Not connected");

    matrixAdjacent(graph, adjacentVertex, &stats->adjacent);

    (void)edges;
    (void)edgeCount;
}

/* 결과 출력 */
void printResult(const char *title, size_t memory,
                 Statistics stats)
{
    printf("\n%s\n", title);
    printf("Memory                         : %zu Bytes\n", memory);
    printf("Edge insertion comparisons     : %lld\n", stats.insert);
    printf("Edge deletion comparisons      : %lld\n", stats.delete);
    printf("Connectivity check comparisons : %lld\n", stats.connectivity);
    printf("Adjacent vertices comparisons  : %lld\n", stats.adjacent);
}

int main(void)
{
    Edge sparseEdges[SPARSE_EDGES];
    Edge denseEdges[DENSE_EDGES];

    MatrixGraph sparseMatrix, denseMatrix;
    ListGraph sparseList, denseList;

    Statistics sparseMatrixStats = {0};
    Statistics sparseListStats = {0};
    Statistics denseMatrixStats = {0};
    Statistics denseListStats = {0};

    int sparseInsertU, sparseInsertV;
    int denseInsertU, denseInsertV;

    int sparseDeleteU, sparseDeleteV;
    int denseDeleteU, denseDeleteV;

    int queryU, queryV, adjacentVertex;

    srand((unsigned int)time(NULL));

    /* 두 그래프를 각각 한 번만 생성 */
    generateGraph(sparseEdges, SPARSE_EDGES);
    generateGraph(denseEdges, DENSE_EDGES);

    /* 두 표현 방식에 동일한 그래프 구성 */
    buildMatrix(&sparseMatrix, sparseEdges, SPARSE_EDGES);
    buildList(&sparseList, sparseEdges, SPARSE_EDGES);

    buildMatrix(&denseMatrix, denseEdges, DENSE_EDGES);
    buildList(&denseList, denseEdges, DENSE_EDGES);

    /* 삽입할 간선은 기존 그래프에 없는 간선 */
    findMissingEdge(sparseEdges, SPARSE_EDGES,
                    &sparseInsertU, &sparseInsertV);

    findMissingEdge(denseEdges, DENSE_EDGES,
                    &denseInsertU, &denseInsertV);

    /* 삭제할 간선은 기존 그래프에 존재하는 간선 */
    sparseDeleteU = sparseEdges[0].u;
    sparseDeleteV = sparseEdges[0].v;

    denseDeleteU = denseEdges[0].u;
    denseDeleteV = denseEdges[0].v;

    /* 모든 표현 방식에서 동일한 조회 조건 사용 */
    queryU = rand() % VERTICES;
    queryV = rand() % VERTICES;

    while (queryU == queryV)
        queryV = rand() % VERTICES;

    adjacentVertex = rand() % VERTICES;

    printf("Vertices: %d\n", VERTICES);
    printf("Sparse edges: %d\n", SPARSE_EDGES);
    printf("Dense edges: %d\n", DENSE_EDGES);

    printf("\nInsertion edge (Sparse): (%d, %d)\n",
           sparseInsertU, sparseInsertV);
    printf("Deletion edge (Sparse): (%d, %d)\n",
           sparseDeleteU, sparseDeleteV);

    printf("\nInsertion edge (Dense): (%d, %d)\n",
           denseInsertU, denseInsertV);
    printf("Deletion edge (Dense): (%d, %d)\n",
           denseDeleteU, denseDeleteV);

    printf("\nConnectivity query: (%d, %d)\n", queryU, queryV);
    printf("Adjacent vertex query: %d\n", adjacentVertex);

    /* Case 1: 희소 그래프 - 인접 행렬 */
    measureMatrix(&sparseMatrix, sparseEdges, SPARSE_EDGES,
                  sparseInsertU, sparseInsertV,
                  sparseDeleteU, sparseDeleteV,
                  queryU, queryV, adjacentVertex,
                  &sparseMatrixStats);

    printResult("Case 1 : Sparse Graph - Adjacency Matrix",
                matrixMemory(&sparseMatrix), sparseMatrixStats);

    /* Case 2: 희소 그래프 - 인접 리스트 */
    sparseListStats.insert =
        listInsert(&sparseList, sparseInsertU, sparseInsertV);

    sparseListStats.delete =
        listDelete(&sparseList, sparseDeleteU, sparseDeleteV);

    sparseListStats.connectivity = 0;

    int sparseConnected = listConnected(
        &sparseList, queryU, queryV,
        &sparseListStats.connectivity);

    sparseListStats.adjacent = 0;

    printf("\nConnectivity (%d, %d): %s\n",
           queryU, queryV,
           sparseConnected ? "Connected" : "Not connected");

    listAdjacent(&sparseList, adjacentVertex,
                 &sparseListStats.adjacent);

    printResult("Case 2 : Sparse Graph - Adjacency List",
                listMemory(&sparseList), sparseListStats);

    /* Case 3: 밀집 그래프 - 인접 행렬 */
    measureMatrix(&denseMatrix, denseEdges, DENSE_EDGES,
                  denseInsertU, denseInsertV,
                  denseDeleteU, denseDeleteV,
                  queryU, queryV, adjacentVertex,
                  &denseMatrixStats);

    printResult("Case 3 : Dense Graph - Adjacency Matrix",
                matrixMemory(&denseMatrix), denseMatrixStats);

    /* Case 4: 밀집 그래프 - 인접 리스트 */
    denseListStats.insert =
        listInsert(&denseList, denseInsertU, denseInsertV);

    denseListStats.delete =
        listDelete(&denseList, denseDeleteU, denseDeleteV);

    denseListStats.connectivity = 0;

    int denseConnected = listConnected(
        &denseList, queryU, queryV,
        &denseListStats.connectivity);

    denseListStats.adjacent = 0;

    printf("\nConnectivity (%d, %d): %s\n",
           queryU, queryV,
           denseConnected ? "Connected" : "Not connected");

    listAdjacent(&denseList, adjacentVertex,
                 &denseListStats.adjacent);

    printResult("Case 4 : Dense Graph - Adjacency List",
                listMemory(&denseList), denseListStats);

    /* 동적 메모리 해제 */
    freeList(&sparseList);
    freeList(&denseList);

    return 0;
}
