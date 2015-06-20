#ifndef RunnableH
#define RunnableH

/*
 *
 * We created threads in the same way as Java did.
 * The Runnable interface defines only one method, "public void run()".
 * It's an interface, so the method is public.
 * We will write a C++ class that provides a run() method and will inherits from Runnable.

    Passing auto_ptr<Runnable> object to the Thread class constructor passes ownership of the Runnable object from the main thread to the child thread.
    The auto_ptr<Runnable> object in the child thread that receives the auto_ptr<Runnable> object owns the Runnable object that it has a pointer to, and will automatically delete the pointed-to object when the child thread is destroyed.
    When ownership is passed to the thread, the auto_ptr<Runnable> object in main is set automatically to a null state and can no longer be used to refer to the Runnable object. This protects against double deletion by the child thread and the main thread. It also prevents main from deleting the Runnable object before the thread has completed method run() and from accessing the Runnable object while the thread is accessing it.

 */

class Runnable {
public:
	virtual void* run() = 0;
	virtual ~Runnable() = 0;
};

#endif
