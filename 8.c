
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define V 50
#define SPARSE_EDGES 100
#define DENSE_EDGES 600
#define MAX_WEIGHT 1000
#define INF 1000000000

typedef struct {
    int src;
    int dest;
    int weight;
} Edge;

typedef struct AdjNode {
    int dest;
    int weight;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    AdjNode *head[V];
} AdjList;

typedef struct {
    int parent[V];
    int rank[V];
} UnionFind;

typedef struct {
    int src;
    int dest;
    int weight;
} MSTEdge;

typedef struct {
    int count;
    long long totalWeight;
    MSTEdge edges[V - 1];
} MSTResult;

/* Random integer in the range [min, max] */
int randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

/* Check whether an edge already exists */
int edgeExists(int matrix[V][V], int u, int v)
{
    return matrix[u][v] != 0;
}

/* Generate a connected undirected simple graph */
void generateGraph(Edge edges[], int *edgeCount, int targetEdges)
{
    int matrix[V][V] = {0};
    int u, v;
    int weight;

    *edgeCount = 0;

    /*
       First create a spanning tree.
       Each vertex from 1 to V-1 connects to
       one of the previously created vertices.
    */
    for (v = 1; v < V; v++) {
        u = randomInt(0, v - 1);
        weight = randomInt(1, MAX_WEIGHT);

        edges[*edgeCount].src = u;
        edges[*edgeCount].dest = v;
        edges[*edgeCount].weight = weight;
        (*edgeCount)++;

        matrix[u][v] = 1;
        matrix[v][u] = 1;
    }

    /*
       Add random edges without self-loops
       or duplicate edges.
    */
    while (*edgeCount < targetEdges) {
        u = randomInt(0, V - 1);
        v = randomInt(0, V - 1);

        if (u == v || edgeExists(matrix, u, v)) {
            continue;
        }

        weight = randomInt(1, MAX_WEIGHT);

        edges[*edgeCount].src = u;
        edges[*edgeCount].dest = v;
        edges[*edgeCount].weight = weight;
        (*edgeCount)++;

        matrix[u][v] = 1;
        matrix[v][u] = 1;
    }
}

/* Create an adjacency list */
void initAdjList(AdjList *graph)
{
    int i;

    for (i = 0; i < V; i++) {
        graph->head[i] = NULL;
    }
}

void addAdjNode(AdjList *graph, int src, int dest, int weight)
{
    AdjNode *node = (AdjNode *)malloc(sizeof(AdjNode));

    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    node->dest = dest;
    node->weight = weight;
    node->next = graph->head[src];
    graph->head[src] = node;
}

void buildAdjList(AdjList *graph, Edge edges[], int edgeCount)
{
    int i;

    initAdjList(graph);

    for (i = 0; i < edgeCount; i++) {
        addAdjNode(graph, edges[i].src,
                   edges[i].dest, edges[i].weight);

        addAdjNode(graph, edges[i].dest,
                   edges[i].src, edges[i].weight);
    }
}

void freeAdjList(AdjList *graph)
{
    int i;

    for (i = 0; i < V; i++) {
        AdjNode *current = graph->head[i];

        while (current != NULL) {
            AdjNode *next = current->next;
            free(current);
            current = next;
        }

        graph->head[i] = NULL;
    }
}

/* Sort edges by weight for Kruskal */
int compareEdges(const void *a, const void *b)
{
    const Edge *e1 = (const Edge *)a;
    const Edge *e2 = (const Edge *)b;

    if (e1->weight < e2->weight) return -1;
    if (e1->weight > e2->weight) return 1;
    return 0;
}

/* Union-Find */
void initUnionFind(UnionFind *uf)
{
    int i;

    for (i = 0; i < V; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
}

int findRoot(UnionFind *uf, int x)
{
    if (uf->parent[x] != x) {
        uf->parent[x] = findRoot(uf, uf->parent[x]);
    }

    return uf->parent[x];
}

int unionSets(UnionFind *uf, int x, int y)
{
    int rootX = findRoot(uf, x);
    int rootY = findRoot(uf, y);

    if (rootX == rootY) {
        return 0;
    }

    if (uf->rank[rootX] < uf->rank[rootY]) {
        uf->parent[rootX] = rootY;
    } else if (uf->rank[rootX] > uf->rank[rootY]) {
        uf->parent[rootY] = rootX;
    } else {
        uf->parent[rootY] = rootX;
        uf->rank[rootX]++;
    }

    return 1;
}

/* Kruskal's algorithm */
MSTResult kruskal(Edge edges[], int edgeCount)
{
    UnionFind uf;
    MSTResult result = {0, 0, {{0, 0, 0}}};
    int i;

    initUnionFind(&uf);

    qsort(edges, edgeCount, sizeof(Edge), compareEdges);

    for (i = 0; i < edgeCount && result.count < V - 1; i++) {
        if (unionSets(&uf, edges[i].src, edges[i].dest)) {
            result.edges[result.count].src = edges[i].src;
            result.edges[result.count].dest = edges[i].dest;
            result.edges[result.count].weight = edges[i].weight;

            result.totalWeight += edges[i].weight;
            result.count++;
        }
    }

    return result;
}

/* Create an adjacency matrix */
void buildAdjMatrix(int matrix[V][V], Edge edges[], int edgeCount)
{
    int i, j;

    for (i = 0; i < V; i++) {
        for (j = 0; j < V; j++) {
            matrix[i][j] = INF;
        }
        matrix[i][i] = 0;
    }

    for (i = 0; i < edgeCount; i++) {
        int u = edges[i].src;
        int v = edges[i].dest;
        int w = edges[i].weight;

        matrix[u][v] = w;
        matrix[v][u] = w;
    }
}

/* Prim's algorithm using an adjacency matrix */
MSTResult prim(int matrix[V][V])
{
    int key[V];
    int parent[V];
    int visited[V] = {0};
    MSTResult result = {0, 0, {{0, 0, 0}}};
    int i, j;

    for (i = 0; i < V; i++) {
        key[i] = INF;
        parent[i] = -1;
    }

    key[0] = 0;

    for (i = 0; i < V; i++) {
        int u = -1;
        int minKey = INF;

        /* Select the unvisited vertex with minimum key */
        for (j = 0; j < V; j++) {
            if (!visited[j] && key[j] < minKey) {
                minKey = key[j];
                u = j;
            }
        }

        if (u == -1) {
            break;
        }

        visited[u] = 1;

        if (parent[u] != -1) {
            result.edges[result.count].src = parent[u];
            result.edges[result.count].dest = u;
            result.edges[result.count].weight = key[u];

            result.totalWeight += key[u];
            result.count++;
        }

        /* Update the costs of adjacent vertices */
        for (j = 0; j < V; j++) {
            if (!visited[j] &&
                matrix[u][j] != INF &&
                matrix[u][j] > 0 &&
                matrix[u][j] < key[j]) {

                key[j] = matrix[u][j];
                parent[j] = u;
            }
        }
    }

    return result;
}

/* Calculate memory used by the graph and MST data structures */
size_t kruskalMemoryBytes(AdjList *graph, int edgeCount)
{
    size_t memory = 0;
    int i;

    /* Adjacency-list head pointers */
    memory += sizeof(graph->head);

    /* All adjacency-list nodes */
    for (i = 0; i < V; i++) {
        AdjNode *current = graph->head[i];

        while (current != NULL) {
            memory += sizeof(AdjNode);
            current = current->next;
        }
    }

    /* Original edge array and Union-Find arrays */
    memory += (size_t)edgeCount * sizeof(Edge);
    memory += sizeof(UnionFind);

    /* MST result storage */
    memory += sizeof(MSTResult);

    return memory;
}

size_t primMemoryBytes(void)
{
    size_t memory = 0;

    memory += sizeof(int) * V * V; /* Adjacency matrix */
    memory += sizeof(int) * V;     /* key[] */
    memory += sizeof(int) * V;     /* parent[] */
    memory += sizeof(int) * V;     /* visited[] */
    memory += sizeof(MSTResult);   /* MST result */

    return memory;
}

/* Print MST edges and performance results */
void printResult(
    int caseNo,
    const char *graphType,
    const char *representation,
    const char *algorithm,
    int edgeCount,
    MSTResult result,
    size_t memoryBytes,
    double executionMs)
{
    int i;

    printf("\n========================================\n");
    printf("Case %d : %s Graph - %s - %s\n",
           caseNo, graphType, representation, algorithm);
    printf("========================================\n");

    printf("Vertices       : %d\n", V);
    printf("Edges          : %d\n", edgeCount);

    printf("MST Edges:\n");

    for (i = 0; i < result.count; i++) {
        printf("  %2d -- %2d (Weight: %d)\n",
               result.edges[i].src,
               result.edges[i].dest,
               result.edges[i].weight);
    }

    printf("MST Edge Count : %d\n", result.count);
    printf("MST Cost       : %lld\n", result.totalWeight);
    printf("Memory         : %zu Bytes\n", memoryBytes);
    printf("Execution Time : %.6f ms\n", executionMs);

    if (result.count != V - 1) {
        printf("WARNING: MST does not contain %d edges.\n", V - 1);
    }
}

int main(void)
{
    Edge sparseEdges[SPARSE_EDGES];
    Edge denseEdges[DENSE_EDGES];

    int sparseCount = 0;
    int denseCount = 0;

    AdjList sparseList;
    AdjList denseList;

    int sparseMatrix[V][V];
    int denseMatrix[V][V];

    MSTResult result;
    clock_t start, end;
    double executionMs;
    size_t memoryBytes;

    srand((unsigned int)time(NULL));

    /* Generate both graphs only once */
    generateGraph(sparseEdges, &sparseCount, SPARSE_EDGES);
    generateGraph(denseEdges, &denseCount, DENSE_EDGES);

    /* Build graph representations before timing */
    buildAdjList(&sparseList, sparseEdges, sparseCount);
    buildAdjList(&denseList, denseEdges, denseCount);

    buildAdjMatrix(sparseMatrix, sparseEdges, sparseCount);
    buildAdjMatrix(denseMatrix, denseEdges, denseCount);

    /*
       Case 1: Sparse graph, adjacency list, Kruskal
       Copy the edges because Kruskal sorts its edge array.
    */
    {
        Edge workEdges[SPARSE_EDGES];

        memcpy(workEdges, sparseEdges, sizeof(sparseEdges));

        start = clock();
        result = kruskal(workEdges, sparseCount);
        end = clock();

        executionMs = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
        memoryBytes = kruskalMemoryBytes(&sparseList, sparseCount);

        printResult(1, "Sparse", "Adjacency List", "Kruskal",
                    sparseCount, result, memoryBytes, executionMs);
    }

    /* Case 2: Dense graph, adjacency list, Kruskal */
    {
        Edge workEdges[DENSE_EDGES];

        memcpy(workEdges, denseEdges, sizeof(denseEdges));

        start = clock();
        result = kruskal(workEdges, denseCount);
        end = clock();

        executionMs = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
        memoryBytes = kruskalMemoryBytes(&denseList, denseCount);

        printResult(2, "Dense", "Adjacency List", "Kruskal",
                    denseCount, result, memoryBytes, executionMs);
    }

    /* Case 3: Sparse graph, adjacency matrix, Prim */
    {
        start = clock();
        result = prim(sparseMatrix);
        end = clock();

        executionMs = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
        memoryBytes = primMemoryBytes();

        printResult(3, "Sparse", "Adjacency Matrix", "Prim",
                    sparseCount, result, memoryBytes, executionMs);
    }

    /* Case 4: Dense graph, adjacency matrix, Prim */
    {
        start = clock();
        result = prim(denseMatrix);
        end = clock();

        executionMs = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
        memoryBytes = primMemoryBytes();

        printResult(4, "Dense", "Adjacency Matrix", "Prim",
                    denseCount, result, memoryBytes, executionMs);
    }

    /* Release dynamically allocated adjacency-list nodes */
    freeAdjList(&sparseList);
    freeAdjList(&denseList);

    return 0;
}

