#include "Page.h"

//Random Page Replacement Algorithm
page *RandomPageReplacement(LISTOFPAGES* ListOfPages) 
{
   // pointer to current page in the list, starting at the head
   page* currentPage = ListOfPages->head;

   // pointer will point to our random page after traversal
   page* randomPage = NULL;

   // get index of some random page in the list
   int random_page_index = rand() % TOTAL_PAGE;

   int index = 0;
   // traverse to the randomly selected page
   while (currentPage != NULL) {
      // if we are at our random page, stop
      if (index == random_page_index) {
         randomPage = currentPage;
         break;
      }
      // otherwise, continue traversal
      index = index + 1;
      currentPage = currentPage->next;
   }
    
   return randomPage;
}
