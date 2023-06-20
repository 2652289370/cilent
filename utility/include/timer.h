#pragma once 

#include <ctime>
#include <algorithm>
#include <functional>
#include <chrono>
#include <memory>
#include <iostream>
#include <set>
#include <time.h>


using namespace std;

struct TimerNodeBase 
{
    time_t expire;
    int id;
};


struct TimerNode :public TimerNodeBase{
    using Callback = std::function<void(const TimerNode &node)>;
    Callback func;
};

bool operator < (const TimerNodeBase &n1, const TimerNodeBase &n2){
    if (n1.expire < n2.expire)
    {
        return true;
    }
    else if (n1.expire > n2.expire)
    {
        return false;
    }
    return n1.id < n2.id; 
}

 
class Timer{
public:
    static time_t GetTick(){
        auto sc = chrono::time_point_cast<chrono::milliseconds>(chrono::steady_clock::now());
        auto temp = chrono::duration_cast<chrono::milliseconds>(sc.time_since_epoch());
        return temp.count();
    }

    int64_t GetID(){
        return ++gid;
    }

    TimerNodeBase AddTimer(time_t msec, TimerNode::Callback func){
        TimerNode tnode;
        tnode.expire = GetTick() + msec;
        tnode.id = GetID();
        timermap.insert(tnode);
        return static_cast<TimerNodeBase>(tnode);
    }

    bool DelTimer(TimerNodeBase &node){
        auto iter = timermap.find(node);
        if (iter != timermap.end())
        {
            timermap.erase(iter);
            return true;
        }
        return false;
    }

    bool CheckTimer(){
        auto iter = timermap.begin();
        if (iter != timermap.end() && iter->expire <= GetTick())
        {
            iter->func(*iter);
            return true;
        }
        return false;
    }

    time_t TimeToSleep(){
        auto iter = timermap.begin();
        if (iter == timermap.end())
        {
            return -1;
        }
        time_t diss = iter->expire - GetTick();
        return diss > 0 ? diss : 0;
    }
private:
    static int64_t gid;
    set<TimerNode, std::less<>> timermap;
};
int64_t Timer::gid = 0;