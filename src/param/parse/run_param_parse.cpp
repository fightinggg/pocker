#include "run_param_parse.hpp"

#include "../../utils/uuid_utils.hpp"
#include "cmdline.hpp"
#include "parse_utils.hpp"

RunParam *RunParamParse::parse(int argc, char *argv[]) {
  cmdline::parser run;

  run.add("interactive", 'i', "Keep STDIN open even if not attached");
  run.add("tty", 't', "Allocate a pseudo-TTY");
  run.add("detach", 'd', "Run container in background and print container ID");

  run.add<string>("memory", 'm', "Memory limit", false, "10m");
  run.add<string>(
      "memory-swap", 0,
      "Swap limit equal to memory plus swap: '-1' to enable unlimited swap",
      false, "10m");

  run.add<string>("name", 0, "Assign a name to the containe", false);

  run.add<double>("cpus", 0, "Number of CPUs", false, 1);

  run.footer("image [image args]");

  run.parse_check(argc, argv);

  auto *runParam = new RunParam();

  runParam->setDetach(run.exist("detach"));
  runParam->setInteractive(run.exist("interactive"));
  runParam->setTty(run.exist("tty"));

  // memory
  string memory = run.get<string>("memory");
  runParam->setMemory(ParseUtils::parseMemory(memory));

  // memory swap
  string memorySwap = run.get<string>("memory-swap");
  runParam->setMemorySwap(ParseUtils::parseMemory(memorySwap));

  // cpu
  double cpus = run.get<double>("cpus");
  runParam->setCpus(cpus);

  // name
  string name = run.get<string>("name");
  runParam->setContainerName(name);

  // id
  runParam->setContainerId(uuid::generate_uuid_v4());

  vector<string> rest = run.rest();

  // rest[0] = 'run'
  if (rest.size() == 1) {
    cerr << "you should add image name" << endl;
    exit(-1);
  }
  runParam->setImage(rest[1]);

  rest.erase(rest.begin());
  rest.erase(rest.begin());
  runParam->setExec(rest);

  cout << runParam->tostring() << endl;

  return runParam;
}
