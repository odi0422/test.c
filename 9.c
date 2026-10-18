
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V 10
#define E 20
#define INF 1000000000

int graph[V][V];
int dist[V][V];
int nextVertex[V][V];

typedef struct {
    int u;
    int v;
    int weight;
} Edge;

Edge edges[E];

/* 그래프 초기화 */
void initializeGraph(void) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            graph[i][j] = 0;
        }
    }
}

/* 두 정점 사이에 간선이 있는지 확인 */
int hasEdge(int u, int v) {
    return graph[u][v] != 0;
}

/* 간선 추가 */
void addEdge(int u, int v, int weight) {
    graph[u][v] = weight;
    graph[v][u] = weight;
}

/* 연결 그래프를 만들기 위한 깊이 우선 탐색 */
void dfs(int vertex, int visited[V]) {
    visited[vertex] = 1;

    for (int i = 0; i < V; i++) {
        if (hasEdge(vertex, i) && !visited[i]) {
            dfs(i, visited);
        }
    }
}

/* 모든 정점이 연결되어 있는지 확인 */
int isConnected(void) {
    int visited[V] = {0};
    int count = 0;

    dfs(0, visited);

    for (int i = 0; i < V; i++) {
        count += visited[i];
    }

    return count == V;
}

/* 무작위 가중치 무방향 단순 그래프 생성 */
void generateGraph(void) {
    int count = 0;

    initializeGraph();

    /*
       먼저 모든 정점을 연결하는 트리를 생성한다.
       이렇게 하면 그래프가 반드시 연결된다.
    */
    for (int i = 1; i < V; i++) {
        int parent = rand() % i;
        int weight = rand() % 100 + 1;

        addEdge(parent, i, weight);

        edges[count].u = parent;
        edges[count].v = i;
        edges[count].weight = weight;
        count++;
    }

    /* 남은 간선을 무작위로 추가하여 총 20개로 만든다. */
    while (count < E) {
        int u = rand() % V;
        int v = rand() % V;

        /* 자기 자신과 연결하거나 중복 간선을 만들지 않는다. */
        if (u == v || hasEdge(u, v)) {
            continue;
        }

        int weight = rand() % 100 + 1;

        addEdge(u, v, weight);

        edges[count].u = u;
        edges[count].v = v;
        edges[count].weight = weight;
        count++;
    }
}

/* 생성된 그래프 출력 */
void printGraph(void) {
    printf("Graph Vertices : %d\n", V);
    printf("Graph Edges    : %d\n\n", E);

    printf("%-12s %s\n", "Edge", "Weight");
    printf("--------------------\n");

    for (int i = 0; i < E; i++) {
        printf("(%d, %d)       %d\n",
               edges[i].u,
               edges[i].v,
               edges[i].weight);
    }

    printf("\n");
}

/* Floyd-Warshall 알고리즘 */
void floydWarshall(void) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j) {
                dist[i][j] = 0;
                nextVertex[i][j] = i;
            } else if (graph[i][j] != 0) {
                dist[i][j] = graph[i][j];
                nextVertex[i][j] = j;
            } else {
                dist[i][j] = INF;
                nextVertex[i][j] = -1;
            }
        }
    }

    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] != INF &&
                    dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {

                    dist[i][j] = dist[i][k] + dist[k][j];
                    nextVertex[i][j] = nextVertex[i][k];
                }
            }
        }
    }
}

/* 최단경로 출력 */
void printPath(int u, int v) {
    if (nextVertex[u][v] == -1) {
        printf("No path");
        return;
    }

    printf("%d", u);

    while (u != v) {
        u = nextVertex[u][v];

        if (u == -1) {
            printf(" -> No path");
            return;
        }

        printf(" -> %d", u);
    }
}

/* 모든 정점 쌍의 최단경로와 최단거리 출력 */
void printAllShortestPaths(void) {
    int totalPairs = 0;

    printf("========== Shortest Paths ==========\n\n");

    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            printf("%d -> %d\n", u, v);

            printf("Path     : ");
            printPath(u, v);
            printf("\n");

            printf("Distance : %d\n\n", dist[u][v]);

            totalPairs++;
        }
    }

    printf("Total vertex pairs : %d\n", totalPairs);
}

int main(void) {
    srand((unsigned int)time(NULL));

    generateGraph();

    /* 생성 결과가 올바른지 확인 */
    if (!isConnected()) {
        printf("Error: Graph is not connected.\n");
        return 1;
    }

    printGraph();

    floydWarshall();

    printAllShortestPaths();

    return 0;
}
