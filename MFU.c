#include "Page.h"

page *MostFrequentlyUsed(LISTOFPAGES* pageList) {

    page* mfu_page = pageList->head;
    int maxCounter = mfu_page->counter;

    for (page* curr_page = pageList->head->next; curr_page != NULL; curr_page = curr_page->next) {
        if (curr_page->counter > maxCounter) {
            mfu_page = curr_page;
            maxCounter = curr_page->counter;
        }
    }


    return mfu_page;
}

