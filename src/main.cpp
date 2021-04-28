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

void prepareContainer(RunParam *runParam) {
    // create cpu subsystem
    char cmd[128];
    string cmdList[] = {
            "cd /sys/fs/cgroup/cpu ",
            "&& mkdir %s ",
            "&& cd %s ",
            "&& echo %d > cpu.cfs_quota_us ",
            "&& echo 50000 > cpu.cfs_period_us "
    };
    string cmdOrigin;
    for (string &s:cmdList) {
        cmdOrigin += s;
    }
    sprintf(cmd, cmdOrigin.data(),
            runParam->getContainerId().data(),
            runParam->getContainerId().data(),
            int(runParam->getCpus() * 50000)
    );
    system(cmd);
}

int doContainer(void *param) {


    auto *runParam = (RunParam *) param;


    // add cpu subsystem
    char cmd[128];
    sprintf(cmd, "echo %d >> /sys/fs/cgroup/cpu/%s/tasks",
            getpid(), runParam->getContainerId().data());
    system(cmd);

    int cur = time(0);
    int a = 1;
    for (int i = 0; i < 1e9; i++) {
        a += i;
    }
    cout << a << endl;
    cout << "cost: " << time(0) - cur << endl;


    // mount proc system
    if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL)) {
        cerr << "mount proc error" << endl;
        exit(-1);
    }


    if (runParam->getImage() == "centos") {
        vector<string> v = runParam->getExec();

        switch (v.size()) {
            case 0:
                cerr << "no params ???" << endl;
                exit(-1);
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
                exit(-1);
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
                        | CLONE_NEWNS | CLONE_NEWNET
                        | CLONE_NEWIPC | CLONE_NEWUSER;

    prepareContainer((RunParam *) param);
    int pid = clone(doContainer, (char *) stack + FIBER_STACK, containerFlag, param);//创建子线程
    waitpid(pid, NULL, 0);
    cout << "parent exit " << endl;
}
