#include "cpptools/reactor/reactor.h"

int main(int argc, char *argv[])
{
    cpptools::reactor::ReactorServer server("0.0.0.0", 10040);
    server.start();
}
