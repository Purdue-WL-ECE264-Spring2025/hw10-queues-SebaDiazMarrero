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
    return -1;
}
