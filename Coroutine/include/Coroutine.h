#pragma once

#include <ucontext.h>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <future>
#include "Log.h"

namespace w{

    #define DEFAULT_STACK_SZIE (1024*128)
    #define MAX_COROUTINE_SIZE   1024


    enum CoroutineStatus
        {
            FREE, // 空闲状态
            RUNNABLE, // 就绪状态
            RUNNING, // 运行状态
            SUSPEND // 暂停状态
        };

        struct Coroutine
        {
            typedef std::shared_ptr<Coroutine> Ptr;
            ucontext_t ctx;
            std::function<void()> func;
            CoroutineStatus state;
            char stack[DEFAULT_STACK_SZIE];
            Coroutine(){}
            ~Coroutine(){         
            }
        };

        class Schedule
        {
        public:
            Schedule(/* args */);
            ~Schedule();
        private:
            
        };

    class CoroutineManger
    {
        friend void uthread_main(CoroutineManger* ut);
        friend int getFireberID();
    public:
        /**
         * @brief 创建协程
         * @param f 输入函数
         * @param args 输入函数的参数
        */
       template<typename F, typename ...Args>
       int CreatCoroutine(F&& f, Args&& ...args);
       /**
         * @brief 创建协程并接着执行
         * @param f 输入函数
         * @param args 输入函数的参数
        */
       template<typename F, typename ...Args>
       int CreatAndStartCoroutine(F&& f, Args&& ...args);
        /**
         * @brief 开启协程
         * @param id 协程的id
        */
       void StartCoroutine(const int& id);
        /**
         * @brief 暂停正在运行的协程
        */
       void yield();
        /**
         * @brief 继续执行暂停的协程
        */
       void resume(const int& id);
       /**
        * @brief 获取对象在当前线程的单例
       */
       static CoroutineManger* getSingleton();
    private:
        CoroutineManger(/* args */);
        ~CoroutineManger();
        CoroutineManger(const CoroutineManger &signal);

        // 禁止外部赋值操作
        const CoroutineManger &operator=(const CoroutineManger &signal);
    private:
        ucontext_t main_ctx;
        int running_thread;
        std::vector<Coroutine::Ptr> coroutines;
        thread_local static  CoroutineManger*  CoPtr;
    };
   
    void uthread_main(CoroutineManger* ut);
    

template<typename F, typename ...Args>
int CoroutineManger::CreatCoroutine(F&& f, Args&& ...args)
{
    std::function<void()> warpper_func= std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    int id = -1;
    for (size_t i = 0; i < coroutines.size(); i++)
    {
        if (coroutines[i]->state == CoroutineStatus::FREE)
        {
            id = i;
            break;
        }
        
    }
    
    Coroutine::Ptr co = std::make_shared<Coroutine>(Coroutine());
    co->state = CoroutineStatus::RUNNABLE;
    co->func = std::move(warpper_func);
    getcontext(&co->ctx);
    co->ctx.uc_stack.ss_sp = co->stack;
    co->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
    co->ctx.uc_link = &main_ctx;

    if(id ==-1)
    {
        coroutines.push_back(co);
        id = coroutines.size() - 1;
    }
    else
    {
        coroutines[id] = co;
    }
    makecontext(&co->ctx, (void (*)())&(uthread_main), 1, CoPtr);
    return id;
}


template<typename F, typename ...Args>
int CoroutineManger::CreatAndStartCoroutine(F&& f, Args&& ...args)
{
    int id = CreatCoroutine(std::forward<F>(f), std::forward<Args>(args)...);
    StartCoroutine(id);
    return id;
}
 
    
}