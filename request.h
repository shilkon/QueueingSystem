#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <memory>

namespace QueueingSystem
{
    struct RequestId
    {
        int sourceId;
        int serialNumber;
    };

    struct Request
    {
        RequestId id;
        double generationTime;
    };

    bool operator<(const Request& first, const Request& second);

    using URequest = std::unique_ptr<Request>;
}

#endif
