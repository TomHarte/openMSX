// $Id$

#include "Scheduler.hh"
#include "Schedulable.hh"
#include "Thread.hh"
#include "MSXCPU.hh"
#include "serialize.hh"
#include <cassert>
#include <algorithm>

namespace openmsx {

// TODO move to utils
template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator begin, InputIterator end,
                       OutputIterator out, Predicate p)
{
	while (begin != end) {
		if (p(*begin)) {
			*out = *begin;
			++out;
		}
		++begin;
	}
	return out;
}


struct LessSyncPoint {
	bool operator()(EmuTime::param time,
	                const SynchronizationPoint& sp) const;
	bool operator()(const SynchronizationPoint& sp,
	                EmuTime::param time) const;
};
bool LessSyncPoint::operator()(
	EmuTime::param time, const SynchronizationPoint& sp) const
{
	return time < sp.getTime();
}

bool LessSyncPoint::operator()(
	const SynchronizationPoint& sp, EmuTime::param time) const
{
	return sp.getTime() < time;
}


struct FindSchedulable {
	explicit FindSchedulable(const Schedulable& schedulable);
	bool operator()(const SynchronizationPoint& sp) const;
	const Schedulable& schedulable;
};
FindSchedulable::FindSchedulable(const Schedulable& schedulable_)
	: schedulable(schedulable_)
{
}

bool FindSchedulable::operator()(const SynchronizationPoint& sp) const
{
	return sp.getDevice() == &schedulable;
}


Scheduler::Scheduler()
	: scheduleTime(EmuTime::zero)
	, cpu(0)
	, scheduleInProgress(false)
{
}

Scheduler::~Scheduler()
{
	assert(!cpu);
	SyncPoints copy(syncPoints);
	for (SyncPoints::const_iterator it = copy.begin();
	     it != copy.end(); ++it) {
		it->getDevice()->schedulerDeleted();
	}

	assert(syncPoints.empty());
}

void Scheduler::setSyncPoint(EmuTime::param time, Schedulable& device, int userData)
{
	//PRT_DEBUG("Sched: registering " << device.schedName() <<
	//          " " << userData << " for emulation at " << time);
	assert(Thread::isMainThread());
	assert(time >= scheduleTime);

	// Push sync point into queue.
	SyncPoints::iterator it =
		std::upper_bound(syncPoints.begin(), syncPoints.end(), time,
		            LessSyncPoint());
	syncPoints.insert(it, SynchronizationPoint(time, &device, userData));

	if (!scheduleInProgress && cpu) {
		// only when scheduleHelper() is not being executed
		// otherwise getNext() doesn't return the correct time and
		// scheduleHelper() anyway calls setNextSyncPoint() at the end
		cpu->setNextSyncPoint(getNext());
	}
}

void Scheduler::getSyncPoints(SyncPoints& result, const Schedulable& device) const
{
	copy_if(syncPoints.begin(), syncPoints.end(), back_inserter(result),
	        FindSchedulable(device));
}

void Scheduler::removeSyncPoint(Schedulable& device, int userData)
{
	assert(Thread::isMainThread());
	for (SyncPoints::iterator it = syncPoints.begin();
	     it != syncPoints.end(); ++it) {
		if (((*it).getDevice() == &device) &&
		    ((*it).getUserData() == userData)) {
			syncPoints.erase(it);
			break;
		}
	}
}

void Scheduler::removeSyncPoints(Schedulable& device)
{
	assert(Thread::isMainThread());
	syncPoints.erase(remove_if(syncPoints.begin(), syncPoints.end(),
	                           FindSchedulable(device)),
	                 syncPoints.end());
}

bool Scheduler::pendingSyncPoint(Schedulable& device, int userData)
{
	assert(Thread::isMainThread());
	for (SyncPoints::iterator it = syncPoints.begin();
	     it != syncPoints.end(); ++it) {
		if ((it->getDevice() == &device) &&
		    (it->getUserData() == userData)) {
			return true;
		}
	}
	return false;
}

EmuTime::param Scheduler::getCurrentTime() const
{
	assert(Thread::isMainThread());
	return scheduleTime;
}

void Scheduler::scheduleHelper(EmuTime::param limit)
{
	assert(!scheduleInProgress);
	scheduleInProgress = true;
	while (true) {
		// Get next sync point.
		const SynchronizationPoint sp =
			  syncPoints.empty()
			? SynchronizationPoint(EmuTime::infinity, NULL, 0)
			: syncPoints.front();
		EmuTime::param time = sp.getTime();
		if (time > limit) {
			break;
		}

		assert(scheduleTime <= time);
		scheduleTime = time;

		syncPoints.erase(syncPoints.begin());

		Schedulable* device = sp.getDevice();
		assert(device);
		int userData = sp.getUserData();
		//std::cout << "Sched: Scheduling " << device->schedName()
		//          << " " << userData << " till " << time << std::endl;
		device->executeUntil(time, userData);
	}
	scheduleInProgress = false;

	cpu->setNextSyncPoint(getNext());
}


template <typename Archive>
void SynchronizationPoint::serialize(Archive& ar, unsigned /*version*/)
{
	// SynchronizationPoint is always serialized via Schedulable. A
	// Schedulable has a collection of SynchronizationPoints, all with the
	// same Schedulable. So there's no need to serialize 'device'.
	//Schedulable* device;
	ar.serializeNoID("time", timeStamp);
	ar.serialize("type", userData);
}
INSTANTIATE_SERIALIZE_METHODS(SynchronizationPoint);

template <typename Archive>
void Scheduler::serialize(Archive& ar, unsigned /*version*/)
{
	ar.serialize("currentTime", scheduleTime);
	// don't serialize syncPoints, each Schedulable serializes its own
	// syncpoints
}
INSTANTIATE_SERIALIZE_METHODS(Scheduler);

} // namespace openmsx
