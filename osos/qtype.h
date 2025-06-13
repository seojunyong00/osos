#ifndef _QTYPE_H
#define _QTYPE_H

#include <atomic>
#include <mutex>

typedef unsigned int Key;
typedef void* Value;

typedef struct {
    Key key;
    Value value;
} Item;

typedef struct {
    bool success;
    Item item;
} Reply;

// 노드 기반 함수는 여전히 range를 위해 유지
typedef struct node_t {
    Item item;
    struct node_t* next;
} Node;

#define MAX_QUEUE_SIZE 1000000

typedef struct {
    Item items[MAX_QUEUE_SIZE];  // 배열 기반 우선순위 큐
    std::atomic<int> size;       // 현재 큐 크기
    std::mutex mtx;              // 동기화를 위한 뮤텍스
} Queue;

#endif
