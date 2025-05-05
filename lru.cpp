    #include <iostream>
    #include <queue>
    #include <vector>
    #include <string>
    #include <cstdlib>
    #include <ctime>
    #include <iomanip>
    #include <map>
    #include <set>
    #include <algorithm>
    #include <list>

    using namespace std;

    struct Page {
        string processName;
        int pageNumber;
    };

    struct Process {
        string name;
        int sizeInPages;
        int arrivalTime;
        int duration;
        int remainingTime;
        int currentPage = 0;
        int nextReferenceTime = 0;
    };

    const int MEMORY_SIZE = 100; // 100 MB
    const int PAGE_SIZE = 1;     // 1 MB
    const int PAGE_FRAME_COUNT = MEMORY_SIZE / PAGE_SIZE;
    const int SIM_TIME = 60;     // 60 seconds
    const int REFERENCE_INTERVAL = 100; // ms
    const int MAX_REFERENCES_LOG = 100;

    vector<Page> memory;
    queue<int> freePageList;
    list<Page> lruList; // For LRU
    vector<Process> jobQueue;
    vector<Process> runningProcesses;

    int currentTimeMs = 0;
    int hits = 0, misses = 0;
    int referenceLogCount = 0;
    int processesSwappedIn = 0;

    enum Algorithm { LRU };
    Algorithm currentAlgo = LRU;

    int getRandomPageReference(Process& proc, int currentPage) {
        int r = rand() % 10;
        if (r < 7) {
            int delta[] = {-1, 0, 1};
            int di = delta[rand() % 3];
            return (currentPage + di + proc.sizeInPages) % proc.sizeInPages;
        } else {
            int j;
            do {
                j = rand() % proc.sizeInPages;
            } while (abs(j - currentPage) <= 1);
            return j;
        }
    }

    bool isPageInMemory(const string& proc, int pageNum) {
        for (const Page& p : memory) {
            if (p.processName == proc && p.pageNumber == pageNum)
                return true;
        }
        return false;
    }

    void updateLRUList(const Page& p) {
        lruList.remove_if([&](const Page& pg) {
            return pg.processName == p.processName && pg.pageNumber == p.pageNumber;
        });
        lruList.push_back(p);
    }

    Page evictPageLRU() {
        Page victim = lruList.front();
        lruList.pop_front();
        for (int i = 0; i < PAGE_FRAME_COUNT; ++i) {
            if (memory[i].processName == victim.processName && memory[i].pageNumber == victim.pageNumber) {
                memory[i] = {"", -1};
                freePageList.push(i);
                cout << fixed << setprecision(2) << currentTimeMs / 1000.0
                    << "s: Evicted Page " << victim.pageNumber << " of Process " << victim.processName << endl;
                break;
            }
        }
        return victim;
    }

    void pageIn(Process& proc, int pageNum, Algorithm algo) {
        if (!isPageInMemory(proc.name, pageNum)) {
            misses++;
            Page evicted = {"", -1};
            if (freePageList.empty()) {
                evicted = evictPageLRU();
            }
            int frame = freePageList.front();
            freePageList.pop();
            memory[frame] = {proc.name, pageNum};
            updateLRUList({proc.name, pageNum});

            if (referenceLogCount < MAX_REFERENCES_LOG) {
                cout << fixed << setprecision(2) << currentTimeMs / 1000.0 << "s: "
                    << proc.name << " referenced Page " << pageNum << " -> MISS";
                if (evicted.pageNumber != -1) {
                    cout << ", Evicted: " << evicted.processName << "/" << evicted.pageNumber;
                }
                cout << endl;
                referenceLogCount++;
            }
        } else {
            hits++;
            updateLRUList({proc.name, pageNum});
            if (referenceLogCount < MAX_REFERENCES_LOG) {
                cout << fixed << setprecision(2) << currentTimeMs / 1000.0 << "s: "
                    << proc.name << " referenced Page " << pageNum << " -> HIT" << endl;
                referenceLogCount++;
            }
        }
    }

    void initializeJobs() {
        jobQueue.clear();
        vector<int> sizes = {5, 11, 17, 31};
        vector<int> durations = {1, 2, 3, 4, 5};
        for (int i = 0; i < 150; ++i) {
            Process p;
            p.name = string(1, 'A' + (i % 26)) + to_string(i / 26);
            p.sizeInPages = sizes[rand() % sizes.size()];
            p.duration = durations[rand() % durations.size()];
            p.arrivalTime = rand() % SIM_TIME;
            p.remainingTime = p.duration * 1000;
            jobQueue.push_back(p);
        }
        sort(jobQueue.begin(), jobQueue.end(), [](const Process& a, const Process& b) {
            return a.arrivalTime < b.arrivalTime;
        });
    }

    void printMemoryMap() {
        for (const Page& p : memory) {
            if (p.pageNumber == -1) cout << ".";
            else cout << p.processName[0];
        }
        cout << endl;
    }

    void runSimulation(Algorithm algo) {
        memory.assign(PAGE_FRAME_COUNT, {"", -1});
        while (!freePageList.empty()) freePageList.pop();
        for (int i = 0; i < PAGE_FRAME_COUNT; ++i) freePageList.push(i);
        lruList.clear();
        runningProcesses.clear();
        hits = misses = referenceLogCount = processesSwappedIn = 0;
        currentTimeMs = 0;
        int jobIndex = 0;

        while (currentTimeMs <= SIM_TIME * 1000) {
            while (jobIndex < jobQueue.size() &&
                jobQueue[jobIndex].arrivalTime * 1000 <= currentTimeMs &&
                freePageList.size() >= 4) {
                Process p = jobQueue[jobIndex++];
                pageIn(p, 0, algo);
                runningProcesses.push_back(p);
                processesSwappedIn++;
                cout << fixed << setprecision(2) << currentTimeMs / 1000.0
                    << "s: Process " << p.name << " entered memory ("
                    << p.sizeInPages << " pages, " << p.duration << "s)" << endl;
                printMemoryMap();
            }

            for (auto it = runningProcesses.begin(); it != runningProcesses.end();) {
                it->remainingTime -= REFERENCE_INTERVAL;
                if (it->nextReferenceTime <= 0) {
                    int refPage = getRandomPageReference(*it, it->currentPage);
                    it->currentPage = refPage;
                    pageIn(*it, refPage, algo);
                    it->nextReferenceTime = REFERENCE_INTERVAL;
                }
                if (it->remainingTime <= 0) {
                    cout << fixed << setprecision(2) << currentTimeMs / 1000.0
                        << "s: Process " << it->name << " completed and exited memory" << endl;
                    for (int i = 0; i < PAGE_FRAME_COUNT; ++i) {
                        if (memory[i].processName == it->name) {
                            memory[i] = {"", -1};
                            freePageList.push(i);
                        }
                    }
                    // This prevents stale entries from affecting future evictions.
                    lruList.remove_if([&](const Page& pg) {
                        return pg.processName == it->name;
                    });

                    printMemoryMap();
                    it = runningProcesses.erase(it);
                } 
                else {
                    ++it;
                }
            }

            currentTimeMs += REFERENCE_INTERVAL;
        }
    }

    int main() {
        srand(time(NULL));
        double totalHitRatio = 0;
        double totalProcesses = 0;
        for (int run = 1; run <= 5; ++run) {
            cout << "\n=== RUN #" << run << " ===" << endl;
            initializeJobs();
            runSimulation(currentAlgo);
            double hitRatio = (double)hits / (hits + misses);
            totalHitRatio += hitRatio;
            totalProcesses += processesSwappedIn;
            cout << "Run #" << run << " Hits: " << hits << ", Misses: " << misses
                << ", Hit Ratio: " << hitRatio << ", Swapped-In: " << processesSwappedIn << endl;
        }

        cout << "\n=== FINAL AVERAGES ===" << endl;
        cout << "Avg Hit Ratio: " << totalHitRatio / 5.0 << endl;
        cout << "Avg Processes Swapped-In: " << totalProcesses / 5.0 << endl;
        return 0;
    }
