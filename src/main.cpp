#include <sched.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <sys/mount.h>
#include "./param/parse/ParamParse.hpp"
#include "./param/bean/RunParam.hpp"

using namespace std;
#define FIBER_STACK 8192

int doContainer(void *param) {
    auto *runParam = (RunParam *) param;

    if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL)) {
        cerr << "mount proc error" << endl;
        exit(-1);
    }
    if (runParam->getImage() == "centos") {
        vector<string> v = runParam->getExec();

        switch (v.size()) {
            case 1:
                execlp(v[0].data(), nullptr);
                break;
            case 2:
                execlp(v[0].data(), v[1].data(), NULL);
                break;
            case 3:
                execlp(v[0].data(), v[1].data(), v[2].data(), NULL);
                break;
            case 4:
                execlp(v[0].data(), v[1].data(), v[2].data(), v[3].data(), NULL);
                break;
            default:
                cerr << "too many params, only four params support" << endl;
                exit(0);
        }
        cerr << "exec error: " << endl;
        exit(-1);
    } else {
        cerr << "could not find image '" << runParam->getImage() << "'" << endl;
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    Param *param = ParamParse::parse(argc, argv);

    void *stack = malloc(FIBER_STACK);//为子进程申请系统堆栈


    int containerFlag = SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID
                        | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWUSER;
    int pid = clone(doContainer, (char *) stack + FIBER_STACK, containerFlag, param);//创建子线程
    waitpid(pid, NULL, 0);
    cout << "parent exit " << endl;
}
