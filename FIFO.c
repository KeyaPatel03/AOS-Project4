#include "Page.h"

//FCFS algorithm
page *FirstInFirstOut(LISTOFPAGES* ListOfPages) 
{
    page* currentPage = ListOfPages->head;
    page* fcfsPage = ListOfPages->head;
    
    while(currentPage) 
    {
        if(currentPage->timeBought < fcfsPage->timeBought) 
        {
            fcfsPage = currentPage;
        }

        currentPage = currentPage->next;
    }

    if (DEBUG==1) 
    {
        printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", fcfsPage->pid, fcfsPage->counter, fcfsPage->loadTime);
    }

    return fcfsPage;
}
