/*******************************************************************************
 * Copyright 2019 NAVER Corp
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>

//fix #129 
//origin from https://github.com/naver/pinpoint-c-agent/issues/157
#ifndef uint
typedef unsigned int uint;
#endif

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
