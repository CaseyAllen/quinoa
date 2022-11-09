#include "./error.h"
#include "./logger.h"
using namespace std;
void print_trace()
{
    char pid_buf[30];
    sprintf(pid_buf, "%d", getpid());
    char name_buf[512];
    name_buf[readlink("/proc/self/exe", name_buf, 511)] = 0;
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
    int child_pid = fork();
    if(!child_pid) {
	execl("/usr/bin/gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
	abort(); /* If gdb failed to start */
    } else {
	waitpid(child_pid, NULL, 0);
    }
}
void error(string reason, bool trace)
{
    Logger::enqueueMode(false);
    Logger::printQueue();
    Logger::error(reason);
    if(trace)
	print_trace();
    exit(1);
}
#define X(ename) { E_##ename, #ename },

std::map<ErrorType, std::string> error_names = { ERR_TYPES };

#undef X
[[noreturn]]void except(ErrorType err, std::string message)
{

    if(err == E_INTERNAL && DEBUG_MODE)
	print_trace();
    auto errname = error_names[err];
    Logger::printQueue();
    Logger::enqueueMode(false);
    Logger::error(errname + " - " + message);
    exit(100 + (int)err);
}
