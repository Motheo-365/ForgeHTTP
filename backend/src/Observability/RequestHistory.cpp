#include "Observability/RequestHistory.h"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace {
std::tm utcTime(std::time_t timestamp) {
    std::tm timeInfo{};

#ifdef _WIN32
    gmtime_s(&timeInfo, &timestamp);
#else
    gmtime_r(&timestamp, &timeInfo);
#endif

    return timeInfo;
}

}

RequestHistory::RequestHistory(std::size_t maxEntries)
    : maxEntries(maxEntries) {}

void RequestHistory::onEvent(const ServerEvent &event) {

    if (event.type != ServerEventType::ResponseSent) {
        return;
    }

    if (event.internal) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);

    requests.push_back(event);

    if (requests.size() > maxEntries)
    {
        requests.pop_front();
    }
}

HttpResponse RequestHistory::getRequests(std::size_t limit) {
    std::lock_guard<std::mutex> lock(mutex);

    if (limit > requests.size()) {
        limit = requests.size();
    }

    std::ostringstream json;

    json << "[";
    auto start = requests.end() - static_cast<std::ptrdiff_t>(limit);
    bool first = true;

    for (auto it = requests.end(); it != start;) {
        --it;

        if (!first) {
            json << ",";
        }

        first = false;

        const auto timestamp = std::chrono::system_clock::to_time_t(it->timestamp);
        const auto timeInfo = utcTime(timestamp);
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            it->timestamp.time_since_epoch()
        ).count() % 1000;

        std::ostringstream time;

        time << std::put_time(&timeInfo, "%Y-%m-%dT%H:%M:%S")
             << '.' << std::setfill('0') << std::setw(3) << milliseconds
             << 'Z';

        json << "{"
             << "\"time\":\"" << time.str() << "\","
             << "\"method\":\"" << it->method << "\","
             << "\"path\":\"" << it->path << "\","
             << "\"status\": " << it->statusCode << ","
             << "\"duration_ms\": " << it->durationMs
             << "}";
    }

    json << "]";
    return HttpResponse::json(json.str());
}

void RequestHistory::setMaxEntries(std::size_t newMaxEntries) {
    std::lock_guard<std::mutex> lock(mutex);

    maxEntries = newMaxEntries;

    while (requests.size() > maxEntries) {
        requests.pop_front();
    }
}