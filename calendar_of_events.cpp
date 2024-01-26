#include "calendar_of_events.h"

#include <algorithm>

namespace QS = QueueingSystem;

QS::CalendarOfEvents::CalendarOfEvents(const std::vector<USource>& sources,
    const std::vector<UDevice>& devices):
    sourcesEventTime_(sources.size()),
    devicesEventTime_(devices.size())
{
    for (int i{}; i < sources.size(); ++i)
        sourcesEventTime_[i] = sources[i]->getNextGenerationTimePtr();

    for (int i{}; i < devices.size(); ++i)
        devicesEventTime_[i] = devices[i]->getProcessingEndTimePtr();
}

QS::EventConstIter QS::CalendarOfEvents::getNextEvent(EventType eventType) const
{
    if (eventType == EventType::sourceEvent)
        return std::min_element(
            sourcesEventTime_.cbegin(), sourcesEventTime_.cend(),
            [](const auto& left, const auto& right)
            {
                return *left < *right;
            });
    else //EventType::deviceEvent
    {
        return std::min_element(
            devicesEventTime_.cbegin(), devicesEventTime_.cend(),
            [](const auto& left, const auto& right)
            {
                if (*left < 0.0)
                    return false;
                else if (*right < 0.0)
                    return true;
                else
                    return *left < *right;
            });
    }
}

int QS::CalendarOfEvents::getEventIndex(const EventConstIter& eventIter, EventType eventType) const
{
    if (eventType == EventType::sourceEvent)
        return eventIter - sourcesEventTime_.cbegin();
    else //EventType::deviceEvent
        return eventIter - devicesEventTime_.cbegin();
}

int QS::CalendarOfEvents::getFreeDeviceIndex(int deviceIndex) const
{
    auto devicesStatusBegin = devicesEventTime_.cbegin();
    auto isDeviceFree = [](const double* status) { return *status < 0.0; };

    auto it = std::find_if(devicesStatusBegin + deviceIndex,
        devicesEventTime_.cend(), isDeviceFree);
    if (it != devicesEventTime_.end())
        return it - devicesStatusBegin;

    it = std::find_if(devicesStatusBegin, devicesStatusBegin + deviceIndex, isDeviceFree);
    return it - devicesStatusBegin != deviceIndex ?
        it - devicesStatusBegin : devicesEventTime_.size();
}

std::vector<const double*> QueueingSystem::CalendarOfEvents::getSourcesEventTime() const
{
    return sourcesEventTime_;
}

std::vector<const double*> QueueingSystem::CalendarOfEvents::getDevicesEventTime() const
{
    return devicesEventTime_;
}
