#include "Coroutine.h"


thread_local w::CoroutineManger* w::CoroutineManger::CoPtr = nullptr;


namespace w
{   
    Schedule::Schedule(/* args */)
{
}

Schedule::~Schedule()
{
}


CoroutineManger::CoroutineManger():running_thread(-1)
{
    
}

CoroutineManger::~CoroutineManger()
{

}


void uthread_main(CoroutineManger* ut)
{
    if (ut->running_thread == -1)
    {
        return;
    }
    ut->coroutines[ut->running_thread]->func();
    ut->coroutines[ut->running_thread]->state = CoroutineStatus::FREE;
    ut->running_thread = -1;
}

void CoroutineManger::StartCoroutine(const int& id)
{
    running_thread = id;
    if (coroutines[id]->state != CoroutineStatus::RUNNABLE)
    {
        return;
    }
    coroutines[running_thread]->state = CoroutineStatus::RUNNING;
    swapcontext(&main_ctx, &coroutines[id]->ctx);
}

void CoroutineManger::yield()
{
    if (running_thread != -1 && coroutines[running_thread]->state == CoroutineStatus::RUNNING)
    {
        int id = running_thread;
        coroutines[id]->state = CoroutineStatus::SUSPEND;
        running_thread = -1;
        swapcontext(&(coroutines[id]->ctx), &main_ctx);
    }
    
}

void CoroutineManger::resume(const int& id)
{
    if (coroutines[id]->state != CoroutineStatus::SUSPEND || id < -1 \
    || id == running_thread)
    {
        return;
    }
    running_thread = id;
    coroutines[id]->state = CoroutineStatus::RUNNING;
    swapcontext(&(main_ctx), &(coroutines[id]->ctx));
}

CoroutineManger* CoroutineManger::getSingleton()
{
    if (CoPtr == nullptr)
    {
        CoPtr = new CoroutineManger();
    }
    return CoPtr;
}



}




