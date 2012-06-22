#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED



#endif // STRUCTS_H_INCLUDED
/*
    定义四条链表
    1，用户链表
    2，每位用户拥有的商品链表ItemList
    3，相似用户链表SimUserList
    4，推荐商品链表ItemUnHaveList
    获得数据以后，会对数据进行预处理，先获得每位用户所购买的商品信息
    然后进行n(n - 1)/2 次扫描获得每位用户的相似用户列表，并计算出相似度，剔除相似度低于0.1的客户
*/


#define  TRUE          1
#define  FALSE         0

typedef struct ItemNode{

  int id;
  double score;
  int  degree;
  char own;
  struct ItemNode *next;
  struct ItemNode *self;
}ItemNode;

typedef struct ItemList{

    int counter;
    struct ItemNode *next;
}ItemList;

typedef struct SimNode{

    int id;
    float sim;
    struct SimNode *next;
    struct uNode   *original;
}SimNode;

typedef struct SimUserList{
    int counter;
    float sum;
    struct SimNode  *next;
}SimUserList;

typedef struct DiffNode{

    int id;
    float weight;
    int   degree;
    struct DiffNode *next;
}DiffNode;

typedef struct DiffList{
    int counter;
    struct DiffNode *next;
}DiffList;

typedef struct uNode{

    int id;

    struct ItemList  *iList;
    struct DiffList  *dList;
    struct SimUserList *sList;
    struct uNode       *self;
    struct uNode       *next;
}uNode;

typedef struct uList{

    int counter;
    struct uNode  *next;
}uList;


void  uListAddItem(uList *header,int id,int iid,ItemNode *self);
ItemNode* iListAddItem(ItemList *header,int id,ItemNode *self,int *position,ItemNode **reclist);
void  sUserListAddItem(SimUserList *header,int id,float sim,uNode *self);
void  iunHaveList(DiffList *header,int id,float weight,int degree);

float sim(ItemList *user1H,ItemList *user2H,int TOT);
float getScore(ItemList *header,int id);
char searchItemList(ItemList *header,int id);
int findDiffItem(ItemList *target,ItemList *sim);
float Get_QIJL(DiffList *target,DiffList *sim);


float sim(ItemList *user1H,ItemList *user2H,int TOT){

    //int TOT = 0;
    float TOTUSER,TOTUSS;
    //int li,lk;
    float sim;

    //ItemNode  *list1,*list2;

    /*list1 = user1H->next;
    list2 = user2H->next;*/

    /*if( user1H->counter < user2H->counter) TOTUSER = user1H->counter;
    else TOTUSER = user2H->counter;*/

    /*for(li = 0;li < user2H->counter;li++){
        list1 = user1H->next;

        for(lk = 0;lk < user1H->counter;lk++){

            if(list1->id == list2->id){

                TOT++;
            }
            list1 = list1->next;
        }
        list2 = list2->next;
    }*/

    TOTUSER = user1H->counter;
    TOTUSS  = user2H->counter;

    sim = TOT / sqrt((TOTUSER * TOTUSS));

    return sim;
}


void uListAddItem(uList *header,int cid,int iid,ItemNode *self){

    int i;
    uNode *list;
    char flag = FALSE;

    if(header->counter != 0){

        list = header->next;

        for(i = 0;i < header->counter;i++){

            if( list->id == cid) {
            //find user in the list & insert
                iListAddItem(list->iList,iid,self,NULL,NULL);
                flag = TRUE;
                break;
            }
            else if(list->next != NULL){
                list = list->next;
            }
        }

        if( flag != TRUE){

        list->next = malloc(sizeof(uNode));
        list = list->next;
        list->id = cid;

        list->dList = malloc(sizeof(DiffList));
        list->dList->counter = 0;
        list->dList->next    = NULL;

        list->iList = malloc(sizeof(ItemList));
        list->iList->counter = 0;
        list->iList->next    = NULL;

        list->sList = malloc(sizeof(SimUserList));
        list->sList->counter = 0;
        list->sList->sum = 0;
        list->sList->next   = NULL;

        list->self   = NULL;

        list->next = NULL;

        header->counter++;

        /*list->iList->next = malloc(sizeof(ItemNode));
        list->iList->counter++;
        list->iList->next->id = iid;*/
        iListAddItem(list->iList,iid,self,NULL,NULL);

        }
        //printf("ID is %d\n",list->id);
    }
    else {
        header->next = malloc(sizeof(uNode));
        header->next->id = cid;

        header->next->dList = malloc(sizeof(DiffList));
        header->next->dList->counter = 0;
        header->next->dList->next    = NULL;

        header->next->iList = malloc(sizeof(ItemList));
        header->next->iList->counter = 0;
        header->next->iList->next    = NULL;

        header->next->sList = malloc(sizeof(SimUserList));
        header->next->sList->counter = 0;
        header->next->sList->sum     = 0;
        header->next->sList->next    = NULL;

        header->next->self   = NULL;

        header->next->next = NULL;

        header->counter++;

        iListAddItem(header->next->iList,iid,self,NULL,NULL);
    }
}

ItemNode* iListAddItem(ItemList *header,int id,ItemNode *self,int *position,ItemNode **reclist ){

    int i;
    ItemNode *list;
    char  flag = FALSE;
    if(header->counter != 0){

        list = header->next;

        for(i = 0;i < header->counter;i++){

            if( list->id == id) {
                list->degree++;
                flag  = TRUE;
                break;
            }
            else if(list->next != NULL){
                list = list->next;
            }
        }
        if( flag != TRUE){

        list->next = malloc(sizeof(ItemNode));
        list = list->next;
        list->id = id;
        list->score = 0;
        list->self = self;
        list->degree = 1;
        list->own    = FALSE;
        //list->score = score;
        list->next = NULL;
        if( (position != NULL) &&(reclist != NULL) ){
            (*position)++;
            reclist[*position] = list;
        }
        header->counter++;
        }

    }
    else {
        header->next = malloc(sizeof(ItemNode));
        list = header->next;
        list->id = id;
        list->score = 0;
        list->self = self;
        list->degree = 1;
        list->own    = FALSE;
        //header->next->score = score;
        list->next = NULL;
        if( (position != NULL) &&(reclist != NULL) ){
            (*position)++;
            reclist[*position] = list;
        }

        header->counter++;
    }
    return list;
}

void sUserListAddItem(SimUserList *header,int id,float sim,uNode *self){


    SimNode *list;

    if(header->counter != 0){

        list = header->next;

        while(list->next != NULL){

            list = list->next;
        }
        list->next = malloc(sizeof(SimNode));
        list = list->next;
        list->id  = id;
        list->sim = sim;
        list->original = self;
        list->next = NULL;

        header->counter++;
        header->sum += sim;

}
    else {
        header->next = malloc(sizeof(SimNode));
        header->next->id = id;
        header->next->sim = sim;
        header->next->original = self;
        header->next->next = NULL;
        header->counter++;
        header->sum += sim;
    }
}

void iunHaveList(DiffList *header,int id,float weight,int degree){

    char found = FALSE;
    int step = 0;
    DiffNode *backward,*forward,*middle;

    //if((header->counter != 0) && (header->counter <= RECOMMENND_MAX)){
    if(header->counter != 0){
        forward = header->next;
        backward = NULL;
        middle = NULL;
        //printf("F%d\n",header->counter);
        while( (forward != NULL) && (found != TRUE) ){
        if(forward->id != id){
            if( forward->weight > weight){

                if( forward->next == NULL){

                    //if(header->counter < RECOMMENND_MAX){
                        forward->next = malloc(sizeof(DiffNode));
                        backward  = forward->next;
                        backward->id = id;
                        backward->next = NULL;
                        backward->weight = weight;
                        backward->degree = degree;

                        found = TRUE;
                        header->counter++;
                        //printf("Add End %d\n",header->counter);
                    //}
                }
            }
            else if( forward->weight <= weight){

                                if( step == 0){

                                    middle = malloc(sizeof(DiffNode));
                                    //printf("Step 0\n");
                                    middle->next = header->next;
                                    header->next = middle;
                                    middle->id = id;
                                    middle->weight = weight;
                                    middle->degree = degree;

                                    header->counter++;
#ifdef   RECOMMENND_MAX
    if(header->counter > RECOMMENND_MAX){

     backward = header->next;
     forward = backward->next;

     while( forward->next != NULL){
        forward = forward->next;
        backward = backward->next;
    }

    backward->next = NULL;
    header->counter = RECOMMENND_MAX;
    //printf("Add Middle success %d\n",header->counter);

}
#endif
                                        found = TRUE;
                                }
                                else if(step != 0){

                                    middle = malloc(sizeof(DiffNode));
                                    backward->next = middle;
                                    middle->next  = forward;

                                    middle->id = id;
                                    middle->weight = weight;
                                    middle->degree = degree;

                                    header->counter++;
#ifdef   RECOMMENND_MAX
if(header->counter > RECOMMENND_MAX){

     backward = header->next;
     forward = backward->next;

     while( forward->next != NULL){
        forward = forward->next;
        backward = backward->next;
    }

    backward->next = NULL;
    header->counter = RECOMMENND_MAX;

}
#endif
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
        header->next = malloc(sizeof(DiffNode));
        header->next->id = id;
        header->next->weight = weight;
        header->next->degree = degree;
        header->next->next = NULL;
        header->counter++;
        //printf("Add Head\n");

    }
}

char searchItemList(ItemList *header,int id){

    ItemNode *list;
    list = header->next;

    while(list != NULL){

        if(list->id == id) return TRUE;

        list = list->next;
    }

    return  FALSE;
}

float getScore(ItemList *header,int id){
    ItemNode *list;
    list = header->next;
    while(list != NULL){
        if( list->id == id) return 1;
        list = list->next;
    }
    return 0;
}

int findDiffItem(ItemList *target,ItemList *sim){

    ItemNode *ptrTarget,*ptrSim;
    int  ct = 0;

    ptrSim = sim->next;

    while( ptrSim != NULL){
        ptrTarget = target->next;

        /*while( ptrTarget != NULL){

            if(ptrTarget->id == ptrSim->id) {
                ct++;
                break;
            }
            ptrTarget = ptrTarget->next;
        }*/
        if( ptrSim->self->own != FALSE) ct++;

        ptrSim = ptrSim->next;
    }
    return ct;
}

float Get_QIJL(DiffList *target,DiffList *sim){

    DiffNode *ptrTarget,*ptrSim;
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

/*userList = userHeader->next;
    while( userList != NULL){
        printf("testing\n");
        ptrSim = userHeader->next;
        while( ptrSim != NULL){
            if( userList->id != ptrSim->id){
                DiffItemCounter = findDiffItem(userList->iList,ptrSim->iList);
                if(DiffItemCounter != 0){
                    simValue = sim(userList->iList,ptrSim->iList,DiffItemCounter);
                    printf("Sim:%f\n",simValue);
                    sUserListAddItem(userList->sList,ptrSim->id,simValue);
                }
            }
            ptrSim = ptrSim->next;
        }
        userList = userList->next;
    }*/
