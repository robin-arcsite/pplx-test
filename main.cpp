//
//  main.cpp
//  pplx-test
//
//  Created by Robin Peng on 2021/10/22.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include <pplx/pplxtasks.h>

using namespace std::chrono_literals;

pplx::task<int> RunTask(int id) {
    return pplx::create_task([id](){
        auto time = 50ms * (std::rand() % 5 + 1);
        std::this_thread::sleep_for(time);
        std::cout << "task(" << id << ") done in " << time.count() << "ms." << std::endl;
        return id;
    });
}

pplx::task<int> RunTaskChain(int id) {
    auto primary_task = pplx::create_task([id](){
        auto time = 50ms * (std::rand() % 5 + 1);
        std::this_thread::sleep_for(time);
        std::cout << "primary task(" << id << ") done in " << time.count() << "ms." << std::endl;
        return id;
    });
    
    return primary_task.then([](int id){
        auto time = 25ms * (std::rand() % 5 + 1);
        std::this_thread::sleep_for(time);
        std::cout << "secondary task(" << id << ") done in " << time.count() << "ms." << std::endl;
        return id;
    });
}

pplx::task<int> RunNestedTask(int id) {
    return pplx::create_task([id](){
        auto sub_task1 = RunTask(id*100+1);
        int id1 = sub_task1.get();
        
        auto sub_task2 = RunTask(id1 + 1);
        auto id2 = sub_task2.get();
        
        return id2;
    });
}

int main(int argc, const char * argv[]) {
    const int kTaskPoolSize = 100;
    
    std::cout << "start >>" << std::endl;
    std::vector<pplx::task<int>> tasks;
    for (int i = 0; i < kTaskPoolSize; ++i) {
        tasks.push_back(RunNestedTask(i));
    }
    std::cout << "(task pool built)" << std::endl;
    
    pplx::when_all(tasks.begin(), tasks.end()).get();
    std::cout << "<< end" << std::endl;
    
    return 0;
}
