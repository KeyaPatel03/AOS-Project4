#include "Page.h"

static void printEvictedPage(const page* evictedPage) {
    if (DEBUG == 1) {
        printf("EVICTED: p[%03d] c:%02d l:%02f\n",
               evictedPage->pid, evictedPage->counter, evictedPage->loadTime);
    }
}

static void resetPageData(page* evictedPage) {
    evictedPage->pid = -1;
    evictedPage->pageNumber = -1;
}

page *MostFrequentlyUsed(LISTOFPAGES* pageList) {
    if (pageList == NULL || pageList->head == NULL) {
        return;
    }

    page* mfu_page = pageList->head;
    int maxCounter = mfu_page->counter;

    for (page* curr_page = pageList->head->next; curr_page != NULL; curr_page = curr_page->next) {
        if (curr_page->counter > maxCounter) {
            mfu_page = curr_page;
            maxCounter = curr_page->counter;
        }
    }

    printEvictedPage(mfu_page);
    resetPageData(mfu_page);

    return mfu_page;
}

