#include "Page.h"

//LFU algorithm
page *LeastFrequentlyUsed(LISTOFPAGES* ListOfPages) 
{
    page* currentPage = ListOfPages->head;
    page* lfuPage = ListOfPages->head;
    int min = currentPage->counter;

    while(currentPage) 
    {
        if(currentPage->counter < min)
        {
            lfuPage = currentPage;
            min = currentPage->counter;
        }

        currentPage = currentPage->next;
    }

    if (DEBUG==1) 
    {
        printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", lfuPage->pid, lfuPage->counter, lfuPage->loadTime);
    }
    
    return lfuPage;
}
