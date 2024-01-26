#ifndef CALENDAR_OF_EVENTS
#define CALENDAR_OF_EVENTS

#include "source.h"
#include "device.h"

#include <vector>
#include <memory>

namespace QueueingSystem
{
    enum class EventType
    {
        sourceEvent,
        deviceEvent,
    };

    using EventConstIter = std::vector<const double*>::const_iterator;

    class CalendarOfEvents
    {
    public:
        CalendarOfEvents(const std::vector<USource>& sources,
            const std::vector<UDevice>& devices);

        EventConstIter getNextEvent(EventType eventType) const;
        int getEventIndex(const EventConstIter& eventIter, EventType eventType) const;
        int getFreeDeviceIndex(int deviceIndex) const;

        std::vector<const double*> getSourcesEventTime() const;
        std::vector<const double*> getDevicesEventTime() const;

    private:
        std::vector<const double*> sourcesEventTime_;
        std::vector<const double*> devicesEventTime_;
    };
}

#endif
