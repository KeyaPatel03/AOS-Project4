#include "Page.h"

//Initializing page list
void InitPageList(LISTOFPAGES* pl) 
{
    pl->head = malloc(sizeof(page));
    page* it = pl->head;
    int i;
    for(i=0;i<TOTAL_PAGE;i++) 
    {
        it->pid = -1; it->pageNumber = -1;
        it->next = NULL;
        
        if(i < 99) 
        {
            it->next = malloc(sizeof(page));
            it = it->next;
        }
    }
}

//Displaying all pages
void DisplayAllPage(LISTOFPAGES* pl) 
{
    page* it = pl->head;
    int cnt = 0;
    while(it) 
    {
        printf(it->pid > 0 ? "|*| p[%03d] c:%02d l:%02f |*|" : "|*|",it->pid, it->counter, it->loadTime);
        cnt++;
        if((cnt % 10) == 0) 
            printf("\n");
        it = it->next;
    }

    printf("\n");
}

//Pages which are free
int FindFreePages(LISTOFPAGES* pl,int counter) 
{
    page* it = pl->head;
    while(it) 
    {
        if(it->pid == -1) 
        { // page not being used by any process;
            counter--;
        }

        if(!counter) 
            return 1;
        it = it->next;
    }

    return 0;
}

//Pages in memory
int PagesInMemory(LISTOFPAGES* pl,int pid,int pageNumber) 
{
    page* it = pl->head;
    while(it) 
    {
        if(it->pid == pid && it->pageNumber == pageNumber) 
            return 1;
        it = it->next;
    }

    return 0;
}

page* PageFree(LISTOFPAGES* pl) 
{
    page* it = pl->head;
    while(it) 
    {
        if(it->pid == -1) 
            return it;
        it = it->next;
    }

    return NULL;
}
//Memory getting free
int FreeMemory(LISTOFPAGES* pl,int pid) 
{
    page* it = pl->head;
    int pages_freed = 0;
    while(it) 
    {
        if(it->pid == pid) 
        {
            it->pid = -1;
            it->pageNumber = -1;
            pages_freed++;
        }

        it = it->next;
    }

    return pages_freed;
}
//Generating next page number
int GenNextPageNumber(int curr_page_no,int max_page_size) 
{
    int x = rand() % 10;
    // 0 <= x < 7
    // 70% chance
    if(x < 7) 
    {
        // delta = -1, 0, or 1
        int delta = ((rand() % 3) - 1);
        x = curr_page_no + delta;
    }
    // 7 <= x < 10
    // 30% chance
    else 
    {
        x = rand() % max_page_size;
        while(abs(x - curr_page_no) <= 1) 
            x = rand()% max_page_size;
    }
    
    if (x < 0)
        x = 0;

    return x % max_page_size;
}

//Page id which are free
page* PageIDFree(LISTOFPAGES* pl,int pid,int pageNumber) 
{
    page* it = pl->head;
    while(it) 
    {
        if(it->pid == pid && it->pageNumber == pageNumber) 
            return it;
        it = it->next;
    }

    return NULL;
}

//Comparing arrival time
int CompArrTime(const void* a,const void* b) 
{
    return ((process*)a)->arrivalTime - ((process*)b)->arrivalTime;
}

// display page status
void DisplayStatus(page *p, float timestamp, char *status) {
    printf("timestamp: %2.1f seconds, process id: %3d, page: %3d, status: %s\n", timestamp, p->pid, p->pageNumber, status);
}
