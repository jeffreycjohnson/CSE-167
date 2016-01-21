#include "ThreadPool.h"

ThreadPool * workerPool;

void ThreadPool::Job::queue()
{
    pool->jobLock.lock();
    pool->readyQueue.insert(this);
    pool->jobLock.unlock();
}

ThreadPool::Job* ThreadPool::Job::addDependency(Job* other)
{
    dependencies.insert(other);
    other->dependents.insert(this);
    return this;
}

ThreadPool::Job* ThreadPool::Job::setAffinity(int thread)
{
    affinity = thread;
    return this;
}

ThreadPool::Job* ThreadPool::Job::setPriority(int amount)
{
    priority = amount;
    return this;
}

ThreadPool::Job::Job(std::function<void()> func, ThreadPool* pool) : pool(pool), func(func)
{
}

bool ThreadPool::jobComparator(Job* first, Job* second)
{
    if(first->priority != second->priority) return first->priority > second->priority;
    return first > second;
}

ThreadPool::ThreadPool(size_t threadCount) : threadCount(threadCount > 0 ? threadCount : std::thread::hardware_concurrency()), readyQueue(&jobComparator)
{
    activeJobs.resize(this->threadCount);
    for (size_t i = 0; i < this->threadCount; i++)
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
    jobLock.lock();
    if (readyQueue.count(job) != 0) {
        jobLock.unlock();
        return false;
    }
    bool ret = true;
    for (auto active : activeJobs) {
        if (active == job) ret = false;
    }
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
            pool->activeJobs[id] = job;
            pool->jobLock.unlock();

            job->func();

            pool->jobLock.lock();
            for (auto dependent : job->dependents)
            {
                dependent->dependencies.erase(job);
            }
            delete job;
            pool->activeJobs[id] = nullptr;
            break;
        }
        pool->jobLock.unlock();
    }
}
