#include "profiler.h"

Profiler profiler;

PROFILER_TOKEN::PROFILER_TOKEN(std::string func) : m_FuncName(func) 
{
    m_StartTime = clock();
}

PROFILER_TOKEN::~PROFILER_TOKEN()
{
    clock_t end_time = clock();
    double elapsed = double(end_time - m_StartTime) / CLOCKS_PER_SEC;
    profiler.m_TimeAccounting[m_FuncName] += elapsed;
   // std::cout << m_FuncName << " time: " << profiler.m_TimeAccounting[m_FuncName] << "\n";
}

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
}
