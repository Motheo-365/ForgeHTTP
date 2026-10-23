#include "Observability/RequestHistory.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

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

    if (event.type != ServerEventType::ResponseSent &&
        event.type != ServerEventType::RequestFailed) {
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

    nlohmann::json result = nlohmann::json::array();
    auto start = requests.end() - static_cast<std::ptrdiff_t>(limit);
    for (auto it = requests.end(); it != start;) {
        --it;

        const auto timestamp = std::chrono::system_clock::to_time_t(it->timestamp);
        const auto timeInfo = utcTime(timestamp);
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            it->timestamp.time_since_epoch()
        ).count() % 1000;

        std::ostringstream time;

        time << std::put_time(&timeInfo, "%Y-%m-%dT%H:%M:%S")
             << '.' << std::setfill('0') << std::setw(3) << milliseconds
             << 'Z';

        result.push_back({
            {"time", time.str()},
            {"method", it->method},
            {"path", it->path},
            {"status", it->statusCode},
            {"duration_ms", it->durationMs}
        });
    }

    return HttpResponse::json(result.dump());
}

void RequestHistory::setMaxEntries(std::size_t newMaxEntries) {
    std::lock_guard<std::mutex> lock(mutex);

    maxEntries = newMaxEntries;

    while (requests.size() > maxEntries) {
        requests.pop_front();
    }
}