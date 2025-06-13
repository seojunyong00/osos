#include <iostream>
#include <mutex>
#include <atomic>
#include "queue.h"

#define MAX_QUEUE_SIZE 1000000

using namespace std;

// 내부 구조는 배열 기반으로 구현

Queue* init(void) {
    Queue* queue = new Queue;
    queue->size = 0;
    return queue;
}

void release(Queue* queue) {
    delete queue;
}

Node* nalloc(Item item) {
    Node* node = new Node;
    node->item = item;
    node->next = nullptr;
    return node;
}

void nfree(Node* node) {
    delete node;
}

Node* nclone(Node* node) {
    if (node == nullptr) return nullptr;
    Node* copy = new Node;
    copy->item = node->item;
    copy->next = nullptr;
    return copy;
}

Reply enqueue(Queue* queue, Item item) {
    lock_guard<mutex> lock(queue->mtx);

    Reply reply = { false, item };

    if (queue->size.load() >= MAX_QUEUE_SIZE) return reply;

    int i = queue->size.load() - 1;
    while (i >= 0 && queue->items[i].key < item.key) {
        queue->items[i + 1] = queue->items[i];
        i--;
    }
    queue->items[i + 1] = item;
    queue->size.fetch_add(1);

    reply.success = true;
    return reply;
}



Reply dequeue(Queue* queue) {
    lock_guard<mutex> lock(queue->mtx);

    Reply reply = { false, {0, nullptr} };
    if (queue->size.load() == 0) return reply;

    reply.success = true;
    reply.item = queue->items[0];

    for (int i = 1; i < queue->size.load(); ++i) {
        queue->items[i - 1] = queue->items[i];
    }
    queue->size.fetch_sub(1);

    return reply;
}



Queue* range(Queue* queue, Key start, Key end) {
    Item temp[MAX_QUEUE_SIZE];
    int count = 0;

    {
        lock_guard<mutex> lock(queue->mtx);
        int size = queue->size.load();
        for (int i = 0; i < size; ++i) {
            Key k = queue->items[i].key;
            if (start <= k && k <= end) {
                temp[count++] = queue->items[i];
            }
        }
    }

    Queue* new_queue = init();
    for (int i = 0; i < count; ++i) {
        enqueue(new_queue, temp[i]);
    }

    return new_queue;
}

