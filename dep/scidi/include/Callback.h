// Pattern Command and Observer + further reading:
// http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible
// http://www.codeproject.com/Articles/136799/Lightweight-Generic-C-Callbacks-or-Yet-Another-Del
// http://www.newty.de/jakubik/callback.pdf
// http://stackoverflow.com/questions/12988320/store-function-pointers-to-any-member-function
//

#ifndef SDEVENTH
#define SDEVENTH

#include <cstdlib> // std::size_t
#include <memory>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <tuple>
#include <functional>

// http://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments

namespace helper
{
	template <std::size_t... Ts>
	struct index {};

	template <std::size_t N, std::size_t... Ts>
	struct gen_seq : gen_seq<N - 1, N - 1, Ts...> {};

	template <std::size_t... Ts>
	struct gen_seq<0, Ts...> : index<Ts...>{};
}

struct Observer {
	virtual ~Observer() {}
	virtual bool invoke() = 0;
};

struct ObserverFunc : Observer {
	ObserverFunc(void(*f)()) : _f(f) {}

	virtual bool invoke() override { _f(); return true; }

	void(*_f)();
};

template <typename... ArgTypes>
struct ObserverStdFunc : Observer {
	typedef std::function<void(ArgTypes...)> func_t;
	typedef std::tuple<ArgTypes...> arg_t;

	ObserverStdFunc(func_t f) : _f(f) {}

	void SetArg(ArgTypes... arg) {
		args = std::make_tuple(std::forward<ArgTypes>(arg)...);
	}

	template <typename... Args, std::size_t... N>
	void func(std::tuple<Args...>& tup, helper::index<N...>)
	{
		_f(std::get<N>(tup)...);
	}

	template <typename... Args>
	void func(std::tuple<Args...>& tup)
	{
		func(tup, helper::gen_seq<sizeof...(Args)>{});
	}

	virtual bool invoke() {
		if (_f){
			func(args);
			return true;
		}
		else {
			return false;
		}
	}

	func_t _f;
	arg_t args;
};


class SCIDI_API sdEvent {

private:
	

	template <typename T>
	struct ObserverMember : Observer {
		typedef void (T::*Action)();
		
		ObserverMember(std::weak_ptr<T> p, Action f) : _p(p), _f(f) {}

		virtual bool invoke() override {
			std::shared_ptr<T> p = _p.lock();
			if (p && _f) {
				(p.get()->*_f)();
				return true;
			}
			else {
				return false;
			}
		}

		std::weak_ptr<T> _p;
		Action _f;
	};

	typedef std::unique_ptr<Observer> OPtr;
	typedef std::vector<OPtr> Observers;

	// mutable because we remove observers lazily
	mutable std::unordered_map<std::string, Observers> _observers;

public:
	// Callback observers of "name"
	// Returns the number of observers so invoked
	template <typename... ArgTypes>
	size_t signal(std::string const& name, ArgTypes... arg) const {
		auto const it = _observers.find(name);
		if (it == _observers.end()) { return 0; }

		Observers& obs = it->second;

		size_t count = 0;
		auto invoker = [&count](OPtr const& p) -> bool {
			bool const invoked = p->invoke();
			count += invoked;
			return !invoked; // if not invoked, remove it!
		};

		obs.erase(std::remove_if(obs.begin(), obs.end(), invoker), obs.end());

		if (obs.empty()) { _observers.erase(it); }

		return count;
	}

	template <typename... ArgTypes>
	void SetArg(std::string const& name, ArgTypes... arg) {
		auto const it = _observers.find(name);
		if (it == _observers.end()) { return; }

		Observers& obs = it->second;
		typedef ObserverStdFunc<ArgTypes... > obs_t;
		
		obs_t* _p = dynamic_cast<obs_t*>(obs[0].get());
		_p->SetArg(arg...);
	}
	// Registers a function callback on event "name"
	void connect(std::string const& name, void(*f)()) {
		_observers[name].push_back(OPtr(new ObserverFunc(f)));
	}

	// Registers an object callback on event "name"
	template <typename T>
	void connect(std::string const& name, std::shared_ptr<T> const& p, void (T::*f)()) {
		_observers[name].push_back(OPtr(new ObserverMember<T>(p, f)));
	}

	// Registers an object callback on event "name"
	template <typename... ArgTypes>
	void connect(std::string const& name, std::function<void(ArgTypes...)> f) {
		_observers[name].push_back(OPtr(new ObserverStdFunc<ArgTypes...>(f)));
	}

	void clear() {
		_observers.clear();
	}

}; // class EventManager


class SCIDI_API sdCommand
{
public:
	virtual bool Execute()=0;
};


template <class Receiver>
class SCIDI_API sdCallback : public sdCommand // Inheriting
{
	// (You can change the callback to take more parameters or to return something)
	typedef void (Receiver::*Action)();

	public:
		sdCallback() // constructor
		{
			// Important : zeroing the pointer so we later can check for errors
			pFunction = 0;
		}
		sdCallback(Action _action) : pFunction(_action) {}
		//sdCallback(Receiver *_receiver, Action _action) : _receiver(cInst), _action(pFunction) {}

		// Execute the Callback
		virtual void Execute()
		{
			if (pFunction) return (cInst->*pFunction)();
			else printf("ERROR : the callback function has not been defined !!!!");
			// an undefined callback function will crash the program if you don't check here !
		}

		// Set or change the Callback
		void SetCallback (Receiver  *ReceiverPointer, 
						  Action   pFunctionPointer)
		{
			cInst     = ReceiverPointer;
			pFunction = pFunctionPointer;
		}

	private:
		Receiver  *cInst;
		Action  pFunction;
};

#endif // SDEVENTH




