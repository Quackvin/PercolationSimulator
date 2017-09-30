#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <omp.h>
#include <math.h>

#define NCOLS 5
#define NROWS 5
#define PRINT 0
#define PRECISION 5
#define NTHREADS 4 // must be square number

int clusterCount = 0;

// added
typedef struct BoundaryNodeLL {
    int row;
    int col;
    struct BoundaryNodeLL *next;
} boundary_t;

// changed
typedef struct Cluster {
    int node_count;
    bool vertP[NROWS];
    bool horzP[NROWS];
    boundary_t *boundaryHead;
} cluster_t;

typedef struct List {
    int cluster_size;
    bool percolates;
    struct List *next;
} list_t;

typedef struct Site {
    bool north;
    bool south;
    bool east;
    bool west;
    bool seen;
    bool free;
    int count;
} site_t;

typedef struct SearchStack {
    int row;
    int col;
    struct SearchStack *next;
} sstack_t; 

void seedSite(site_t lattice[NROWS][NCOLS], float P) ;
void seedBond(site_t lattice[NROWS][NCOLS], float p) ;
void printLattice(site_t lattice[NROWS][NCOLS], char* perc_type) ;
// need to be able to free
void addBoundary(int row, int col, cluster_t **cluster) ;
// need to make record only node count and boundary node info
void addNodeToCluster(cluster_t **head, int row, int col, int bounds[]) ;
// need to stop at boundary
void searchNode(site_t lattice[NROWS][NCOLS], int row, int col, cluster_t **head, char *perc_type, int bounds[]) ;
bool percolate(cluster_t *head, int perc_dir) ;
// added testing of boundary nodes
list_t *dfs(site_t lattice[NROWS][NCOLS], char *perc_type, int perc_dir, int bounds[]) ;
// need to remove percolation check and do later
void addClusterToList(list_t **head, cluster_t *cluster, int perc_dir) ;
void printPerc(list_t *head) ;
int getLargestCluster(list_t *head) ;
void freeList(list_t *head) ;
bool hasBond(float p) ;
void push(sstack_t **top, int row, int col) ;
void pop(sstack_t **top, int *row, int *col) ;
cluster_t *createCluster(void) ;

int main(int argc, char *argv[]) {

    srand((unsigned int) time(NULL)); //Seeding random number generator
    clock_t start, end;
    double total;
    static site_t lattice[NROWS][NCOLS];
    if (strcmp(argv[1],"s") == 0) seedSite(lattice, strtof(argv[2], NULL));
    if (strcmp(argv[1],"b") == 0) seedBond(lattice, strtof(argv[2], NULL));

    printLattice(lattice, argv[1]);

    start = clock();

    list_t *allClusters[NTHREADS];
    omp_set_num_threads(NTHREADS);
    int tileSize = ceil(NCOLS/sqrt(NTHREADS));
    // printf("Tile size: %d\n", tileSize);
#pragma omp parallel
    {
        int xUpper, xLower, yUpper, yLower;
        xLower = (omp_get_thread_num() % (int)sqrt(NTHREADS)) * tileSize;
        xUpper = xLower + tileSize;
        if (xUpper >= NCOLS) xUpper = NCOLS;
        
        yLower = floor(omp_get_thread_num() / (int)sqrt(NTHREADS)) * tileSize;
        yUpper = yLower + tileSize;
        if (yUpper >= NROWS) yUpper = NROWS;

        int bounds[] = {xLower,xUpper,yLower,yUpper};
        // printf("Thread: %d, xmin: %d, xmax: %d, ymin: %d, ymax: %d\n", omp_get_thread_num(), xLower, xUpper, yLower, yUpper);
        allClusters[omp_get_thread_num()] = dfs(lattice, argv[1], (int) strtol(argv[3], NULL, 10), bounds);
    }

    


    end = clock();
    total = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Process took %fs.\n\n", total);

    return 0;
}

void seedSite(site_t lattice[NROWS][NCOLS], float P) {

    float tP = P * RAND_MAX; //Setting threshold probability

    for (int i = 0; i < NROWS; i++) {
        for (int j = 0; j < NCOLS; j++) {
            lattice[i][j].free = rand() < tP ? false : true;
            lattice[i][j].seen = false;
            lattice[i][j].count = 0;
        }
    }
}

void seedBond(site_t lattice[NROWS][NCOLS], float p) {
    for (int i = 0; i < NROWS; i++) {
        int prev_i = i - 1, next_i = i + 1;
        if (prev_i < 0) prev_i = NROWS - 1;
        if (next_i == NROWS) next_i = 0;

        for (int j = 0; j < NCOLS; j++) {
            int prev_j = j - 1, next_j = j + 1;
            if (prev_j < 0) prev_j = NCOLS - 1;
            if (next_j == NCOLS) next_j = 0;

            lattice[i][j].north = lattice[prev_i][j].south = hasBond(p);
            lattice[i][j].south = lattice[next_i][j].north = hasBond(p);
            lattice[i][j].east = lattice[i][next_j].west = hasBond(p);
            lattice[i][j].west = lattice[i][prev_j].east = hasBond(p);

            lattice[i][j].seen = false;
            lattice[i][j].free = true;
            lattice[i][j].count = 0;
        }
    }
}

bool hasBond(float p) {
    int n = 1;
    for (int i = 0; i < PRECISION; i++) {
        n *= 10;
    }
    float random = (float) (rand() % n) / (float) n;
    return random > p ? true : false;
}

void printLattice(site_t lattice[NROWS][NCOLS], char* perc_type) {
    if (strcmp(perc_type,"s") == 0) {
        for (int i = 0; i < NROWS; i++) {
            for (int j = 0; j < NCOLS; j++) {
                if (lattice[i][j].free == false) {
                    printf("[XX]");
                } else if (lattice[i][j].free == true) {
                    printf("[%2i]", lattice[i][j].count);
                } else {
                    printf("[  ]");
                }
            }
            printf("\n");
        }
    }
    if (strcmp(perc_type,"b") == 0) {
        for (int i = 0; i < NROWS; i++) {
            for (int j = 0; j < NCOLS; j++) {
                if (lattice[i][j].seen) {
                    printf("[%2i]", lattice[i][j].count);
                } else {
                    printf("[  ]");
                }
            }
            printf("\n");
        }
    }
}

list_t *dfs(site_t lattice[NROWS][NCOLS], char *perc_type, int perc_dir, int bounds[]) {
    list_t *clusterlist = NULL;

    for (int i = bounds[2]; i < bounds[3]; i++) {
        for (int j = bounds[0]; j < bounds[1]; j++) {
            if (!lattice[i][j].seen && lattice[i][j].free) {
                clusterCount++;
                cluster_t *head = createCluster();
                searchNode(lattice, i, j, &head, perc_type, bounds);
                addClusterToList(&clusterlist, head, perc_dir);

                // testing
                boundary_t *readBoundary = head->boundaryHead;
                while(readBoundary != NULL){
                    // printf("%d readBoundary: [%d, %d]\n", omp_get_thread_num(), readBoundary->col, readBoundary->row);
                    readBoundary = readBoundary->next;
                }

                free(head);
            }
        }
    }
    return clusterlist;
}


void searchNode(site_t lattice[NROWS][NCOLS], int row, int col, cluster_t **head, char *perc_type, int bounds[]) {
    sstack_t *stack = NULL;
    push(&stack, row, col);

    while (stack != NULL) {
        int p_row, p_col;
        pop(&stack, &p_row, &p_col);
        // constrain row and col
        if (!lattice[p_row][p_col].seen && p_col >= bounds[0] && p_col < bounds[1] && p_row >= bounds[2] && p_row < bounds[3]) {
            lattice[p_row][p_col].seen = true;
            lattice[p_row][p_col].count = clusterCount;
            addNodeToCluster(head, p_row, p_col, bounds);

            int prevRow = p_row - 1, nextRow = p_row + 1;
            int prevCol = p_col - 1, nextCol = p_col + 1;
            if (prevRow < 0) prevRow = NROWS - 1;
            if (nextRow == NROWS) nextRow = 0;
            if (prevCol < 0) prevCol = NCOLS - 1;
            if (nextCol == NCOLS) nextCol = 0;

            if (strcmp(perc_type,"s") == 0) {
                if (lattice[prevRow][p_col].free && !lattice[prevRow][p_col].seen) {
                    push(&stack, prevRow, p_col);
                }
                if (lattice[nextRow][p_col].free && !lattice[nextRow][p_col].seen) {
                    push(&stack, nextRow, p_col);
                }
                if (lattice[p_row][prevCol].free && !lattice[p_row][prevCol].seen) {
                    push(&stack, p_row, prevCol);
                }
                if (lattice[p_row][nextCol].free && !lattice[p_row][nextCol].seen) {
                    push(&stack, p_row, nextCol);
                }
            }

            if (strcmp(perc_type,"b") == 0) {
                if (lattice[p_row][p_col].north && !lattice[prevRow][p_col].seen) {
                    push(&stack, prevRow, p_col);
                }
                if (lattice[p_row][p_col].south && !lattice[nextRow][p_col].seen) {
                    push(&stack, nextRow, p_col);
                }
                if (lattice[p_row][p_col].west && !lattice[p_row][prevCol].seen) {
                    push(&stack, p_row, prevCol);
                }
                if (lattice[p_row][p_col].east && !lattice[p_row][nextCol].seen) {
                    push(&stack, p_row, nextCol);
                }
            }
        }
    }
}


cluster_t *createCluster(void) {
    cluster_t *new_cluster = NULL;
    new_cluster = malloc(sizeof(cluster_t));
    if (new_cluster == NULL) exit(EXIT_FAILURE);
    new_cluster->node_count = 0;
    new_cluster->boundaryHead = NULL;
    return new_cluster;
}

void addBoundary(int row, int col, cluster_t **cluster){
    boundary_t *newBoundary = malloc(sizeof(boundary_t));
    if (newBoundary == NULL) exit(EXIT_FAILURE);
    newBoundary->row = row;
    newBoundary->col = col;
    newBoundary->next = (*cluster)->boundaryHead;
    (*cluster)->boundaryHead = newBoundary;
}

void addNodeToCluster(cluster_t **head, int row, int col, int bounds[]) {
    (*head)->node_count++;
    (*head)->vertP[row] = true;
    (*head)->horzP[col] = true;
    // add to boundary list
    if(col == bounds[0] || col == bounds[1]-1 || row == bounds[2] || row == bounds[3]-1){
        addBoundary(row,col,head);
    }
}

void addClusterToList(list_t **head, cluster_t *cluster, int perc_dir) {
    list_t *new_cluster = malloc(sizeof(list_t));
    if (new_cluster == NULL) exit(EXIT_FAILURE);
    new_cluster->cluster_size = cluster->node_count;
    new_cluster->percolates = percolate(cluster, perc_dir);
    new_cluster->next = *head;
    *head = new_cluster;
}

bool percolate(cluster_t *head, int perc_dir) {
    bool percsV = false;
    bool percsH = false;

    if (perc_dir == 0 || perc_dir == 2) { //vertical percolation
        for (int i = 0; i < NROWS; i++) {
            if (!head->vertP[i]) return false;
        }
        percsV = true;
    }

    if (perc_dir == 1 || perc_dir == 2) { //vertical percolation
        for (int i = 0; i < NCOLS; i++) {
            if (!head->horzP[i]) return false;
        }
        percsH = true;
    }
    if (perc_dir == 0) return (percsV);
    if (perc_dir == 1) return (percsH);
    if (perc_dir == 2) return (percsV && percsH);
    return false;
}

void printPerc(list_t *head) {
    list_t *current = head;

    while (current != NULL) {
        if (current->percolates) {
            printf("Percolation path found.\n");
            return;
        }
        current = current->next;
    }
    printf("None found.\n");
}

int getLargestCluster(list_t *head) {
    list_t *current = head;
    int max = 0;

    while (current != NULL) {
        if (current->cluster_size > max) max = current->cluster_size;
        current = current->next;
    }
    return max;
}

void freeList(list_t *head) {
    list_t *tmp;

    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

void push(sstack_t **top, int row, int col) {
    sstack_t *new_element = malloc(sizeof(sstack_t));
    if (new_element == NULL) exit(EXIT_FAILURE);
    new_element->row = row;
    new_element->col = col;
    new_element->next = *top;
    *top = new_element;
}

void pop(sstack_t **top, int *row, int *col) {
    sstack_t *tmp = NULL;

    if (*top != NULL) {
        tmp = (*top)->next;
        *row = (*top)->row;
        *col = (*top)->col;
        free(*top);
        *top = tmp;
    }
}