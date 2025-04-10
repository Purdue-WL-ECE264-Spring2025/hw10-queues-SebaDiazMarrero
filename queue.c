#include "queue.h"
#include "tile_game.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void enqueue(struct queue *q, struct game_state state) {
    if(q==NULL){
        return;
    }

    uint64_t serialized = serialize(state);
    insert_at_tail(&q->data, serialized);
}

struct game_state dequeue(struct queue *q) {
    if(q==NULL || q->data.head==NULL){
        return (struct game_state){0};
    }
    
    uint64_t serialized = remove_from_head(&q->data);
    return deserialize(serialized);
}

int number_of_moves(struct game_state start) {
    
    uint8_t goal[4][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12},
        {13,14, 15,  0}
    };

    bool solved = true;
    for (int i = 0; i < 4 && solved; i++){
        for (int j = 0; j < 4; j++){
            if(start.tiles[i][j] != goal[i][j]){
                solved = false;
                break;
            }
        }
    }
    if (solved){
        return start.num_steps;
    }

    int visitedCapacity = 1024;
    int visitedCount = 0;
    uint64_t *visited = malloc(visitedCapacity * sizeof(uint64_t));
    if(!visited){
        fprintf(stderr, "Memory allocation error for visited array. \n");
        return -1;
    }

    uint64_t startSerial = serialize(start);
    visited[visitedCount++] = startSerial;

    struct queue q;
    q.data.head = NULL;
    enqueue(&q, start);

    while(q.data.head != NULL){
        struct game_state current = dequeue(&q);

        solved = true;
        for(int i = 0; i < 4 && solved; i++){
            for(int j = 0; j < 4; j++){
                if(current.tiles[i][j] != goal[i][j]){
                    solved = false;
                    break;
                }
            }
        }
        if(solved){
            free(visited);
            return current.num_steps;
        }

        struct game_state next;
        uint64_t serial;
        bool found;

        if(current.empty_row > 0){
            next = current;

            {
                uint8_t tmp = next.tiles[next.empty_row - 1][next.empty_col];
                next.tiles[next.empty_row - 1][next.empty_col] = 0;
                next.tiles[next.empty_row][next.empty_col] = tmp;
            }
            next.empty_row--;
            next.num_steps = current.num_steps + 1;
            serial = serialize(next);

            found = false;
            for(int k = 0; k < visitedCount; k++){
                if(visited[k] == serial){
                    found = true;
                    break;
                }
            }
            if(!found){
                if(visitedCount >= visitedCapacity){
                    visitedCapacity *= 2;
                    uint64_t *temp = realloc(visited, visitedCapacity * sizeof(uint64_t));
                    if(!temp){
                        free(visited);
                        return -1;
                    }
                    visited = temp;
                }
                visited[visitedCount++] = serial;
                enqueue(&q, next);

            }
        }

        if(current.empty_col < 3){
            next = current;
            {
                uint8_t tmp = next.tiles[next.empty_row][next.empty_col + 1];
                next.tiles[next.empty_row][next.empty_col + 1] = 0;
                next.tiles[next.empty_row][next.empty_col] = tmp;
            }
            next.empty_col++;
            next.num_steps = current.num_steps + 1;
            serial = serialize(next);
            found = false;
            for(int k = 0; k < visitedCount; k++){
                if(visited[k] == serial){
                    found = true;
                    break;
                }
            }
            if(!found){
                if(visitedCount >= visitedCapacity){
                    visitedCapacity *= 2;
                    uint64_t *temp = realloc(visited, visitedCapacity * sizeof(uint64_t));
                    if(!temp){
                        free(visited);
                        return -1;
                    }
                    visited = temp;
                }
                visited[visitedCount++] = serial;
                enqueue(&q, next);
            }
        }

    }
    free(visited);
    return -1;
    
}
