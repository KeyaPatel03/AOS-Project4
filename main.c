#include "Page.h"

float avgProcessesStarted = 0;
float avgPagesSwappedIn = 0;
float avgTotalPagesReferenced = 0;
float avgPagesAlreadyInMemory = 0;

void DisplayMemoryMap(LISTOFPAGES* pl) {
    page* it = pl->head;
    while (it) {
        if (it->pid == -1)
            printf(".");
        else
            printf("%c", 'A' + (it->pid % 26));  // A-Z mapping
        it = it->next;
    }
    printf("\n");
}

int main(int arg1, char* arg2[]) 
{
    int timeStamp = 0;  //Time stamp simulator
    int *pageCountOPT = malloc(sizeof(int)*4); //Paging options
    char message[100];
    pageCountOPT[0] = 5;
    pageCountOPT[1] = 11;
    pageCountOPT[2] = 17;
    pageCountOPT[3] = 31;


    //Pointer to the Page
    page *pagePointer;

    page *(*algoFunct)(LISTOFPAGES*);
    if (arg1 != 2)
    {
        printf("Page replacement algorithm is missing in command line. Allowed algorithms are: FIFO, LRU, LFU, MFU or Random.\n");
        return -1;
    }
    
    //Determining the paging algorithm to be processed.
    if(strcmp(arg2[1], "FIFO") == 0)
    {
        algoFunct = FirstInFirstOut;
    }
    else if(strcmp(arg2[1], "LRU") == 0)
    {
        algoFunct = LeastRecentlyUsed;
    }
    else if(strcmp(arg2[1], "LFU") == 0)
    {
        algoFunct = LeastFrequentlyUsed;
    }
    else if(strcmp(arg2[1], "MFU") == 0)
    {
        algoFunct = MostFrequentlyUsed;
    }
    else if(strcmp(arg2[1], "Random") == 0)
    {
        algoFunct = RandomPageReplacement;
    }
    else 
    {
        printf("####################################################################\n");
        printf("Available algorithms to run for simulation : FIFO, LRU, LFU, MFU or Random.\n");
        printf("####################################################################\n");
        return -1;
    }

    int pagesSwappedIn = 0;
    int totalPagesReferenced = 0;
    int pagesAlreadyInMemory = 0;
    int processesStarted = 0;
    srand(0);
    int i;
    for(i = 0; i < SIMULATION_COUNT; i++) 
    {
        printf("<<<<<<<======= Running simulator   %d ========>>>>>>>\n", i+1);
        LISTOFPAGES pl;
        InitPageList(&pl);
        process Q[TOTAL_PROCESS];
        int i;
        for(i=0;i<TOTAL_PROCESS;i++) 
        {
            Q[i].pid = i;
            Q[i].pageCounter = pageCountOPT[rand() % 4];
            snprintf(Q[i].name, sizeof(Q[i].name), "%c%d", 'A' + (i % 26), i / 26);
            Q[i].arrivalTime = rand() % 60;
            Q[i].serviceDuration =  1 + rand() % PROCESS_DURATION; //Maximum process duration
            Q[i].originalDuration =  Q[i].serviceDuration;
            Q[i].pageReference = 0; //All processes begin with page 0
        }

        qsort(Q,TOTAL_PROCESS,sizeof(process),CompArrTime);

        int index = 0; //Index to the start of process queue
        for(timeStamp = 0; timeStamp < TOTAL_DURATION; timeStamp++) 
        {
            //Looking for new process at start of every second
            while(index < TOTAL_PROCESS && Q[index].arrivalTime <= timeStamp) 
            {
                //To check atleast four pages
                if(FindFreePages(&pl,4)) 
                {
                    //If it is present then bring it in the memory
                    page* p = PageFree(&pl);
                    p->pid = Q[index].pid;
                    p->pageNumber = Q[index].pageReference;
                    p->timeBought = 1.0*timeStamp;
                    p->counter = 1;
                    p->loadTime = timeStamp;
                    // printf("timestamp: %2d.0 seconds, process %s (id: %d), page count: %2d, duration: %d, status: exiting\n",
                    // timeStamp + 1, Q[i].name, Q[i].pid, Q[i].pageCounter, Q[i].originalDuration);

                    //DisplayStatus(p, timeStamp, "need first page to run");
                    index++;

                    pagesSwappedIn++;
                    totalPagesReferenced++;
                    processesStarted++;
                } 
                else {
                    break;      
                }
            }

            printf("##########################################################################################\n");

            int i; 
            for(i=0;i<10;i++) 
            {
                int j;
                for(j=0;j<index;j++) {
                    if(Q[j].serviceDuration > 0)
                    {
                        Q[j].pageReference = GenNextPageNumber(Q[j].pageReference,Q[j].pageCounter);
                        if(PagesInMemory(&pl,Q[j].pid,Q[j].pageReference)) 
                        {
                            pagePointer = PageIDFree(&pl,Q[j].pid,Q[j].pageReference);
                            if (pagePointer == NULL)
                            {
                                printf("There is an error, got null : pid %d page:: %d\n", Q[j].pid, Q[j].pageReference);
                                return -1;
                            }
                            pagePointer->counter++;
                            pagePointer->loadTime = timeStamp + (0.1*i);
                            
                            DisplayStatus(pagePointer, pagePointer->loadTime, "page already in memory");
                            pagesAlreadyInMemory++;
                            totalPagesReferenced++;
                            continue;
                        }

                        page* pge = PageFree(&pl);
                        // if there are no free pages:
                        if (!pge) {
                            page* toBeEvicted = algoFunct(&pl);
                            printf("At time %.1f: Page replacement needed → Process %s (pid: %d), Page %d will be evicted\n",
                                timeStamp + (0.1 * i), Q[toBeEvicted->pid].name, toBeEvicted->pid, toBeEvicted->pageNumber);
                        
                            pge = toBeEvicted;
                            sprintf(message, "evicted page %d, pid %d", pge->pageNumber, pge->pid);
                        }
                        else {

                            sprintf(message, "loaded into free page");
                        }


                        pge->pid = Q[j].pid;
                        pge->pageNumber = Q[j].pageReference;
                        pge->timeBought = timeStamp + (0.1*i);
                        pge->loadTime = pge->timeBought;
                        pge->counter = 0;

                        DisplayStatus(pge, pge->timeBought, message);   

                        pagesSwappedIn++;   
                        totalPagesReferenced++;
                    }
                }
            }

            int j;
            printf("##########################################################################################\n");
            for(j=0;j<index;j++) if(Q[j].serviceDuration > 0) 
            {
                Q[j].serviceDuration--;
                if(Q[j].serviceDuration == 0) 
                {
                    /*printf("####################################################################\n");
                    printf("Process id %d is done. Memory is getting free .... \n",Q[j].pid);
                    printf("####################################################################\n");*/
                    printf("timestamp: %2d.0 seconds, process %s (id: %d), page count: %2d, duration: %d, status: exiting\n", timeStamp + 1, Q[j].name, Q[j].pid, Q[j].pageCounter, Q[j].originalDuration);
                    FreeMemory(&pl,Q[j].pid);

                    // printf("timestamp: %2d.0 seconds, process id: %3d, page count: %2d, duration: %d, status: exiting\n", timeStamp+1, Q[j].pid, Q[j].pageCounter, Q[j].originalDuration);
                    
                }
            }
            printf("##########################################################################################\n");
            printf("Memory Map at time %.1f seconds:\n", timeStamp * 1.0);
            DisplayMemoryMap(&pl);

            usleep(900);

            // if (totalPagesReferenced > 100)
            //     break;
        }
    
        float runHitRatio = (pagesAlreadyInMemory * 1.0) / totalPagesReferenced;
        float runMissRatio = (pagesSwappedIn * 1.0) / totalPagesReferenced;

        printf("\n--- Run #%d Metrics ---\n", i+1);
        printf("Processes Started: %d\n", processesStarted);
        printf("Pages Swapped-In: %d\n", pagesSwappedIn);
        printf("Total Page References: %d\n", totalPagesReferenced);
        printf("Hit Ratio: %.2f%%\n", runHitRatio * 100);
        printf("Miss Ratio: %.2f%%\n", runMissRatio * 100);
        

        // Accumulate for final average
        avgProcessesStarted += processesStarted;
        avgPagesSwappedIn += pagesSwappedIn;
        avgTotalPagesReferenced += totalPagesReferenced;
        avgPagesAlreadyInMemory += pagesAlreadyInMemory;

        // Reset for next run
        pagesSwappedIn = 0;
        pagesAlreadyInMemory = 0;
        totalPagesReferenced = 0;
        processesStarted = 0;
    }

// Final averages
printf("\n=== Final Averages ===\n");
printf("Average Processes Started: %.2f of %d\n", avgProcessesStarted / SIMULATION_COUNT, TOTAL_PROCESS);
printf("Average Pages Swapped-In: %.2f\n", avgPagesSwappedIn / SIMULATION_COUNT);
float finalHitRatio = (float)avgPagesAlreadyInMemory / avgTotalPagesReferenced;
float finalMissRatio = 1.0 - finalHitRatio;
printf("Average Hit Ratio: %.2f%%\n", finalHitRatio * 100);
printf("Average Miss Ratio: %.2f%%\n", finalMissRatio * 100);

}