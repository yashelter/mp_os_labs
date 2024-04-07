#include <gtest/gtest.h>
#include <../include/client_logger_builder.h>
#include <../include/client_logger.h>


int main(
    int argc,
    char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    client_logger_builder builder = client_logger_builder();
    
    builder.add_console_stream(logger::severity::trace);

    builder.add_file_stream("out.txt", logger::severity::trace);
    builder.add_file_stream("dfasf", logger::severity::trace);
    builder.add_file_stream("riumuer", logger::severity::warning);

    client_logger *logger = builder.build();
    logger->log("test", logger::severity::trace);
    std::cout << '1';
    return RUN_ALL_TESTS();
}