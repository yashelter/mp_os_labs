#include <gtest/gtest.h>

#include <big_int.h>
#include <client_logger.h>
#include <client_logger_builder.h>
#include <operation_not_supported.h>

logger *create_logger(
    std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
    bool use_console_stream = true,
    logger::severity console_stream_severity = logger::severity::debug)
{
    logger_builder *builder = new client_logger_builder();
    
    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }
    
    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }
    
    logger *built_logger = builder->build();
    
    delete builder;
    
    return built_logger;
}



int main(
    int argc,
    char **argv)
{
    //testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();

    big_int a({0, 1, 2, 3});
	big_int b({1, 2, 3});
	auto res = a.compare(b, 1);

	if (res == std::strong_ordering::equal) {
		std::cout << "a is equal to b" << std::endl;
	} else if (res == std::strong_ordering::greater) {
		std::cout << "a is greater than b" << std::endl;
	} else {
		std::cout << "a is less than b" << std::endl;
	}
}