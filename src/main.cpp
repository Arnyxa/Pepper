#include "context.hpp"
#include "globals.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace krn
{
    namespace log = spdlog;
}

void test()
{
    auto console = spdlog::stdout_color_mt("penis");

    console->set_level(krn::log::level::trace);
    console->info("Penis {} {}", 69, "dicks");
}

int main()
{
    test();
    {
        ppr::Context myContext;
        myContext.Run();
    }

    return EXIT_SUCCESS;
}