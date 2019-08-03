//
// Created by Alon on 06/06/2019.
//

#include <unistd.h>
#include <cstring>
#include <cstdlib>

#define MAX_SIZE (100000000)
#define SBRK_ERROR ((void*)(-1))
#define ALIGNMENT_BYTES (4)
#define NUM_OF_BYTES_TO_SPLIT (128)

typedef struct allocated_meta_data {
    size_t requested_size;
    void *first_byte_in_block;
    bool is_free;
    allocated_meta_data *prev;
    allocated_meta_data *next;
} Meta_data
        __attribute__((aligned(ALIGNMENT_BYTES)));

static Meta_data list_head = {0, NULL, false, NULL, NULL};
// list_head is a dummy node, wil never be freed true

size_t _size_meta_data();
void free(void *p);

bool align_program_break() {
    /// Problem 4 fix
    void *current_prg_brk = sbrk(0);
    if (current_prg_brk == SBRK_ERROR) return false;
    int offset = ((long) current_prg_brk % ALIGNMENT_BYTES);
    if (offset) {
        return sbrk(offset);
    }
    return true;
}

size_t align_number_upward(size_t size) {
    /// Problem 4 fix
    switch (size % 4) {
        case 0:
            return 0;
        case 1:
            return 3;
        case 2:
            return 2;
        case 3:
            return 1;
    }
    return 0; // shouldn't get here
}

void *_helper_on_problem_one(Meta_data *node, size_t size) {
    /// Problem 1 fix
    //assert(node->requested_size >= size);

    signed long remaining_free_space_temp =
            node->requested_size - size - _size_meta_data();
    size_t remaining_free_space = (remaining_free_space_temp >= 0) ?
                                  remaining_free_space_temp : 0;
    if (remaining_free_space < NUM_OF_BYTES_TO_SPLIT) {
        node->is_free = false;
        return node->first_byte_in_block;
    }

    // else 128 or more >> split
    // case making 1 block into 2 blocks:
    void *new_p = (void *) (((char *) node->first_byte_in_block) + size);
    Meta_data *new_node = (Meta_data *) new_p;
    *new_node = (Meta_data) {remaining_free_space, new_node + 1, true, node,
                             node->next};
    node->next = new_node;
    node->is_free = false;
    node->requested_size = size;
    if (new_node->next) new_node->next->prev = new_node;
    new_node->is_free = false; // preparing for free function
    free(new_node + 1); /// problem 2 fix
    return node->first_byte_in_block;
}

void *_helper_on_problem_three(Meta_data *node, size_t size) {
    /// Problem 3 fix: "Wilderness"
    //assert(node && !node->next && size);

    void *res = sbrk(size - node->requested_size); // resizing last node
    if (res == SBRK_ERROR) return NULL;
    node->requested_size = size;
    node->is_free = false;
    return node->first_byte_in_block;
}

void *malloc(size_t size) {
    if (!size || size > MAX_SIZE) return NULL;
    //if (!align_program_break()) return NULL;
    /// this change canceled on 21.06.2019
    Meta_data *node = &list_head;
    size = size + align_number_upward(size); // fix for problem 4


    while (node) { // scan the list for freed space
        if (node->is_free && node->requested_size >= size) {
            return _helper_on_problem_one(node, size);
        }
        if (node->next)
            node = node->next;
        else break; // breaks only for last node in the list
    }

    if (node && node->is_free && !node->next) { //Wilderness
        return _helper_on_problem_three(node, size);
    }

    // not enough free space, allocating:
    void *res = sbrk(_size_meta_data() + size);
    if (res == SBRK_ERROR) return NULL;
    // writing the meta-data
    Meta_data *new_data = (Meta_data *) res;
    *new_data = (Meta_data) {size, new_data + 1, false, node, NULL};
    node->next = new_data;
    return new_data->first_byte_in_block; // returns the start of the data segment
}

void *calloc(size_t num, size_t size) {
    if (!size || !num || size * num > MAX_SIZE) return NULL;
    size = size * num + align_number_upward(size * num); // fix for problem 4
    void *res = malloc(size);
    if (!res) return res;
    return std::memset(res, 0, size);
}

void free(void *p) {
    if (!p) return;
    Meta_data *meta_data = ((Meta_data *) p) - 1;
    if (meta_data->is_free) return;
    meta_data->is_free = true;

    /// Problem 2 fix
    if (meta_data->prev && meta_data->prev->is_free) {
        Meta_data *old_data = meta_data;
        meta_data = old_data->prev;
        meta_data->requested_size = meta_data->requested_size +
                                    old_data->requested_size +
                                    _size_meta_data();
        meta_data->next = old_data->next;
        if (meta_data->next) meta_data->next->prev = meta_data;

    }
    if (meta_data->next && meta_data->next->is_free) {
        Meta_data *old_data = meta_data->next;
        meta_data->requested_size = meta_data->requested_size +
                                    old_data->requested_size +
                                    _size_meta_data();
        meta_data->next = old_data->next;
        if (meta_data->next) meta_data->next->prev = meta_data;
    }
}

void *realloc(void *oldp, size_t size) {
    if (!size || size > MAX_SIZE) return NULL;
    if (!oldp) return malloc(size);
    size = size + align_number_upward(size); // fix for problem 4

    Meta_data *node = ((Meta_data *) oldp) - 1;
    if (node->requested_size >= size) {
        return _helper_on_problem_one(node, size); // splits block if possible
    }
    // ELSE: case new_size > old_size:

    if (node->next && node->next->is_free) {
        // merging with adjacant higher block
        node->requested_size += _size_meta_data() + node->next->requested_size;
        node->next = node->next->next;
        if (node->next) node->next->prev = node;
        return realloc(oldp, size);
    }

    if (!node->next) { // "Wildereness"
        return _helper_on_problem_three(node, size); // enlarge last block
    }

    void *res = malloc(size);
    if (!res) return res;
    res = memcpy(res, oldp, node->requested_size);
    free(oldp); /// problem 2 fix
    return res;
}

size_t _num_free_blocks() {
    Meta_data *node = list_head.next;
    size_t free_blocks_counter = 0;
    while (node) { // scan the list for freed blocks
        free_blocks_counter += node->is_free;
        node = node->next;
    }
    return free_blocks_counter;
}

size_t _num_free_bytes() {
    Meta_data *node = list_head.next;
    size_t free_bytes_counter = 0;
    while (node) { // scan the list for freed blocks
        free_bytes_counter += node->is_free ? node->requested_size : 0;
        node = node->next;
    }
    return free_bytes_counter;
}

size_t _num_allocated_blocks() {
    Meta_data *node = list_head.next;
    size_t allocated_blocks_counter = 0;
    while (node) { // scan the list for freed blocks
        allocated_blocks_counter++;
        node = node->next;
    }
    return allocated_blocks_counter;
}

size_t _num_allocated_bytes() {
    Meta_data *node = list_head.next;
    size_t allocated_bytes_counter = 0;
    while (node) { // scan the list for freed blocks
        allocated_bytes_counter += node->requested_size;
        node = node->next;
    }
    return allocated_bytes_counter;
}

size_t _num_meta_data_bytes() {
    return _num_allocated_blocks() * _size_meta_data();
}

size_t _size_meta_data() {
    return sizeof(struct allocated_meta_data);
}


