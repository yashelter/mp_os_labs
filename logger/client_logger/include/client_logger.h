#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include <logger.h>

#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>

class client_logger final:
    public logger
{
public:
    class logger_subscription final
    {
        friend class client_logger_builder;
        friend class client_logger;
    private:
        severity _severity;
        bool _console_stream = false;
        std::vector<std::string> _paths;
    };

private:
    std::unordered_map<severity, logger_subscription> subscriptions;

private:
    static std::unordered_map<std::string, std::pair<std::ofstream, size_t>> _all_streams;


public:
    client_logger(std::unordered_map<severity, logger_subscription>);
    client_logger(
        client_logger const &other);

    client_logger &operator=(
        client_logger const &other);

    client_logger(
        client_logger &&other) noexcept;

    client_logger &operator=(
        client_logger &&other) noexcept;

    ~client_logger() noexcept final;

public:

    [[nodiscard]] logger const *log(
        const std::string &message,
        logger::severity severity) const noexcept override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H