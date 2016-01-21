#ifndef INCLUDE_THREADPOOL_H
#define INCLUDE_THREADPOOL_H

#include "ForwardDecs.h"
#include <functional>
#include <set>
#include <list>
#include <thread>
#include <mutex>
#include <vector>

class ThreadPool
{
public:
    class Job
    {
    public:
        friend ThreadPool;

        void queue();
        Job* addDependency(Job* other);
        Job* setAffinity(int thread);
        Job* setPriority(int amount);
        Job(std::function<void()> func, ThreadPool* pool);

    private:
        ThreadPool* const pool;
        std::function<void()> func;
        std::set<Job*> dependencies;
        std::set<Job*> dependents;
        int affinity = -1;
        int priority = 0;
    };

    explicit ThreadPool(size_t threadCount = 0);
    ~ThreadPool();

    const size_t threadCount;
    bool shutdown = false;

    Job* createJob(const std::function<void()>& func);
    bool completed(Job* job);
    void wait(Job* job);

private:
    static bool jobComparator(Job* first, Job* second);
    std::set<Job*, bool(*)(Job*, Job*)> readyQueue;
    std::list<std::thread> threads;
    std::vector<Job*> activeJobs;
    std::mutex jobLock;

    static void runThread(ThreadPool* pool, size_t id);
};

extern ThreadPool * workerPool;

#endif