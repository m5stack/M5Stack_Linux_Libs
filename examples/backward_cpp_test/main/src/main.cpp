#include<stdio.h>
#include<stdlib.h>
#define BACKWARD_HAS_DW 1
#include "backward.hpp"
#include "backward.h"


int main(){
    char *c = "hello world";
    c[1] = 'H';
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <signal.h>
// #include <fstream>
// #include <iostream>

// #define BACKWARD_HAS_DW 1
// #include "backward.hpp"
// namespace backward
// {
//     backward::SignalHandling sh;
// }

// char *str = (char *)"test";
// void core_test()
// {
//     str[1] = 'T';
// }

// // This is definitely not async-signal-safe. Let's hope it doesn't crash or hang.
// void handler(int signo)
// {
//     if (std::ofstream file_stream("my_stacktrace.log", std::ios::trunc); file_stream.is_open())
//     {
//         file_stream << "Caught signal " << signo << ".\nTrace:\n";
//         backward::StackTrace st;
//         st.load_here(32);
//         backward::Printer p;
//         p.print(st, file_stream);   // print to file
//         p.print(st, std::cout);     // print to console
//     }

//     // Raise signal again. Should usually terminate the program.
//     raise(signo);
// }

// int main()
// {
//     // Repeat this for each signal you want to catch and log.
//     struct sigaction act {};
//     // Allow signal handler to re-raise signal and reset to default handler after entering.
//     act.sa_flags = SA_NODEFER | SA_RESETHAND;
//     act.sa_handler = &handler;
//     sigfillset(&act.sa_mask);
//     sigdelset(&act.sa_mask, SIGSEGV /* or other signal */);
//     if (sigaction(SIGSEGV /* or other signal */, &act, nullptr) == -1)
//         std::exit(EXIT_FAILURE);

//     core_test();
// }
