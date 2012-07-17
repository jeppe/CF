#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define TRUE  1
#define FALSE 0
#define L     20

typedef struct ItemNode{
    int id;
    int degree;
    double score;
    struct ItemNode *self;
    char own;
    struct ItemNode *next;
}ItemNode;

typedef struct UIList{
    int counter;
    struct ItemNode *next;
}UIList;

typedef struct RecList{
    int counter;
    struct ItemNode *next;
}RecList;

typedef struct ItemList{
    int counter;
    struct ItemNode *next;
}ItemList;

typedef struct uNode{
    int id;
    double score;
    int degree;
    struct uNode  *self;
    struct UIList *uilist;
    struct RecList *rlist;
    struct uNode   *next;
}uNode;

typedef struct uList{
    int counter;
    struct uNode *next;
}uList;

void UListInsert(uList *header,int cid,int iid,ItemNode *self);//往用户列表中按照扫描顺序插入节点
ItemNode *IListInsert(ItemList *header,int iid,int *position,ItemNode **reclist);//往商品列表中有序的插入节点
void UIListInsert(UIList *header,int iid,ItemNode *self);//往用户购买过的商品列表中有序的插入节点
void RECListInsert(RecList *header,int id,int degree,double score);//往用户推荐列表中有顺序的插入节点
void IListSequence(ItemList *header,int id,int degree);

/*----------------------------------------------------Defination of Quick Sort Handling----*/
void swap(ItemNode **a,ItemNode **b);
int  division(ItemNode **list,int left,int right,int pivot_index);
void quickSort(ItemNode **list,int left,int right);
void insert_sort(ItemNode **list,int left,int right);
/*-----------------------------------------------------End---------------------------------*/

int  get_nozero_position(ItemNode **list,int left,int right);

void conform_rec(ItemList *ilist,uList *ulist);
void initial_rec(ItemNode **list,uList *ulist,int index);

float Get_QIJL(RecList *target,RecList *sim);
char found(UIList *header,int id);

void conform_rec(ItemList *ilist,uList *ulist){
    uNode *user;
    int i;
    ItemNode *itemlist,*uilist;
    for(i = 0;i < 1;i++){
        user = ulist->next;
        itemlist = ilist->next;
        while(user != NULL){
            uilist = user->uilist->next;

            while(uilist != NULL){
                user->score += uilist->self->score / user->degree;
                uilist = uilist->next;
            }
            user = user->next;
        }
        while(itemlist != NULL){
            itemlist->score = 0;
            itemlist = itemlist->next;
        }
        user = ulist->next;
        while(user != NULL){
            uilist = user->uilist->next;

            while(uilist != NULL){
                uilist->self->score += user->score / uilist->self->degree;
                uilist = uilist->next;
            }
            user = user->next;
        }
    }

}

void initial_rec(ItemNode **list,uList *ulist,int index){
    int i;
    uNode *userlist;

    if(ulist != NULL){
        userlist = ulist->next;
            while(userlist != NULL){
                userlist->score = 0;
                userlist = userlist->next;
            }
            for(i = 0;i <= index;i++){
                if(list[i]->own != TRUE)list[i]->score = 0;
                else list[i]->score = 1;
            }
        }
    else{
        for(i = 0;i <= index;i++){
            list[i]->score = 0;
        }
    }
}

void UIListInsert(UIList *header,int iid,ItemNode *self){

    int i;
    ItemNode *list;
    char flag = FALSE;
    if(header->counter != 0){

        list = header->next;

        for(i = 0;i < header->counter;i++){

            if( list->id == iid) {
                flag = TRUE;
                break;
            }
            else if(list->next != NULL){
                list = list->next;
            }
        }
        if( flag != TRUE){

        list->next = malloc(sizeof(ItemNode));
        list = list->next;
        list->id = iid;
        list->self = self;
        list->next = NULL;

        header->counter++;
        }

}
    else {
        header->next = malloc(sizeof(ItemNode));
        header->next->id = iid;
        header->next->self = self;
        header->next->next = NULL;
        header->counter++;
    }

}

ItemNode *IListInsert(ItemList *header,int id,int *position,ItemNode **reclist){

    ItemNode *list,*lastNode;
    int      i;
    char flag = FALSE;

     if(header->next == NULL){
        header->next = malloc(sizeof(ItemNode));
        list = header->next;
        list->id = id;
        list->degree = 1;
        list->own    = FALSE;

        list->next = NULL;
        if( (position != NULL) &&(reclist != NULL) ){
            (*position)++;
            reclist[*position] = list;
        }
        header->counter++;
     }
     else{
            list = header->next;
            for(i = 0;i < header->counter;i++){

                if( list->id == id) {
                    list->degree++;
                    flag = TRUE;
                    break;
                }
                else if(list->next != NULL){
                    list = list->next;
                }
            }
            //printf("Flag:%d\n",flag);
        /*while( (list != NULL) && (flag != TRUE)){
            if( list->next == NULL)lastNode = list;
            if( list->id == id){
                list->degree++;
                flag = TRUE;
            }
            list = list->next;
        }*/
            if( flag != TRUE ){
            //printf("Testing\n");

            list->next = malloc(sizeof(ItemNode));
            list = list->next;
            list->id = id;
            list->degree = 1;
            list->own    = FALSE;

            list->next = NULL;
            if( (position != NULL) &&(reclist != NULL) ){
                (*position)++;
                reclist[*position] = list;
                }
            header->counter++;
            }
        }
    return      list;
}

char found(UIList *header,int id){
    ItemNode *list;
    list = header->next;

    while(list != NULL){
        if( list->id == id){
            return TRUE;
        }
        list = list->next;
    }

    return FALSE;
}
void RECListInsert(RecList *header,int id,int degree,double score){
    char found = FALSE;
    int step = 0;
    ItemNode *backward,*forward,*middle;

    if( header->counter != 0 ){

        forward = header->next;
        backward = NULL;
        middle = NULL;
        //printf("F%d\n",header->counter);
        while( (forward != NULL) && (found != TRUE) ){
        if(forward->id != id){
            if( forward->score > score){

                if( forward->next == NULL){

                        forward->next = malloc(sizeof(ItemNode));
                        backward  = forward->next;
                        backward->id = id;
                        backward->score = score;
                        backward->degree = degree;
                        backward->next = NULL;

                        found = TRUE;
                        header->counter++;
                        //printf("Add End %d\n",header->counter);

                }
            }
            else if( forward->score < score){


                                if( step == 0){

                                    middle = malloc(sizeof(ItemNode));
                                    //printf("Step 0\n");
                                    middle->next = header->next;
                                    header->next = middle;
                                    middle->id = id;
                                    middle->degree = degree;
                                    middle->score  = score;

                                    header->counter++;
                                        /*if(header->counter > RECOMMENND_MAX){

                                         backward = header->next;
                                         forward = backward->next;

                                         while( forward->next != NULL){
                                            forward = forward->next;
                                            backward = backward->next;
                                        }

                                        backward->next = NULL;
                                        header->counter = RECOMMENND_MAX;
                                        //printf("Add Middle success %d\n",header->counter);

                                    }*/

                                        found = TRUE;
                                }
                                else if(step != 0){

                                    middle = malloc(sizeof(ItemNode));
                                    backward->next = middle;
                                    middle->next  = forward;
                                    //printf("Step  No  Zero\t\t");
                                    //getchar();
                                    middle->id = id;
                                    middle->degree = degree;
                                    middle->score  = score;

                                    header->counter++;

                                    found = TRUE;
                                }

            }else if( (forward->score == score)  && (forward->id != id) ){

                        if( step == 0){

                                    middle = malloc(sizeof(ItemNode));

                                    middle->next = header->next;
                                    header->next = middle;
                                    middle->id = id;
                                    middle->degree = degree;
                                    middle->score  = score;

                                    header->counter++;

                                    found = TRUE;
                                }
                                else if(step != 0){

                                    middle = malloc(sizeof(ItemNode));
                                    backward->next = middle;
                                    middle->next  = forward;

                                    middle->id = id;
                                    middle->degree = degree;
                                    middle->score  = score;

                                    header->counter++;

                                    found = TRUE;
                                }
                }
        }
        else  {
            break;
        }
            backward = forward;
            forward = forward->next;
            step++;
        }

}
    else if(header->counter == 0) {
        header->next = malloc(sizeof(ItemNode));
        header->next->id = id;
        header->next->degree = degree;
        header->next->score  = score;
        header->next->next = NULL;
        header->counter++;
        //printf("Add Head\n");

    }
}

void IListSequence(ItemList *header,int id,int degree){
    char found = FALSE;
    int step = 0;
    ItemNode *backward,*forward,*middle;

    if( header->counter != 0 ){

        forward = header->next;
        backward = NULL;
        middle = NULL;
        //printf("F%d\n",header->counter);
        while( (forward != NULL) && (found != TRUE) ){
        if(forward->id != id){
            if( forward->degree > degree){

                if( forward->next == NULL){

                        forward->next = malloc(sizeof(ItemNode));
                        backward  = forward->next;
                        backward->id = id;
                        backward->degree = degree;
                        backward->next = NULL;

                        found = TRUE;
                        header->counter++;
                        //printf("Add End %d\n",header->counter);

                }
            }
            else if( forward->degree < degree){


                                if( step == 0){

                                    middle = malloc(sizeof(ItemNode));
                                    //printf("Step 0\n");
                                    middle->next = header->next;
                                    header->next = middle;
                                    middle->id = id;
                                    middle->degree = degree;

                                    header->counter++;
                                        /*if(header->counter > RECOMMENND_MAX){

                                         backward = header->next;
                                         forward = backward->next;

                                         while( forward->next != NULL){
                                            forward = forward->next;
                                            backward = backward->next;
                                        }

                                        backward->next = NULL;
                                        header->counter = RECOMMENND_MAX;
                                        //printf("Add Middle success %d\n",header->counter);

                                    }*/

                                        found = TRUE;
                                }
                                else if(step != 0){

                                    middle = malloc(sizeof(ItemNode));
                                    backward->next = middle;
                                    middle->next  = forward;
                                    //printf("Step  No  Zero\t\t");
                                    //getchar();
                                    middle->id = id;
                                    middle->degree = degree;

                                    header->counter++;

                                    found = TRUE;
                                }

            }else if( (forward->degree == degree)  && (forward->id != id) ){

                        if( step == 0){

                                    middle = malloc(sizeof(ItemNode));

                                    middle->next = header->next;
                                    header->next = middle;
                                    middle->id = id;
                                    middle->degree = degree;

                                    header->counter++;

                                    found = TRUE;
                                }
                                else if(step != 0){

                                    middle = malloc(sizeof(ItemNode));
                                    backward->next = middle;
                                    middle->next  = forward;

                                    middle->id = id;
                                    middle->degree = degree;

                                    header->counter++;

                                    found = TRUE;
                                }
                }
        }
        else  {
            break;
        }
            backward = forward;
            forward = forward->next;
            step++;
        }

}
    else if(header->counter == 0) {
        header->next = malloc(sizeof(ItemNode));
        header->next->id = id;
        header->next->degree = degree;
        header->next->next = NULL;
        header->counter++;
        //printf("Add Head\n");

    }
}
void UListInsert(uList *header,int cid,int iid,ItemNode *self){

    int i;
    uNode *list;
    uNode *lastNode;
    char found = FALSE;
    /*header->next = malloc(sizeof(uNode));
    header->counter++;
    header->next->id = 123;*/
    if(header->counter != 0){

        list = header->next;

        for(i = 0;i < header->counter;i++){
            if( list->next == NULL){
                lastNode = list;
            }
            if( list->id == cid) {
            //find user in the list & insert
                UIListInsert(list->uilist,iid,self);
                list->degree++;
                found = TRUE;
                break;
            }
            else if(list->next != NULL){
                list = list->next;
            }
        }

        if( found == FALSE){

        list->next = malloc(sizeof(uNode));
        list = list->next;
        list->id = cid;
        list->degree = 1;
        list->score  = 0;

        list->uilist = malloc(sizeof(UIList));
        list->uilist->counter = 0;
        list->uilist->next = NULL;

        list->rlist = malloc(sizeof(ItemNode));
        list->rlist->counter = 0;
        list->rlist->next = NULL;

        list->self   = NULL;
        list->next = NULL;

        header->counter++;

        /*list->iList->next = malloc(sizeof(ItemNode));
        list->iList->counter++;
        list->iList->next->id = iid;*/
        UIListInsert(list->uilist,iid,self);

        }
        //printf("ID is %d\n",list->id);
    }
    else {
        header->next = malloc(sizeof(uNode));
        list = header->next;
        list->id = cid;
        list->degree = 1;
        list->score = 0;

        list->uilist = malloc(sizeof(UIList));
        list->uilist->counter = 0;
        list->uilist->next = NULL;

        list->rlist = malloc(sizeof(ItemNode));
        list->rlist->counter = 0;
        list->rlist->next = NULL;

        list->self   = NULL;
        list->next = NULL;

        header->counter++;

        UIListInsert(list->uilist,iid,self);
    }
}

void swap(ItemNode **a,ItemNode **b){
    ItemNode *tem;

    tem = *b;
    *b  = *a;
    *a  = tem;
}
int division(ItemNode **list,int left,int right,int pivot_index){
    double pivot = list[pivot_index]->score;
    int storeIndex = left;
    int i;
    swap(&list[pivot_index],&list[right]);

    for(i = left;i <= right - 1;i++){

        if( list[i]->score > pivot){
            swap(&list[i],&list[storeIndex]);
            storeIndex++;
        }
    }
    swap(&list[right],&list[storeIndex]);
    return storeIndex;
}
void insert_sort(ItemNode **list,int left,int right){
    ItemNode *sentry;
    int    i,j;
    //printf("enter insert sort\n");
    for(i = left + 1;i <= right;i++){
        if( list[i]->score > list[i - 1]->score){
            sentry = list[i];
            list[i] = list[i - 1];
            for( j = i -2;(j >= 0) && (sentry->score > list[j]->score);j--) {

                list[j + 1] = list[j];
            }
            list[j + 1] = sentry;
        }
    }
}

void quickSort(ItemNode **list,int left,int right){
    int index,pivot_index = (left + right) / 2;

    if( left < right){
        if( right - left + 1 <= 16){
            insert_sort(list,left,right);
        }
        else{
            index = division(list,left,right,pivot_index);
            quickSort(list,left,index - 1);
            quickSort(list,index + 1,right);

        }
    }
}

float Get_QIJL(RecList *target,RecList *sim){
    ItemNode *ptrTarget,*ptrSim;
    float  ct = 0;

    ptrSim = sim->next;

    while( ptrSim != NULL){
        ptrTarget = target->next;

        while( ptrTarget != NULL){

            if(ptrTarget->id == ptrSim->id) {
                ct++;
                break;
            }
            ptrTarget = ptrTarget->next;
        }
        ptrSim = ptrSim->next;
    }
    return ct;
}
int  get_nozero_position(ItemNode **list,int left,int right){
    int tp = 0;
    int i = 0,j = right;

    if(left < right){
        while( i < j){
            while( (j > i) && (list[j]->score == 0 ) ) j--;

            while( (j > i) && (list[i]->score > 0 ) ) i++;

                if( j == i) {
                    tp = i;
                }
                else{

                    swap(&list[i],&list[j]);
                }

            }
        }

    return tp;
}

#endif // NODE_H_INCLUDED
