/*
 * http://stackoverflow.com/questions/19480281/how-to-call-a-function-by-its-name-an-approach-using-stlmap-and-class
 * http://en.cppreference.com/w/cpp/utility/functional/bind
 * http://stackoverflow.com/questions/7624017/c0x-storing-any-type-of-stdfunction-in-a-stdmap
 *
 */

#pragma once
#include <map>
#include <functional>
#include <memory>
#include "IProcessor.h"


class BaseFunctor
{
public:
	virtual ~BaseFunctor() {}
};


template <typename... ArgTypes>
class Functor : public BaseFunctor
{
	typedef std::function<void(ArgTypes...)> func_t;
	func_t _f;
public:
	Functor() {};
	Functor(func_t f) : _f(f) {};
	void operator()(ArgTypes... args) { if (_f) _f(args...); }
};


template <typename... ArgTypes>
class StatusFunctor : public BaseFunctor
{
	typedef std::function<bool(ArgTypes...)> func_t;
	func_t _f;
public:
	StatusFunctor() {};
	StatusFunctor(func_t f) : _f(f) {};
	void operator()(ArgTypes... args) { }
	bool GetStatus(ArgTypes... args) { 
		bool status = false;
		if (_f) status = _f(args...);
		return status;
	}
};


typedef shared_ptr<BaseFunctor> BaseFunctorPtr;
typedef std::map<std::string, BaseFunctorPtr> FunctorMap;


class SCIDI_API StringIProcessor :
	public IProcessor
{
public:
	StringIProcessor(void);
	virtual ~StringIProcessor(void);
	
	void ExecFunc(std::string name);
	void ExecFunc(std::string name, ProcessorSettings* param);
	//template <class... Settings>
	//void ExecFunc(std::string name, Settings... param);
	bool GetStatus(std::string);
private:
	template <class T> void add(std::string name, const T& cmd);
	FunctorMap FuncNames;
};

PLUMA_PROVIDER_HEADER(StringIProcessor);