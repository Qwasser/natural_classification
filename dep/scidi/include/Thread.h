/*
 * Copied from http://www.bogotobogo.com/cplusplus/multithreaded3.php
*/

// stackoverflow.com/questions/13662689/what-is-the-best-solution-to-pause-and-resume-pthreads

#pragma once

#include <memory> // auto_ptr
#include <cstdlib> // exit
#include <cstring> // strerror
#include <iostream>
#include <pthread.h>
#include <cassert>

#include "Runnable.h"
#include "setup.h"
#include "Callback.h"

using namespace std;

class SCIDI_API Thread
{
public:
	Thread(shared_ptr<Runnable> run, bool isDetached = false);
	Thread(bool isDetached = false);
	virtual ~Thread();
	void start();
	void* join();
	void terminate();
	bool in_progress() { return m_started; };
	bool is_completed() { return m_completed; };
	bool checkTermination();
private:
	// thread ID
	pthread_t ThreadID;
	// true if thread created in detached state
	bool detached;
	pthread_attr_t threadAttribute;
	// runnable object will be deleted automatically
	shared_ptr<Runnable> runnable;
	Thread(const Thread&);
	const Thread& operator=(const Thread&);
	
	bool m_started;
	bool m_completed;
	pthread_mutex_t m_TerminatedMutex;
	bool m_TerminationFlag;

	// called when run() completes
	virtual void setCompleted();
	// stores return value from run()
	void* result;
	virtual void* run() { return result; };
	static void* startThreadRunnable(void* pVoid);
	static void* startThread(void* pVoid);
	void printError(const char * msg, int status);
};


class SCIDI_API TerminalThread : public Thread
{
public:
	TerminalThread(shared_ptr<Runnable> run) : Thread(run) { }
	void setCallback(sdEvent* c) { _callback = c; };
private:
	void setCompleted() { _callback->signal("completed"); };
	sdEvent* _callback;
};


class SCIDI_API ThreadCommand :
	public sdCommand
{
public:
	ThreadCommand(Thread* thread) : _thread(thread) {}
	bool Execute() { return _thread->checkTermination(); };
private:
	Thread* _thread;
};
