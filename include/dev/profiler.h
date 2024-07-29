#ifndef PROFILER_H
#define PROFILER_H

#include <unordered_map>
#include <ctime>
#include <string>
#include <iostream>


#define EMIT_PROFILE_TOKEN PROFILER_TOKEN tok(std::string(typeid(*this).name()) + "::" + std::string(__func__)) ;

/*
    WARNING: INSERTING THIS INTO MULTITHREADED FUNCTIONS CAN CAUSE ERRORS
             USE ONLY IN CRITICAL PATH
*/
struct PROFILER_TOKEN
{
    PROFILER_TOKEN(std::string function = __func__);
    ~PROFILER_TOKEN();
    clock_t m_StartTime;
    std::string m_FuncName;
};






class Profiler
{
public:
    Profiler();
    ~Profiler();


    // we need to set up locks for this if we want to profile multiple threads --> probably slow
    std::unordered_map<std::string, double> m_TimeAccounting;
private:
    
};



#endif