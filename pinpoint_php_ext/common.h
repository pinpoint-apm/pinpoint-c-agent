#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>

typedef struct list{

    void *value;

    struct list* next;
    struct list* pre;
    int size;
}Node;

inline void init_list(Node* list)
{
    list->value = NULL;
    list->pre = list->next = list;
    list->size = 0;
}

inline int is_bottom(Node* list)
{
    return ( list->next == list ) ? (1):(0);
}

inline void* get_top(Node* list)
{
    return list->pre->value;
}

inline uint get_size(Node* list)
{
    return (uint)list->size;
}

inline int is_empty(Node* list)
{
    return list->size == 0;
}

inline Node* push_back(Node *list)
{
    if(is_empty(list))
    {
        list->size++;
        return list;
    }

    Node* node = (Node*)malloc(sizeof(Node));
    if(node){

        Node* last = list->pre;

        /// [last] ->  <- [node]
        last->next = node;
        node->pre  = last;

        ///    [node] ->  <- [list]
        list->pre  = node;
        node->next = list;

        node->value = NULL;
        list->size++;
    }
    return node;
}

inline void* pop_back(Node*list)
{

    if(is_empty(list) ) // empty
    {
        return NULL;
    }
    else
    {
        void* value = NULL;

        if(list->next == list)
        {
            value = list->value;
            list->value = NULL;
        }
        else
        {
            Node* last = list->pre;
            Node* pre  = last->pre;
            list->pre = pre;
            pre->next = list;
            value = last->value;
            free(last);
        }

        list->size--;
        return value;
    }
}

#endif /* COMMON_H_ */
