#include "structs.h"
#include <math.h>
/*-------------------------Defination of Variables-----------------*/
char probeData[] = "./datas/probe.data",trainData[] = "./datas/train.data",resultData[] = "./datas/results.data";

FILE *probe,*train,*result;

//initialize lists for User and Items
uList *ulist,*probe_header;

uNode *train_User,*probe_User;

ItemList *ilist;

ItemNode *itemlist,*bufferNode,*CItem;

char flag = FALSE,dou;

unsigned int counter = 0;
//define variables for  Custom  Item  Score
int     CID,IID,SCORE,
        RecordCounterProbe = 0,
        length = 0,k,p1,p2,
        position = -1,index_position = 0,
        recLength = 0,
        checkRecord = 0;
double  RS = 0.0 ,EPL = 0,HL = 0,MEANIL = 0,IL;
float   rank = 0.0,DIL = 0,kDegree = 0;

/*----------End of Variabless Defination--------------*/

/*--------------------------------------------------------------------Quick Sort------------------*/
int main()
{
    ItemNode *reclist[232657] = {NULL};
    train = fopen(trainData,"r");

    ulist = malloc(sizeof(uList));
    ulist->counter = 0;
    ulist->next = NULL;

    probe_header = malloc(sizeof(uList));
    probe_header->counter = 0;
    probe_header->next = NULL;

    ilist  = malloc(sizeof(ItemList));
    ilist->counter = 0;
    ilist->next = NULL;
    result = fopen(resultData,"a+");

    //初始化用户和商品列表
    while(fscanf(train,"%d%d%d",&CID,&IID,&SCORE) != EOF){
        CItem = IListInsert(ilist,IID,&position,reclist);
        UListInsert(ulist,CID,IID,CItem);
        counter++;
        if( counter > 1000000) printf("%d\n",counter);
    }
    counter = 0;


    fclose(train);
/*----------------------------------------------------------------------------Initial Probe User List-----------*/
    probe = fopen(probeData,"r");
    while(fscanf(probe,"%d%d%d",&CID,&IID,&SCORE) != EOF ){
        UListInsert(probe_header,CID,IID,NULL);
    }
    //index_position = get_nozero_position(reclist,0,position);

    printf("Finish reading data\n");
    printf("TotalUser:%d\tTotalItems:%d\n",ulist->counter,ilist->counter);

    //对形成的推荐列表进行测试

    train_User = ulist->next;

    while(train_User != NULL){
        //printf("User:%d\n",train_User->id);
        itemlist = train_User->uilist->next;

        while(itemlist != NULL){
            itemlist->self->own = TRUE;
            if(itemlist->id != itemlist->self->id) printf("error\n");
            itemlist = itemlist->next;
        }
        initial_rec(reclist,ulist,position);

        conform_rec(ilist,ulist);

        index_position = get_nozero_position(reclist,0,position);
        quickSort(reclist,0,index_position);

        k  = 0;
        recLength = 0;
        /*--------------------------------------------------------------Initial Top L reclist-----*/
        while(recLength < L){
            if(reclist[k]->own != TRUE){
                RECListInsert(train_User->rlist,reclist[k]->id,reclist[k]->degree,reclist[k]->score);
                recLength++;
            }
            k++;
        }


        /*---------------------------------------------------------------End Top L Reclist--------*/
        probe_User = probe_header->next;
        flag  = FALSE;

        while( (probe_User != NULL) && (flag != TRUE) ){

            if(probe_User->id == train_User->id){
                probe_User->self  =  train_User;

                itemlist = probe_User->uilist->next;
                //RecordCounterProbe += probe_User->uilist->counter;
                counter++;
                DIL  = 0;
                while(itemlist != NULL){

                    length = ilist->counter - train_User->uilist->counter;
                    dou   = FALSE;
                    rank = 0;

                    for(k = 0;(k <= position)&&(dou != TRUE) ;k++){
                        if( reclist[k]->own != TRUE ){
                            rank++;
                            if(reclist[k]->id == itemlist->id){
                                RecordCounterProbe++;

                                p1 = k;
                                p2 = k;

                                while( (p1 != 0)&&(reclist[p1]->score == reclist[p1 - 1]->score) ) p1--;
                                while( (p2 != position)&&(reclist[p2]->score == reclist[p2 + 1]->score) ) p2++;

                                if( rank <= L)DIL++;

                                if(rank > ((p1 + p2) / 2 )){

                                    rank = (p1 + p2) / 2;
                                }
                                //else if(rank < ((p1 + p2) / 2 )) less++;

                                RS += (rank / length);
                                dou = TRUE;
                            }
                        }
                    }
                    itemlist = itemlist->next;
                }
                EPL  = EPL + DIL / L;
                flag = TRUE;
            }
            probe_User = probe_User->next;
        }


        /*--------------------------------------------------------------Initial itemlist----------*/
        itemlist = train_User->uilist->next;
        while(itemlist != NULL){
            itemlist->self->own = FALSE;
            itemlist = itemlist->next;
        }

        initial_rec(reclist,NULL,position);
        train_User = train_User->next;
    }

    RS = RS / RecordCounterProbe;
    EPL = EPL / counter;
    EPL = (EPL * ulist->counter  / RecordCounterProbe) * ilist->counter;
    //printf("Test finish\n");
    fclose(probe);

    /*-----------------------------------------------------------------------------------------Diversity Test Begin-----------------*/
    probe_User = probe_header->next;
    uNode *neighbor;
    counter = 0;
    RecordCounterProbe = 0;
    HL                 = 0;
    int total = 0;
    while( probe_User->next != NULL ){
        kDegree  = 0;
        IL       = 0;
        if( probe_User->self != NULL ){
            RecordCounterProbe++;
            neighbor = probe_User->next;

            itemlist = probe_User->self->rlist->next;

            while( itemlist != NULL){
                kDegree  = itemlist->degree;
                IL += log2f( ( ulist->counter / kDegree) );
                itemlist = itemlist->next;
            }

            IL = IL / L;
            MEANIL += IL;
            while( neighbor != NULL){

                if( neighbor->self != NULL){

                    HL += (1 - Get_QIJL(probe_User->self->rlist,neighbor->self->rlist) / L);

                    counter++;
                }
                neighbor = neighbor->next;
            }

            total = counter;
        }

        probe_User = probe_User->next;
    }

    HL = HL / counter;

    MEANIL = MEANIL / RecordCounterProbe;

    /*-----------------------------------------------------------------------------------------Diversity Test END-----------------*/
    printf("\nRS:%f\tEPL:%f\tHL:%f\tMEANIL:%f\n",RS,EPL,HL,MEANIL);
    fprintf(result,"HeatS-RS:%1.4f\tEPL:%f\tHL:%f\tMEANIL:%f\n",RS,EPL,HL,MEANIL);
    fclose(result);
    return 0;
}
