#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "structs.h"

#define L   20

char probeData[] = "./delicious/probe.data",trainData[] = "./delicious/train.data",resultData[] = "./delicious/results.data";

 /*=============定义变量（CID，IID，SCORE，TIME...）======*/
    FILE *probe,*train,*result;
    time_t raw,rawCur;
    double diftime;
    struct tm * timeinfo;

    int  CID,IID,SCORE;
    float simValue,DFWEIGHT = 0.0,sum;
    int   counter = 0,position = -1;
    int   DiffItemCounter = 0,DFID;
    char  flag = TRUE,dou = FALSE;

    uNode  *ptrSim;

    SimNode *SUListTail;
    SimNode *slist;

    DiffList *dListFree,*dListUsed;
    DiffNode *dNode,*td;

    SimUserList *SUFreeList,*SUList;

    uList  *userHeader;

    uList  *probe_UserHeader;

    ItemList *itemHeader;
    ItemNode *itemlist;
    ItemNode *CItem;

    uNode    *userList;
    uNode       *un;

void recBegin(uNode *list,ItemNode **reclist);
void initFreeSimList(SimUserList *header,int len);
void initFreeDiffList(DiffList  *header,int len);
void createSimList(SimUserList *header,int id,float sim,uNode *self);
void createRecItemList(DiffList *header,int id,int degree,float weight);

/*--------------------------------------------------------------------------------------Quick Sort Handling-------------*/
void swap(ItemNode **a,ItemNode **b);
int  division(ItemNode **list,int left,int right,int pivot_index);
void quickSort(ItemNode **list,int left,int right);

/*---------------------------------------------------------------------------------------Initial reclist----------------*/
void initReclist(ItemNode **list);

int main()
{
    SUListTail = NULL;
    ItemNode *reclist[232657] = {NULL};
    printf("Testing\n");

    userHeader = malloc(sizeof(uList));
    userHeader->counter = 0;
    userHeader->next = NULL;

    probe_UserHeader = malloc(sizeof(uList));
    probe_UserHeader->counter = 0;
    probe_UserHeader->next  = NULL;

    itemHeader = malloc(sizeof(ItemList));
    itemHeader->counter = 0;
    itemHeader->next    = NULL;

    SUFreeList = malloc(sizeof(SimUserList));
    SUFreeList->counter = 0;
    SUFreeList->next = NULL;

    SUList = malloc(sizeof(SimUserList));
    SUList->counter = 0;
    SUList->next  = NULL;

    dListFree = malloc(sizeof(DiffList));
    dListFree->counter = 0;
    dListFree->next = NULL;

    dListUsed = malloc(sizeof(DiffList));
    dListUsed->counter = 0;
    dListUsed->next    = NULL;

    probe = fopen(probeData,"r");
    train = fopen(trainData,"r");

    /*====================Initial Train User List====================================*/
    time(&raw);
    //timeinfo = localtime(&raw);
    //printf("time:%s\n",asctime(timeinfo));
    while( fscanf(train,"%d%d",&CID,&IID) != EOF){
        CItem = iListAddItem(itemHeader,IID,NULL,&position,reclist);
        uListAddItem(userHeader,CID,IID,CItem);

    }
    time(&rawCur);
    diftime = difftime(rawCur,raw);
    timeinfo = localtime(&rawCur);
    //printf("Current time is :%s",asctime(timeinfo));
    //printf("Difftime:%lf\n",diftime);

    initFreeSimList(SUFreeList,userHeader->counter);
    //initFreeDiffList(dListFree,itemHeader->counter);

    fclose(train);

    /*==================Initial Test User List===========================*/
    while( fscanf(probe,"%d%d",&CID,&IID) != EOF){
        uListAddItem(probe_UserHeader,CID,IID,NULL);
    }
    fclose(probe);
    /*=============初始化相似用户链表==============================================*/
    userList = userHeader->next;
    counter = 0;

    int RecordCounterProbe = 0,length = 0;
    double  RS = 0.0 ,EPL = 0,HL = 0,MEANIL = 0,IL;
    float  rank ,DIL , kDegree;
    int k;
    //printf("Finish reading data file\n\n");

    while( userList != NULL){

        //printf("User:%d\n",userList->id);
        itemlist = userList->iList->next;
        time(&raw);
        while( itemlist != NULL){
            itemlist->self->own = TRUE;
            itemlist = itemlist->next;
        }
        ptrSim = userHeader->next;

        while( ptrSim != NULL){
            if( userList->id != ptrSim->id){
                DiffItemCounter = findDiffItem(userList->iList,ptrSim->iList);
                if(DiffItemCounter != 0){
                    simValue = sim(userList->iList,ptrSim->iList,DiffItemCounter);

                    //sUserListAddItem(userList->sList,ptrSim->id,simValue,ptrSim);
                   createSimList(SUList,ptrSim->id,simValue,ptrSim);
                }
            }
            ptrSim = ptrSim->next;
        }
        time(&rawCur);
        diftime = difftime(rawCur,raw);
        timeinfo = localtime(&rawCur);
        //printf("Current time is :%s",asctime(timeinfo));

        //printf("Finish SimUser  takes time:%lf\n",diftime);

        recBegin(userList,reclist);
        /*------------------------------------------------------------------------------Initialize L recommendation list*/
        /*rank = 0;
        dNode = dListUsed->next;
        flag = FALSE;

        while( (dNode != NULL) && (flag != TRUE)){
            rank++;
            iunHaveList(userList->dList,dNode->id,dNode->weight,dNode->degree);
            //printf("ID:%d\tWeight:%f\t",dNode->id,dNode->weight);
            if( rank == L){
                flag = TRUE;
            }
            dNode = dNode->next;
        }*/
        rank = 0;
        for(k = 0;rank < L;k++){
            if( reclist[k]->own != TRUE){
                rank++;
                iunHaveList(userList->dList,reclist[k]->id,reclist[k]->score,reclist[k]->degree);
            }
        }

        //printf("Finish  Recommendation L\n");
        //if( userList->dList->counter != L)printf("NOD:%fDegree:%d\n",rank,dListUsed->counter);

        /*------------------------------------------------------------------------------END Initialize L recommendation list*/


        /*==============================================================================Accuracy Test Begin==================*/


        flag = FALSE;
        un = probe_UserHeader->next;
        int i;
        while( (un != NULL) && (flag != TRUE) ){

            if( un->id == userList->id ){

                counter++;
                un->self  = userList;

        /*-------------------------------------------------------------------------------Test RankingScore-----------*/
                RecordCounterProbe += un->iList->counter;

                itemlist = un->iList->next;
                DIL      = 0;

                while( itemlist != NULL ){

                    /*dNode    = dListUsed->next;
                    length   = dListUsed->counter;
                    rank     = 0;

                    dou      = FALSE;

                    while( (dNode != NULL) && (dou != TRUE) ){
                        rank++;

                        if(itemlist->id == dNode->id){

                            RS  = RS + (rank / length);
                            /*-------------------------------------------------------------Caculate DIL For epl-------------
                            if( rank <= L){
                                DIL++;
                            }

                            dou = TRUE;
                        }
                        dNode = dNode->next;
                    }*/
                    length  = itemHeader->counter - userList->iList->counter;
                    dou   = FALSE;
                    rank  =  0;
                    for(i = 0;(i <= position) && (dou != TRUE);i++){
                        if( reclist[i]->own != TRUE){
                            rank++;
                            if( itemlist->id == reclist[i]->id){
                                RS  = RS + (rank / length);
                                if(rank <= L)DIL++;

                                dou  = TRUE;
                            }
                        }

                    }
                    itemlist = itemlist->next;
                }
                flag = TRUE;
                EPL = EPL + DIL / L;
/*------------------------------------------------------------------------------------------Test RankingScore  END-----------*/
            }

            un = un->next;
        }

        /*-----------------------------------------------------------------------------------END   Caculate DIL For epl-------------*/

        /*-----------------------------------------------------------------------------------Free Recommendation Node---------------*/

        /*dNode = dListUsed->next;
        dListUsed->next = NULL;

        while( dNode != NULL){
            td = dNode;
            dNode = dNode->next;

            dListUsed->next = td->next;

            td->next = dListFree->next;
            dListFree->next = td;
            dListFree->counter++;

            dListUsed->counter--;
        }*/
        initReclist(reclist);

        itemlist = userList->iList->next;
        while( itemlist != NULL){
            itemlist->self->own = FALSE;
            itemlist = itemlist->next;
        }

        /*-------------------------------------------------------------------------------------Accuracy Test END--------------------*/
        userList = userList->next;
    }
    RS = RS / RecordCounterProbe;

    EPL = EPL / counter;
    printf("Source EPL:%f\tOUD:%d\n",EPL,(itemHeader->counter * userHeader->counter) / RecordCounterProbe);
    EPL = EPL * (itemHeader->counter * userHeader->counter) / RecordCounterProbe;

    /*-----------------------------------------------------------------------------------------Diversity Test Begin-----------------*/
    un = probe_UserHeader->next;
    uNode *neighbor;
    counter = 0;
    RecordCounterProbe = 0;
    HL                 = 0;
    int mama = 0,kaka = 0,total = 0;
    float temp = 0;
    result = fopen(resultData,"a+");

    while( un->next != NULL ){
        kDegree  = 0;
        IL       = 0;
        if( un->self != NULL ){
            RecordCounterProbe++;
            neighbor = un->next;
            dNode = un->self->dList->next;

            while( dNode != NULL){
                kDegree  = dNode->degree;
                IL += log2f( ( userHeader->counter / kDegree) );
                dNode = dNode->next;
            }
            IL = IL / L;
            MEANIL += IL;
            mama = 0;
            //printf("Test\t");
            while( neighbor != NULL){
                if( neighbor->self != NULL){
                    HL += (1 - Get_QIJL(un->self->dList,neighbor->self->dList) / L);
                    if( Get_QIJL(un->self->dList,neighbor->self->dList) > 10){
                        mama++;
                        kaka++;

                    }

                    counter++;
                    //printf("un----HL:%f\n",(1 - Get_QIJL(un->self->dList,neighbor->self->dList) / L));
                }

                neighbor = neighbor->next;
            }

/*        fprintf(result,"UN:%d\tmama:%d\tHL--UN:%f\tTotal:%d\n",un->id,mama,(HL - temp),(counter - total));
        fprintf(result,"-----------------------\n");
        temp = HL;*/
        total = counter;
        }

        un = un->next;
    }
    fprintf(result,"kaka:%d\n",kaka);
    fprintf(result,"-----------------------\n");
    printf("Result---HL:%f\tCounter:%d\tmama:%d\t",HL,counter,kaka);
    HL = HL / counter;

    MEANIL = MEANIL / RecordCounterProbe;

    /*-----------------------------------------------------------------------------------------Diversity Test END-----------------*/

    printf("\nRS:%f\tEPL:%f\tHL:%f\tMEANIL:%f\n",RS,EPL,HL,MEANIL);

    fprintf(result,"RS:%1.4f\tEPL:%f\tHL:%f\tMEANIL:%f\n",RS,EPL,HL,MEANIL);
    fclose(result);
    printf("Finish!\n");


    /*================OLD TEST=======================*/
    /*printf("Begin Testing\t\tCounter:%d\n\n",counter);

    uNode  *currentUser;
    DiffNode  *bufferNode;
    int RecordCounterProbe = 0,length = 0;
    float  RS = 0.0,rank = 0.0;

    result = fopen(resultData,"w+");
    //counter = 0;
    while( fscanf(probe,"%d%d%d",&CID,&IID,&SCORE) != EOF ){

        currentUser = userHeader->next;
        while(currentUser != NULL){

            bufferNode = currentUser->dList->next;

            if( currentUser->id == CID){
                RecordCounterProbe++;
                length = currentUser->dList->counter;
                //printf("Current User ID:%d\tRecList length:%d\t",CID,length);
                rank = 0;
                while( bufferNode != NULL){
                    rank++;

                    if( bufferNode->id == IID){
                        fprintf(result,"%1.5f\n",(rank / length));
                        RS = RS + (rank / length);
                        //printf("Rank:%f\tRS:%f\n",rank,RS);
                        break;
                    }
                    bufferNode = bufferNode->next;
                }
                break;
            }
            currentUser = currentUser->next;
        }
    }*/


    //getchar();
    return 0;
}


void swap(ItemNode **a,ItemNode **b){
    ItemNode *tem;

    tem = *b;
    *b  = *a;
    *a  = tem;
}

void initReclist(ItemNode **list){
    int i;

    for(i = 0;i <= position;i++){
        //printf("%f\t",list[i]->score);
        list[i]->score = 0;
    }

}
int division(ItemNode **list,int left,int right,int pivot_index){
    float pivot = list[pivot_index]->score;
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

void quickSort(ItemNode **list,int left,int right){
    int index,pivot_index = (left + right) / 2;

    if( left < right){
        index = division(list,left,right,pivot_index);
        quickSort(list,left,index - 1);
        quickSort(list,index + 1,right);
    }
}
void recBegin(uNode *list,ItemNode **reclist){
    /*===========开始寻找每个用户的推荐物品并构建推荐链表============================*/
    //userList = userHeader->next;
    //counter = 0;
    SimNode *tem;

    //while(userList != NULL){

        //counter++;
        slist = SUList->next;
        //SUList->next = NULL;
        time(&raw);
        while(slist != NULL){
            itemlist = slist->original->iList->next;
            while(itemlist != NULL){
                itemlist->self->score += slist->sim;
                itemlist = itemlist->next;
            }
            tem = slist;

            slist = slist->next;
            //free(tem);
            tem->id = 0;
            tem->original = NULL;
            tem->sim   = 0;
            SUList->next = tem->next;

            tem->next = SUFreeList->next;

            SUFreeList->next = tem;
            SUFreeList->counter++;
            SUList->counter--;
            //printf("SUFreeList length:%d\n",SUFreeList->counter);
            //getchar();

        }
        time(&rawCur);
        diftime = difftime(rawCur,raw);
        //timeinfo = localtime(&rawCur);
        //printf("Current time is :%s",asctime(timeinfo));
        //printf("initial item score takes time\t%lfs\n",diftime);

        if( SUList->counter != 0) printf("SUlist NotClean\n");
        time(&raw);
        /*itemlist = itemHeader->next;
        while(itemlist != NULL){
            //flag = searchItemList(userList->iList,itemlist->id);
            flag = itemlist->own;

            if( flag != TRUE) {

                //iunHaveList(userList->dList,itemlist->id,itemlist->score);
                createRecItemList(dListUsed,itemlist->id,itemlist->degree,itemlist->score);
            }
            itemlist->score = 0;
            itemlist = itemlist->next;
        }*/
        quickSort(reclist,0,position);

        time(&rawCur);
        diftime = difftime(rawCur,raw);
        //timeinfo = localtime(&rawCur);
        //printf("Current time is :%s",asctime(timeinfo));
        //printf("initial recmmendation list takes  %lfs\n",diftime);

    /*==============Free RecNode Testing============*/
    /*dNode = dListUsed->next;
    dListUsed->next = NULL;
    while( dNode != NULL){
        td = dNode;
        dNode = dNode->next;

        dListUsed->next = td->next;

        td->next = dListFree->next;
        dListFree->next = td;
        dListFree->counter++;

        dListUsed->counter--;
    }*/

    /*    userList = userList->next;
    }*/
}

void initFreeSimList(SimUserList *header,int len){
    SimNode *list;
    int i;

    header->counter = len;

    list = malloc(sizeof(SimNode));
    list->id = 0;
    list->next = NULL;
    list->original = NULL;
    list->sim   = 0;

    header->next = list;

    for(i = 1;i < len;i++){
        list->next = malloc(sizeof(SimNode));
        list = list->next;
        list->id = 0;
        list->next = NULL;
        list->original = NULL;
        list->sim   = 0;
    }

}

void createSimList(SimUserList *header,int id,float sim,uNode *self){
        SimNode *list;

        if( SUFreeList->counter != 0){

            if( header->counter == 0){
                    header->counter++;

                    header->next = SUFreeList->next;
                    list = header->next;
                    SUFreeList->next = list->next;
                    list->next = NULL;
                    SUFreeList->counter--;


                    list->id = id;
                    list->sim = sim;
                    list->original = self;

            }
            else{
                list = header->next;
                while( list->next != NULL){
                    list = list->next;
                }
                list->next = SUFreeList->next;

                list = list->next;

                SUFreeList->next = list->next;
                list->next = NULL;
                SUFreeList->counter--;


                list->id = id;
                list->sim = sim;
                list->original = self;

                header->counter++;
                //printf("Test\n");
            }
        }

}

void initFreeDiffList(DiffList  *header,int len){
    DiffNode *list;
    int i;

    header->counter = len;

    list = malloc(sizeof(DiffNode));
    list->id = 0;
    list->next = NULL;

    list->weight   = 0;

    header->next = list;

    for(i = 1;i < len;i++){
        list->next = malloc(sizeof(DiffNode));
        list = list->next;
        list->id = 0;
        list->next = NULL;

        list->weight   = 0;
    }
}

void createRecItemList(DiffList *header,int id,int degree,float weight){

    char found = FALSE;
    int step = 0;
    DiffNode *backward,*forward,*middle;

    if( dListFree->counter != 0){
        if( header->counter == 0){
            header->counter++;
            header->next = dListFree->next;

            forward = header->next;

            dListFree->next = forward->next;
            dListFree->counter--;

            forward->id = id;
            forward->weight = weight;
            forward->degree = degree;
            forward->next = NULL;
        }
        else{
            forward = header->next;
            backward = NULL;
            middle = NULL;

            while( (forward != NULL) && (found != TRUE) ){
            if(forward->id != id){
                if( forward->weight > weight){

                    if( forward->next == NULL){


                            forward->next = dListFree->next;
                            backward  = forward->next;

                            dListFree->next = backward->next;
                            dListFree->counter--;

                            backward->id = id;
                            backward->next = NULL;
                            backward->weight = weight;
                            backward->degree = degree;

                            found = TRUE;
                            header->counter++;

                    }
                }
                else if( forward->weight <= weight){

                                    if( step == 0){

                                        middle = dListFree->next;

                                        dListFree->next = middle->next;
                                        dListFree->counter--;

                                        //printf("Step 0\n");
                                        middle->next = header->next;
                                        header->next = middle;
                                        middle->id = id;
                                        middle->weight = weight;
                                        middle->degree = degree;

                                        header->counter++;
                                        found = TRUE;
                                    }
                                    else if(step != 0){

                                        middle = dListFree->next;
                                        dListFree->next = middle->next;
                                        dListFree->counter--;

                                        backward->next = middle;
                                        middle->next  = forward;
                                        //printf("Step  No  Zero\t\t");
                                        //getchar();
                                        middle->id = id;
                                        middle->weight = weight;
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
    }
}
/*itemlist = itemHeader->next;
        while( itemlist != NULL){
            flag = searchItemList(userList->iList,itemlist->id);
            sum          = 0.0;
            DFWEIGHT     = 0.0;
            if( flag != TRUE){
                 ptrSimSteady = userList->sList->next;

                 //printf("found item %d sim list length %d\n",itemlist->id,userList->sList->counter);
                 while( ptrSimSteady != NULL){

                    sum += ptrSimSteady->sim;
                    DFWEIGHT += ( ptrSimSteady->sim * getScore(ptrSimSteady->original->iList,itemlist->id) );

                    ptrSimSteady = ptrSimSteady->next;
                 }
                 DFWEIGHT = DFWEIGHT / sum ;
                 //printf("DFWEIGHT:%f\n",DFWEIGHT);
                 //getchar();
                 iunHaveList(userList->dList,itemlist->id,DFWEIGHT);
            }
            itemlist = itemlist->next;
        }*/
