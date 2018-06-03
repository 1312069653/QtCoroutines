#include <QCoreApplication>
#include <iostream>
#include <chrono>
#include <functional>

#include "qtcoroutine.h"
#include "qtawaitables.h"

/*
coroutine::Channel<int> channel;

string async_func()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	return "22";
}

void routine_func1()
{
	int i = channel.pop();
	std::cout << i << std::endl;

	i = channel.pop();
	std::cout << i << std::endl;
}

void routine_func2(int i)
{
	std::cout << "20" << std::endl;
	coroutine::yield();

	std::cout << "21" << std::endl;

	//run function async
	//yield current routine if result not returned
	string str = coroutine::await(async_func);
	std::cout << str << std::endl;
}

void thread_func()
{
	//create routine with callback like std::function<void()>
	coroutine::routine_t rt1 = coroutine::create(routine_func1);
	coroutine::routine_t rt2 = coroutine::create(std::bind(routine_func2, 2));

	std::cout << "00" << std::endl;
	coroutine::resume(rt1);

	std::cout << "01" << std::endl;
	coroutine::resume(rt2);

	std::cout << "02" << std::endl;
	channel.push(10);

	std::cout << "03" << std::endl;
	coroutine::resume(rt2);

	std::cout << "04" << std::endl;
	channel.push(11);

	std::cout << "05" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(6000));
	coroutine::resume(rt2);

	//destroy routine, free resouce allocated
	//Warning: don't destroy routine by itself
	coroutine::destroy(rt1);
	coroutine::destroy(rt2);
}
*/

int main(int argc, char *argv[])
{
	using namespace QtCoroutine;
	using namespace std::chrono_literals;

	QCoreApplication a(argc, argv);

	// compile test
	await(10s);
	awaitargs<>::await(qApp, &QCoreApplication::aboutToQuit);
	auto tpl = awaitargs<QString>::tawait(qApp, &QCoreApplication::objectNameChanged);
	auto str = awaitargs<QString>::await(qApp, &QCoreApplication::objectNameChanged);
	auto obj = std::get<0>(await(qApp, &QCoreApplication::destroyed));
	static_assert(std::is_same<decltype(tpl), std::tuple<QString>>::value, "tpl wrong");
	static_assert(std::is_same<decltype(str), QString>::value, "str wrong");
	static_assert(std::is_same<decltype(obj), QObject*>::value, "obj wrong");

	// compile test

//	std::thread t1(thread_func);
//	std::thread t2([](){
//		//unsupport coordinating routine crossing threads
//	});
//	t1.join();
//	t2.join();
	return 0;
}
