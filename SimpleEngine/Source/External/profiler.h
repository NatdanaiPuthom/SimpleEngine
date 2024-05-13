#pragma once

#ifndef _Simple //SimpleEngine Makes It Lazy
#define BUILD_WITH_EASY_PROFILER
#endif

#include <easy/profiler.h>

#ifndef _SIMPLE
#define PROFILER_ENABLED 0
#else
#define PROFILER_ENABLED 0
#endif

#if PROFILER_ENABLED
#define PROFILER_ENABLE() EASY_PROFILER_ENABLE
#define PROFILER_DISABLE() EASY_PROFILER_DISABLE
#define PROFILER_START_LISTEN() profiler::startListen()
#define PROFILER_INIT() PROFILER_ENABLE(); PROFILER_START_LISTEN();
#define PROFILER_DUMP_FILE(path) profiler::dumpBlocksToFile(path);
#define PROFILER_BEGIN(name) EASY_BLOCK(name)
#define PROFILER_END() EASY_END_BLOCK
#define PROFILER_FUNCTION(...) EASY_FUNCTION(__VA_ARGS__);
#else
#define PROFILER_ENABLE() 
#define PROFILER_DISABLE() 
#define PROFILER_START_LISTEN()
#define PROFILER_INIT()
#define PROFILER_DUMP_FILE(path)
#define PROFILER_BEGIN(name)
#define PROFILER_END()
#define PROFILER_FUNCTION(...)
#endif