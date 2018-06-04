#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QBuffer>

#include "qtcoroutine.h"
#include "qtcoawaitables.h"
#include "qtcoqueue.h"
#include "qtcoiterator.h"
#include "qtcoconcurrent.h"

using namespace QtCoroutine;
using namespace std::chrono_literals;

namespace {

RoutineId coroutine_subroutine_tocancel_id;

QObject *testObj;
Queue<int> testQueue;

QBuffer testDevice;

}

void coroutine_subroutine_basic()
{
	qDebug() << "begin";
	yield();
	qDebug() << "end";
}

void coroutine_subroutine_abort()
{
	qDebug() << "begin";
	QtCoroutine::abort();
	qDebug() << "end";
}

void coroutine_subroutine_canceler()
{
	qDebug() << "begin";
	QtCoroutine::cancel(coroutine_subroutine_tocancel_id);
	qDebug() << "end";
}

void coroutine_subroutine_tocancel()
{
	qDebug() << "begin";
	coroutine_subroutine_tocancel_id = current();
	createAndRun(coroutine_subroutine_canceler);
	qDebug() << "end";
}

void coroutine_basic()
{
	qDebug() << "begin";
	yield();
	qDebug() << "back";

	// normal subroutine
	auto id = createAndRun(coroutine_subroutine_basic).first;
	qDebug() << "back";
	resume(id);
	qDebug() << "back";

	// aborting subroutine
	id = createAndRun(coroutine_subroutine_abort).first;
	qDebug() << "back";
	resume(id);
	qDebug() << "still here";

	// canceled subroutine
	id = createAndRun(coroutine_subroutine_tocancel).first;
	qDebug() << "back";
	resume(id);
	qDebug() << "still her";

	qDebug() << "end";
}

void coroutine_signals()
{
	qDebug() << "begin";

	auto tpl = awaitargs<QString>::tawait(testObj, &QObject::objectNameChanged);
	static_assert(std::is_same<decltype(tpl), std::tuple<QString>>::value, "tpl wrong");
	qDebug() << "name changed to tuple:" << std::get<0>(tpl);

	auto str = awaitargs<QString>::await(testObj, &QObject::objectNameChanged);
	static_assert(std::is_same<decltype(str), QString>::value, "str wrong");
	qDebug() << "name changed to string:" << str;

	auto obj = await(testObj, &QObject::destroyed);
	static_assert(std::is_same<decltype(obj), std::tuple<QObject*>>::value, "obj wrong");
	qDebug() << "object was destroyed:" << std::get<0>(obj);

	awaitargs<>::await(qApp, &QCoreApplication::aboutToQuit);
	qDebug() << "end";
}

void coroutine_devices()
{
	qDebug() << "begin";
	auto data = await(&testDevice, 3);
	qDebug() << "read 3 bytes:" << data;
	data = awaitLine(&testDevice);
	qDebug() << "read line:" << data;
	data = awaitAll(&testDevice);
	qDebug() << "read rest:" << data;
	qDebug() << "end";
}

void coroutine_queue_producer()
{
	qDebug() << "begin";
	for(auto i = 7; i < 42; i += 7) {
		qDebug() << "produced:" << i;
		testQueue.enqueue(i);
	}
	qDebug() << "end";
}

void coroutine_queue_consumer()
{
	qDebug() << "begin";
	int val;
	forever {
		val = testQueue.dequeue();
		qDebug() << "consumed:" << val;
	}
	qDebug() << "end";
}

void coroutine_iterator_producer()
{
	qDebug() << "begin";
	for(auto i = 0; i < 10; i++) {
		qDebug() << "produced:" << i;
		yield_return<int>(i);
	}
	qDebug() << "end";
}

void coroutine_iterator_consumer()
{
	qDebug() << "begin";
	Iterator<int> iterator{coroutine_iterator_producer};
	while(iterator.hasNext()) {
		auto next = iterator.next();
		qDebug() << "consumed:" << next;
	}
	qDebug() << "end";
}

void coroutine_concurrent()
{
	qDebug() << "begin";
	auto res = asyncAwait([](){
		QThread::sleep(2);
		return 42;
	});
	qDebug() << "async result" << res;
	qDebug() << "end";
}

void coroutine_wait_and_quit()
{
	qDebug() << "begin";
	await(3s);
	QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
	qDebug() << "end";
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	qSetMessagePattern("%{function}:%{line} -> %{message}");

	// test simple yield
	auto id = createAndRun(coroutine_basic).first;
	qDebug() << "back";
	resume(id);
	qDebug() << "back";

	// test await signals
	testObj = new QObject(qApp);
	createAndRun(coroutine_signals);
	qDebug() << "back, changing name to test1";
	testObj->setObjectName("test1");
	qDebug() << "back, changing name to test2";
	testObj->setObjectName("test2");
	qDebug() << "back, destroying object";
	delete testObj;
	qDebug() << "back";

	// test device
	testDevice.open(QIODevice::ReadOnly);
	id = createAndRun(coroutine_devices).first;
	qDebug() << "back";
	testDevice.buffer().append("hello you! ");
	emit testDevice.readyRead();
	qDebug() << "back";
	testDevice.buffer().append("Tell me more:\nAnd we have more data ");
	emit testDevice.readyRead();
	qDebug() << "back";
	testDevice.buffer().append("to be read.");
	emit testDevice.readyRead();
	qDebug() << "back";

	// test queue
	testQueue.enqueue(3);
	testQueue.enqueue(13);
	id = createAndRun(coroutine_queue_consumer).first;
	qDebug() << "back";
	testQueue.enqueue(666);
	qDebug() << "back";
	createAndRun(coroutine_queue_producer);
	qDebug() << "back";
	testQueue.enqueue(42);
	qDebug() << "back";
	cancel(id);

	// test iterator
	createAndRun(coroutine_iterator_consumer);
	qDebug() << "back";

	//test async
	createAndRun(coroutine_concurrent);
	qDebug() << "back";

	// test await timeout
	createAndRun(coroutine_wait_and_quit);
	qDebug() << "back";
	return a.exec();
}
