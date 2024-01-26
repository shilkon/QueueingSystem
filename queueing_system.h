#ifndef QUEUEING_SYSTEM_H
#define QUEUEING_SYSTEM_H

#include "source.h"
#include "buffer.h"
#include "device.h"
#include "calendar_of_events.h"
#include "statistics.h"

#include <vector>
#include <memory>

namespace QueueingSystem
{
    struct SystemConfiguration
    {
        int sourcesCount{ 10 };
        float distrRange{ 5.0f };
        int bufferSize{ 50 };
        int devicesCount{ 90 };
        float lambda{ 0.05f };
        int requestsLimit{ 10000 };
    };

    using USystemConfiguration = std::unique_ptr<SystemConfiguration>;

    class QueueingSystem
    {
    public:
        QueueingSystem(const SystemConfiguration& conf);

        SystemStatus getSystemStatus() const;
        SystemFinalStats getSystemFinalStats() const;

        int getRequestsLimit() const;

        void reset();
        void reset(const SystemConfiguration& conf);

        bool makeStep();

    private:
        void processEvent(const EventConstIter& eventIter, EventType eventType);
        void tryProcessRequest(double startTime);

        std::vector<USource> sources_;
        std::vector<UDevice> devices_;
        std::unique_ptr<Buffer> buffer_;
        int deviceIndex_{};
        int requestsCount_{};
        int requestsLimit_;
        std::unique_ptr<CalendarOfEvents> calendarOfEvents_;
        std::unique_ptr<Statistics> stats_;
    };
}

#endif
