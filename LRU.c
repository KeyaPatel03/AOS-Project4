#include "Page.h"

//LRU algorithm
page *LeastRecentlyUsed(LISTOFPAGES* ListOfPages) 
{
    page* currentPage = ListOfPages->head;
    page* lruPage = ListOfPages->head;
    int lru = currentPage->loadTime;

    while(currentPage) 
    {
        if(currentPage->loadTime < lru)
        {
            lruPage = currentPage;
            lru = currentPage->loadTime;
        }

        currentPage = currentPage->next;
    }

    if (DEBUG==1) 
    {
        printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", lruPage->pid, lruPage->counter, lruPage->loadTime);
    }
    
    return lruPage;
}
