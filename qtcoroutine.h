#ifndef QTCOROUTINE_H
#define QTCOROUTINE_H

#include <functional>
#include <chrono>

#include <QAtomicInt>
#include <QObject>
#include <QSet>
#include <QSharedPointer>

#include "qtcoroutine_global.h"

namespace QtCoroutine
{

// standard coroutine methods, as wrappers around the header lib

using RoutineId = quint64;
static const RoutineId InvalidRoutineId = 0;

enum ResumeResult {
	Finished,
	Paused,
	Error
};

QTCOROUTINE_EXPORT extern QAtomicInteger<size_t> StackSize;

QTCOROUTINE_EXPORT RoutineId create(std::function<void()> fn);
QTCOROUTINE_EXPORT bool isPaused(RoutineId id);
QTCOROUTINE_EXPORT ResumeResult resume(RoutineId id);
QTCOROUTINE_EXPORT void cancel(RoutineId id);
QTCOROUTINE_EXPORT std::pair<RoutineId, ResumeResult> createAndRun(std::function<void()> fn);

QTCOROUTINE_EXPORT RoutineId current();
QTCOROUTINE_EXPORT void yield();
QTCOROUTINE_EXPORT void abort();

template <typename TAwaitable>
typename TAwaitable::type await(TAwaitable &&awaitable) {
	awaitable.prepare(std::bind(&QtCoroutine::resume, current()));
	yield();
	return awaitable.result();
};

template <typename TContainer, typename THandler>
void awaitEach(const TContainer &container, THandler handler) {
	auto suspendedIds = QSharedPointer<QSet<RoutineId>>::create();
	const auto selfId = current();
	for(const auto &data : container) {
		auto subRes = createAndRun([fn{std::move(handler)}, data, selfId, suspendedIds](){
			fn(data);
			if(suspendedIds->remove(current()) && suspendedIds->isEmpty())
				resume(selfId); //TODO use resumeAfter
		});
		if(subRes.second == Paused)
			suspendedIds->insert(subRes.first);
	}

	if(!suspendedIds->isEmpty())
		yield();
}

// theoretical template interface for await
// You don't have to implement this interface - it is only ment as documentation.
// Any class you implement that provides members with the same signatures (generic or non generic doesn't matter) can be passed to await
// You can still use this interface though if you want to enforce someone to implement an awaitable
template <typename T>
class Awaitable
{
public:
	using type = T;
	virtual void prepare(std::function<void()>) = 0;
	virtual type result() = 0;
};

};

#endif // QTCOROUTINE_H
