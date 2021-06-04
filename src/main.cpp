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
#include <vector>

#include "./param/bean/run_param.hpp"
#include "./param/parse/param_parse.hpp"
#include "utils/string_utils.hpp"

using namespace std;
#define FIBER_STACK 8192

string getDirectory() {
  char abs_path[1024];
  //获取可执行程序的绝对路径
  int cnt = readlink("/proc/self/exe", abs_path, 1024);
  if (cnt < 0 || cnt >= 1024) {
    return NULL;
  }

  //最后一个'/' 后面是可执行程序名，去掉devel/lib/m100/exe，只保留前面部分路径
  for (int i = cnt; i >= 0; --i) {
    if (abs_path[i] == '/') {
      abs_path[i] = '\0';
      break;
    }
  }
  for (int i = cnt; i >= 0; --i) {
    if (abs_path[i] == '/') {
      abs_path[i] = '\0';
      break;
    }
  }
  return string(abs_path);
}

string appDir = getDirectory();
string imagesDir = appDir + "/data/images";
string busyBoxDir = imagesDir + "/busybox";
string containerDir = appDir + "/data/containers";

string getErr() {
  char s[100];
  sprintf(s, "(%d)%s", errno, strerror(errno));
  return string(s);
}

void prepareContainer(run_param *runParam) {
  // create cpu subsystem
  char cmd[128];
  vector<string> cmdList = {"cd /sys/fs/cgroup/cpu ", "&& mkdir %s ",
                            "&& cd %s ", "&& echo %d > cpu.cfs_quota_us ",
                            "&& echo 50000 > cpu.cfs_period_us "};
  sprintf(cmd, string_utils::join(cmdList, " ").data(),
          runParam->id.data(), runParam->id.data(),
          int(runParam->cpus * 50000));
  system(cmd);

  // create memory subsystem
  cmdList = {"cd /sys/fs/cgroup/memory ", "&& mkdir %s ", "&& cd %s ",
             "&& echo %d > memory.limit_in_bytes ",
             "&& echo %d > memory.memsw.limit_in_bytes "};
  sprintf(cmd, string_utils::join(cmdList, " ").data(),
          runParam->id.data(), runParam->id.data(),
          int(runParam->memory), int(runParam->memory_swap));
  system(cmd);
}

int doContainer(void *arg_param) {
  auto *runParam = (run_param *)arg_param;

  // mount proc system, then container could not find others process
  // CLONE_NEWUSER TODO BUG??
  if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL)) {
    cerr << "mount proc error: " << getErr() << endl;
    exit(-1);
  }

  // use cpu subsystem
  char cmd[128];
  sprintf(cmd, "echo %d >> /sys/fs/cgroup/cpu/%s/tasks", getpid(),
          runParam->id.data());
  system(cmd);

  // use memory subsysem
  sprintf(cmd, "echo %d >> /sys/fs/cgroup/memory/%s/tasks", getpid(),
          runParam->id.data());
  system(cmd);

  string containerDataDir = containerDir + "/" + runParam->id;
  if (mkdir(containerDataDir.data(), 0777)) {
    cerr << " mkdir " << containerDataDir << " error" << endl;
    exit(-1);
  }

  string diskFile = containerDataDir + "/disk.img";
  string containerDisk = containerDataDir + "/disk";

  // create container disk 1. create disk
  sprintf(cmd, "dd if=/dev/zero bs=1M count=20 of=%s", diskFile.data());
  system(cmd);

  // 2. init file system
  if (system(("mkfs.ext4 " + diskFile).data())) {
    cerr << "mkfs error " << diskFile << endl;
    exit(-1);
  }

  mkdir(containerDisk.data(), 0777);

  // 3. mount container disk system
  sprintf(cmd, "mount %s %s", diskFile.data(), containerDisk.data());
  printf("mount upper: %s\n", cmd);
  if (system(cmd)) {
    cerr << "mount " << diskFile << " error" << endl;
    exit(-1);
  }

  string containerUpper = containerDisk + "/upper";
  string containerTmp = containerDisk + "/tmp";
  string containerMerge = containerDisk + "/overlay";

  mkdir(containerUpper.data(), 0777);
  mkdir(containerTmp.data(), 0777);
  mkdir(containerMerge.data(), 0777);

  // system(("ls -al " + containerDisk).data());

  // 4. build overlay
  sprintf(cmd,
          "mount -t overlay overlay "
          "-olowerdir=%s,upperdir=%s,workdir=%s %s",
          busyBoxDir.data(), containerUpper.data(), containerTmp.data(),
          containerMerge.data());
  printf("do: %s\n", cmd);
  if (system(cmd)) {
    cerr << "build overlay error " << getErr() << endl;
    exit(-1);
  }

  // mount busyBox 1. change workdir
  if (chdir(containerMerge.data())) {
    cerr << "chdir to containerMerge error" << getErr() << endl;
    exit(-1);
  }
  // mount busyBox 2. mount busyBox
  // if (mount(busyBoxDir.data(), busyBoxDir.data(), "bind", MS_BIND | MS_REC,
  //           NULL)) {
  //   cerr << "mount busyBox error" << getErr() << endl;
  //   exit(-1);
  // }
  string privotRootName = ".pivot_root" + runParam->id;
  string privotRoot = containerMerge + "/" + privotRootName;
  // mount busyBox 3. mkdir put_old
  if (mkdir(privotRoot.data(), 0777)) {
    cerr << "mkdir privotRoot error" << getErr() << endl;
    exit(-1);
  }
  // mount busyBox 4. privot_root()
  if (syscall(SYS_pivot_root, containerMerge.data(), privotRoot.data())) {
    cerr << "privot_root error" << getErr() << endl;
    exit(-1);
  }
  // mount busyBox 5. to dir /
  if (chdir("/")) {
    cerr << "chdir to / error" << getErr() << endl;
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

  // mount proc system, then container could not find others process
  // CLONE_NEWUSER TODO BUG??
  if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL)) {
    cerr << "mount proc error: " << getErr() << endl;
    exit(-1);
  }

  if (runParam->image == "busybox") {
    vector<string> v = runParam->exec;
    cout << "container begin: " << string_utils::join(v, " ") << endl;
    return execl("/bin/sh", "sh", "-c", string_utils::join(v, " ").data(), NULL);
  } else {
    cerr << "could not find image '" << runParam->image
         << "', you can use image 'busybox'" << endl;
    exit(-1);
  }
}

int main(int argc, char *argv[]) {
  param *arg_param = param_parse::parse(argc, argv);

  void *stack = malloc(FIBER_STACK);  //为子进程申请系统堆栈

  int containerFlag = SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS |
                      CLONE_NEWNET | CLONE_NEWIPC;  // CLONE_NEWUSER

  prepareContainer((run_param *)arg_param);
  int pid = clone(doContainer, (char *)stack + FIBER_STACK, containerFlag,
                  arg_param);  //创建子线程
  int sonStatus;
  if (waitpid(pid, &sonStatus, 0) != pid) {
    cerr << "container error " << getErr() << endl;
    exit(-1);
  }
  if (WIFEXITED(sonStatus)) {
    printf("container exited, status=%d, ", WEXITSTATUS(sonStatus));
  } else if (WIFSIGNALED(sonStatus)) {
    printf("container killed by signal %d, ", WTERMSIG(sonStatus));
  } else if (WIFSTOPPED(sonStatus)) {
    printf("container stopped by signal %d, ", WSTOPSIG(sonStatus));
  } else if (WIFCONTINUED(sonStatus)) {
    printf("container continued, ");
  }
  cout << " thanks for using pocker" << endl;
}
