#include "cpptools/reactor/reactor.h"

int main(int argc, char *argv[])
{
    cpptools::reactor::ReactorServer server("0.0.0.0", 9999);
    server.start();
}
