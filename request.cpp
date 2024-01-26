#include "request.h"

namespace QS = QueueingSystem;

bool QS::operator<(const Request& first, const Request& second)
{
    return first.id.sourceId < second.id.sourceId;
}
