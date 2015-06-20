
#include "Runnable.h"

// Pure virtual destructor: function body required
Runnable::~Runnable(){};

#include "Thread.h"

// two macros ensures any macro passed will
// be expanded before being stringified
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define logError(msg) (msg " " __FILE__ ":" STRINGIZE(__LINE__))


Thread::Thread(shared_ptr<Runnable> r, bool isDetached) : 
	runnable(r), detached(isDetached),
	m_started(false), m_completed(false) {
	if(!runnable.get()){
		cout << "Thread::Thread(shared_ptr<Runnable> r, bool isDetached)"\
		"failed at " << " " << __FILE__ <<":" << __LINE__ << "-" <<
		" runnable is NULL" << endl;
		exit(-1);
	}
}

Thread::Thread(bool isDetached) : runnable(NULL), detached(isDetached), m_started(false), m_completed(false) { }

void* Thread::startThreadRunnable(void* pVoid) {
	// thread start function when a Runnable is involved
	Thread* runnableThread = static_cast<Thread*>(pVoid);
	assert(runnableThread);
	runnableThread->result = runnableThread->runnable->run();
	runnableThread->m_started = false;
	runnableThread->m_completed = true;
	runnableThread->setCompleted();
	return runnableThread->result;
}

void* Thread::startThread(void* pVoid) {
	// thread start function when no Runnable is involved
	Thread* aThread = static_cast<Thread*>(pVoid);
	assert(aThread);
	aThread->result = aThread->run();
	aThread->m_started = false;
	aThread->m_completed = true;
	aThread->setCompleted();
	return aThread->result;
}

Thread::~Thread() {}

void Thread::start() {
	// initialize termination feature
	m_TerminationFlag = false;
	pthread_mutex_init(&m_TerminatedMutex, NULL);

	// initialize attribute object
	int status = pthread_attr_init(&threadAttribute);
	if(status) {
		printError(logError("pthread_attr_init failed at"), status);
		exit(status);
	}

	// set the scheduling scope attribute
	status = pthread_attr_setscope(&threadAttribute,
					PTHREAD_SCOPE_SYSTEM);
	if(status) {
		printError(logError("pthread_attr_setscope failed at"), status);
		exit(status);
	}

	if(detached) {
	    // set the detachstate attribute to detached
		status = pthread_attr_setdetachstate(&threadAttribute,
						PTHREAD_CREATE_DETACHED);	
		if(status) {
			printError(logError("pthread_attr_setdetachstate failed at"), status);
			exit(status);
		}
	}
	else {
		// set the detachstate attribute to joinable
		status = pthread_attr_setdetachstate(&threadAttribute,
			PTHREAD_CREATE_JOINABLE);
		if (status) {
			printError(logError("pthread_attr_setdetachstate failed at"), status);
			exit(status);
		}
	}
	
	// create thread
	if(!runnable.get()) {
		status = pthread_create(&ThreadID, &threadAttribute,
			Thread::startThread, (void*)this);	
		if(status) {
			printError(logError("pthread_create failed at"), status);
			exit(status);
		}
	}
	else {
		status = pthread_create(&ThreadID, &threadAttribute,
			Thread::startThreadRunnable, (void*)this);	
		if(status) {
			printError(logError("pthread_create failed at"), status);
			exit(status);
		}
	}
	
	status = pthread_attr_destroy(&threadAttribute);
	if(status) {
		printError(logError("pthread_attr_destroy failed at"), status);
		exit(status);
	}
	m_started = true;
}

void* Thread::join() {
	// A thread calling T.join() waits until thread T completes.
	int status = pthread_join(ThreadID, NULL);
	// result was already saved by thread start function
	if(status) {
		printError(logError("pthread_join failed at"), status);
		exit(status);
	}
	return result;
}

void Thread::terminate()
{ // tell the thread to suspend
    pthread_mutex_lock(&m_TerminatedMutex);
    m_TerminationFlag = true;
    pthread_mutex_unlock(&m_TerminatedMutex);
}

bool Thread::checkTermination()
{
	return m_TerminationFlag;
}

void Thread::setCompleted() {
	// completion handled by pthread_join()
}

void Thread::printError(const char * msg, int status) {
	cout << msg << "-" << strerror(status) << endl;
}



