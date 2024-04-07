#include <not_implemented.h>

#include "../include/client_logger_builder.h"
#include <filesystem>

client_logger_builder::client_logger_builder()
{
   
}

client_logger_builder::client_logger_builder(
    client_logger_builder const &other)
{
    throw not_implemented("logger_build)", "your code should be here...");
}

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder const &other)
{
    throw not_implemented("logger_build)", "your code should be here...");
}

client_logger_builder::client_logger_builder(
    client_logger_builder&& other) noexcept
{
    throw not_implemented("logger_build)", "your code should be here...");
}

client_logger_builder &client_logger_builder::operator=(
    client_logger_builder &&other) noexcept
{
    throw not_implemented("logger_build)", "your code should be here...");
  
}

client_logger_builder::~client_logger_builder() noexcept
{
    clear();
}

logger_builder *client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity)
{
    auto iter = subscriptions.find(severity);
    if (iter == subscriptions.end())
    {
        auto answ = subscriptions.emplace(severity, client_logger::logger_subscription());
        if (!answ.second)
        {
            throw std::ios_base::failure("problem with emplace in hash_map\n");
        }
        iter = answ.first;
    }
    iter->second._paths.push_back(std::filesystem::weakly_canonical(stream_file_path).string());
    return this;
}

logger_builder *client_logger_builder::add_console_stream(
    logger::severity severity)
{
    auto iter = subscriptions.find(severity);
    if (iter == subscriptions.end())
    {
        auto answ = subscriptions.emplace(severity, client_logger::logger_subscription());
        if (!answ.second)
        {
            throw std::ios_base::failure("problem with emplace in hash_map\n");
        }
        iter = answ.first;
    }
    iter->second._console_stream = true;
    return this;
}

logger_builder* client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path)
{
    throw not_implemented("logger_builder* client_logger_builder::transform_with_configuration(std::string const &configuration_file_path, std::string const &configuration_path)", "your code should be here...");
}

logger_builder *client_logger_builder::clear()
{
    subscriptions.clear();
    return this;
}

client_logger *client_logger_builder::build() const
{
    return new client_logger(subscriptions);
}