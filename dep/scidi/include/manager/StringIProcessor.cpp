
#include "StringIProcessor.h"

template <class T>
void StringIProcessor::add(std::string name, const T& cmd)
{
	FuncNames.insert( std::pair<std::string, BaseFunctorPtr>(name, BaseFunctorPtr(new T(cmd))) );
}

StringIProcessor::StringIProcessor(void)
{
	using namespace std::placeholders;
	
	auto f_start = std::bind(&StringIProcessor::start, this, _1);
	auto f_stop = std::bind(&StringIProcessor::terminate, this);
	auto f_processing = std::bind(&StringIProcessor::in_progress, this);
	auto f_completed = std::bind(&StringIProcessor::is_completed, this);

	add("Start", Functor<ProcessorSettings*>(f_start));
	add("Terminate", Functor<>(f_stop));
	add("Processing", StatusFunctor<>(f_processing));
	add("Completed", StatusFunctor<>(f_completed));
}

StringIProcessor::~StringIProcessor(){}

bool StringIProcessor::GetStatus(std::string name)
{
	typedef StatusFunctor<> status_t;
	status_t* f = static_cast<status_t*>(FuncNames[name].get());
	return f->GetStatus();
}

void StringIProcessor::ExecFunc(std::string name)
{
	typedef Functor<> func_t;
	func_t* f = dynamic_cast<func_t*>(FuncNames[name].get());
	(*f)();
}

void StringIProcessor::ExecFunc(std::string name, ProcessorSettings* param)
{
	typedef Functor<ProcessorSettings*> func_t;
	func_t* f = dynamic_cast<func_t*>(FuncNames[name].get());
	(*f)(param);
}

/*
template <class... Settings>
void StringIProcessor::ExecFunc(std::string name, Settings... param)
{
	typedef Functor<Settings...> func_t;
	func_t* f = dynamic_cast<func_t*>(FuncNames[name].get());
	(*f)(param...);
}
*/
PLUMA_PROVIDER_SOURCE(StringIProcessor, 1, 1);
