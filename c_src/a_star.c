#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 1000
#define MAX_EDGES 10000
#define MAX_CITY_NAME 100
#define MAX_ROUTE_NAME 100

typedef struct {
  int to;
  double cost;
  char route_name[MAX_ROUTE_NAME];
} Edge;

typedef struct {
  int node;
  double f;
} PriorityQueueNode;

typedef struct {
  Edge edges[MAX_EDGES];
  int edge_count;
} AdjacencyList;

AdjacencyList graph[MAX_NODES];
double heuristics[MAX_NODES];
double g_score[MAX_NODES];
int came_from[MAX_NODES];
int visited[MAX_NODES];
char city_names[MAX_NODES][MAX_CITY_NAME];

// Min-heap priority queue
PriorityQueueNode pq[MAX_EDGES];
int pq_size = 0;

void pq_push(int node, double f) {
  int i = pq_size++;
  while (i > 0 && pq[(i - 1) / 2].f > f) {
    pq[i] = pq[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  pq[i].node = node;
  pq[i].f = f;
}

int pq_pop() {
  int min_node = pq[0].node;
  PriorityQueueNode last = pq[--pq_size];

  int i = 0;
  while (i * 2 + 1 < pq_size) {
    int left = i * 2 + 1, right = i * 2 + 2, smallest = i;
    if (pq[left].f < pq[smallest].f)
      smallest = left;
    if (right < pq_size && pq[right].f < pq[smallest].f)
      smallest = right;
    if (smallest == i)
      break;
    pq[i] = pq[smallest];
    i = smallest;
  }
  pq[i] = last;
  return min_node;
}

int pq_empty() { return pq_size == 0; }

void read_city_mapping(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("City mapping file");
    exit(EXIT_FAILURE);
  }

  int node_id;
  char name[MAX_CITY_NAME];
  while (fscanf(file, "%d = %[^\n]", &node_id, name) == 2) {
    strcpy(city_names[node_id], name);
  }

  fclose(file);
}

void read_graph(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Graph file");
    exit(EXIT_FAILURE);
  }

  int from, to;
  double cost;
  char route[MAX_ROUTE_NAME];
  while (fscanf(file, "%d %d %lf %s", &from, &to, &cost, route) == 4) {
    Edge e = {to, cost};
    strcpy(e.route_name, route);
    graph[from].edges[graph[from].edge_count++] = e;
    // Reverse direction
    Edge e2 = {from, cost};
    strcpy(e2.route_name, route);
    graph[to].edges[graph[to].edge_count++] = e2;
  }

  fclose(file);
}

void read_heuristic(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Heuristic file");
    exit(EXIT_FAILURE);
  }

  int node;
  double h;
  while (fscanf(file, "%d %lf", &node, &h) == 2) {
    heuristics[node] = h;
  }

  fclose(file);
}

void reconstruct_path(int start, int goal) {
  int path[MAX_NODES];
  int path_len = 0;
  for (int at = goal; at != -1; at = came_from[at]) {
    path[path_len++] = at;
  }

  printf("\nOptimized Path:\n");
  for (int i = path_len - 1; i >= 0; i--) {
    printf("%s", city_names[path[i]]);
    if (i > 0)
      printf(" -> ");
  }
  printf("\n\nRoutes Used:\n");

  for (int i = path_len - 1; i > 0; i--) {
    int from = path[i];
    int to = path[i - 1];
    // Find the route name in the graph
    for (int j = 0; j < graph[from].edge_count; j++) {
      if (graph[from].edges[j].to == to) {
        printf("%s -> %s via %s (%.2lf km)\n", city_names[from], city_names[to],
               graph[from].edges[j].route_name, graph[from].edges[j].cost);
        break;
      }
    }
  }
  printf("\n");
}

void a_star(int start, int goal) {
  for (int i = 0; i < MAX_NODES; i++) {
    g_score[i] = DBL_MAX;
    came_from[i] = -1;
    visited[i] = 0;
  }

  g_score[start] = 0;
  pq_push(start, heuristics[start]);

  while (!pq_empty()) {
    int current = pq_pop();

    if (current == goal) {
      reconstruct_path(start, goal);
      return;
    }

    if (visited[current])
      continue;
    visited[current] = 1;

    for (int i = 0; i < graph[current].edge_count; i++) {
      Edge edge = graph[current].edges[i];
      double tentative_g = g_score[current] + edge.cost;

      if (tentative_g < g_score[edge.to]) {
        g_score[edge.to] = tentative_g;
        came_from[edge.to] = current;
        pq_push(edge.to, tentative_g + heuristics[edge.to]);
      }
    }
  }

  printf("No path found.\n");
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    fprintf(stderr,
            "Usage: %s [graph_file] [heuristic_file] [city_mapping_file] "
            "[start_node] [end_node]\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  int start_node = atoi(argv[4]);
  int end_node = atoi(argv[5]);

  read_city_mapping(argv[3]);
  read_graph(argv[1]);
  read_heuristic(argv[2]);

  a_star(start_node, end_node);

  return EXIT_SUCCESS;
}
