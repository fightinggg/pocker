#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "./param/bean/RunParam.hpp"
#include "./param/parse/ParamParse.hpp"
#include "format/String.hpp"
#include "format/Vector.hpp"
#include "utils/StringUtils.hpp"

using namespace std;
#define FIBER_STACK 81920
String busyBoxDir = "/root/src/busybox";

String getErr() {
  char s[100];
  sprintf(s, "(%d)%s", errno, strerror(errno));
  return String(s);
}

void prepareContainer(RunParam *runParam) {
  // create cpu subsystem
  char cmd[128];
  Vector<String> cmdList = {"cd /sys/fs/cgroup/cpu ", "&& mkdir %s ",
                            "&& cd %s ", "&& echo %d > cpu.cfs_quota_us ",
                            "&& echo 50000 > cpu.cfs_period_us "};
  sprintf(cmd, StringUtils::join(cmdList, " ").data(),
          runParam->getContainerId().data(), runParam->getContainerId().data(),
          int(runParam->getCpus() * 50000));
  system(cmd);

  // create memory subsystem
  cmdList = {"cd /sys/fs/cgroup/memory ", "&& mkdir %s ", "&& cd %s ",
             "&& echo %d > memory.limit_in_bytes ",
             "&& echo %d > memory.memsw.limit_in_bytes "};
  sprintf(cmd, StringUtils::join(cmdList, " ").data(),
          runParam->getContainerId().data(), runParam->getContainerId().data(),
          int(runParam->getMemory()), int(runParam->getMemorySwap()));
  system(cmd);
}

int doContainer(void *param) {
  auto *runParam = (RunParam *)param;

  // use cpu subsystem
  char cmd[128];
  sprintf(cmd, "echo %d >> /sys/fs/cgroup/cpu/%s/tasks", getpid(),
          runParam->getContainerId().data());
  system(cmd);

  // use memory subsysem
  sprintf(cmd, "echo %d >> /sys/fs/cgroup/memory/%s/tasks", getpid(),
          runParam->getContainerId().data());
  system(cmd);

  // mount proc system, then container could not find others process
  if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL)) {
    cerr << "mount proc error" << endl;
    exit(-1);
  }

  // mount busyBox 1. change workdir
  if (chdir(busyBoxDir.data())) {
    cerr << "chdir to busyBoxDir error" << endl;
    exit(-1);
  }
  // mount busyBox 2. mount busyBox
  if (mount(busyBoxDir.data(), busyBoxDir.data(), "bind", MS_BIND | MS_REC,
            NULL)) {
    cerr << "mount busyBox error" << endl;
    exit(-1);
  }
  String privotRootName = ".pivot_root" + runParam->getContainerId();
  String privotRoot = busyBoxDir + "/" + privotRootName;
  // mount busyBox 3. mkdir put_old
  if (mkdir(privotRoot.data(), S_IRWXU | S_IRWXG | S_IRWXO)) {
    cerr << "mkdir privotRoot error" << endl;
    exit(-1);
  }
  // mount busyBox 4. privot_root()
  if (syscall(SYS_pivot_root, busyBoxDir.data(), privotRoot.data())) {
    cerr << "privot_root error" << endl;
    exit(-1);
  }
  // mount busyBox 5. to dir /
  if (chdir("/")) {
    cerr << "chdir to / error" << endl;
    exit(-1);
  }
  // mount busyBox 6. unmount .privot_root
  if (umount2(("/" + privotRootName).data(), MNT_DETACH)) {
    cerr << "unmount .pivot_root  error " << getErr() << endl;
    exit(-1);
  }
  // mount busyBox 7. delete dir
  if (rmdir(("/" + privotRootName).data())) {
    cerr << "rm .pivot_root  error " << getErr() << endl;
    exit(-1);
  }

  if (runParam->getImage() == "busybox") {
    vector<String> v = runParam->getExec();
    cout << "container begin: " << StringUtils::join(v, " ") << endl;
    return system(StringUtils::join(v, " ").data());
  } else {
    cerr << "could not find image '" << runParam->getImage()
         << "', you can use image 'busybox'" << endl;
    exit(-1);
  }
}

int main(int argc, char *argv[]) {
  Param *param = ParamParse::parse(argc, argv);

  void *stack = malloc(FIBER_STACK);  //为子进程申请系统堆栈

  int containerFlag = SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
                      CLONE_NEWNET | CLONE_NEWIPC | CLONE_NEWUSER;

  prepareContainer((RunParam *)param);
  int pid = clone(doContainer, (char *)stack + FIBER_STACK, containerFlag,
                  param);  //创建子线程
  int sonStatus;
  if (waitpid(pid, &sonStatus, 0) != pid) {
    cerr << "container error " << getErr() << endl;
    exit(-1);
  }
  if (WIFEXITED(sonStatus)) {
    printf("container exited, status=%d\n", WEXITSTATUS(sonStatus));
  } else if (WIFSIGNALED(sonStatus)) {
    printf("container killed by signal %d\n", WTERMSIG(sonStatus));
  } else if (WIFSTOPPED(sonStatus)) {
    printf("container stopped by signal %d\n", WSTOPSIG(sonStatus));
  } else if (WIFCONTINUED(sonStatus)) {
    printf("container continued\n");
  }
  cout << " thanks for using pocker" << endl;
}
