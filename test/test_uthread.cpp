#include <iostream>
#include "Coroutine.h"

using namespace w;



// w::CoroutineManger uthread;
void print(int i, int c)
{
    std::cout << "print1 : " << i<< std::endl;
    CoroutineManger::getSingleton()->yield();
    std::cout << "print2 : "  << c << std::endl;
}
int main(int argc, char** argv)
{
    int ss =1;
    int id = CoroutineManger::getSingleton()->CreatCoroutine(print, ss+1, ss+2);
    CoroutineManger::getSingleton()->StartCoroutine(id);
    // uthread_main(uthread.coroutines[id]->func);
    std::cout << "main" << std::endl;
    int id2 = CoroutineManger::getSingleton()->CreatAndStartCoroutine(print, ss+3, ss+4);
    CoroutineManger::getSingleton()->resume(id);
    std::cout << "main" << std::endl;
    CoroutineManger::getSingleton()->resume(id2);
    print(1,1);
    return 0;
}
