#include "ThreadPool.h"

void ThreadPool::Job::queue()
{
    pool->jobLock.lock();
    pool->readyQueue.insert(this);
    pool->jobLock.unlock();
}

void ThreadPool::Job::addDependency(Job* other)
{
    dependencies.insert(other);
    other->dependents.insert(other);
}

bool ThreadPool::jobComparator(Job* first, Job* second)
{
    return first->priority > second->priority;
}

ThreadPool::Job::Job(std::function<void()> func, ThreadPool* pool) : pool(pool), func(func)
{
}

ThreadPool::ThreadPool(size_t threadCount) : threadCount(threadCount > 0 ? threadCount : std::thread::hardware_concurrency()), readyQueue(&jobComparator)
{
    for (size_t i = 0; i < threadCount; i++)
    {
        threads.emplace_back(runThread, this, i);
    }
}

ThreadPool::~ThreadPool()
{
    shutdown = true;
    for(auto& thread : threads)
    {
        thread.join();
    }
}

ThreadPool::Job* ThreadPool::createJob(const std::function<void()>& func)
{
    return new Job(func, this);
}

bool ThreadPool::completed(Job* job)
{
    bool ret;
    jobLock.lock();
    ret = readyQueue.count(job) == 0;
    jobLock.unlock();
    return ret;
}

void ThreadPool::wait(Job* job)
{
    while(!completed(job)) std::this_thread::yield();
}

void ThreadPool::runThread(ThreadPool* pool, size_t id)
{
    while(!pool->shutdown)
    {
        pool->jobLock.lock();
        for(auto job : pool->readyQueue)
        {
            if (job->dependencies.size() > 0) continue;
            if (job->affinity >= 0 && job->affinity != id) continue;
            pool->readyQueue.erase(job);
            pool->jobLock.unlock();

            job->func();

            pool->jobLock.lock();
            for (auto dependent : job->dependents)
            {
                dependent->dependencies.erase(job);
            }
            delete job;
            break;
        }
        pool->jobLock.unlock();
    }
}
