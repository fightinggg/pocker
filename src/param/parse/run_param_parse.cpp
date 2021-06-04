#include "run_param_parse.hpp"

#include "../../utils/uuid_utils.hpp"
#include "cmdline.hpp"
#include "parse_utils.hpp"

run_param *run_param_parse::parse(int argc, char *argv[]) {
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

  auto *runParam = new run_param();

  runParam->detach = run.exist("detach");
  runParam->interactive = run.exist("interactive");
  runParam->tty = run.exist("tty");

  // memory
  string memory = run.get<string>("memory");
  runParam->memory = parse_utils::parse_memory(memory);

  // memory swap
  string memory_swap = run.get<string>("memory-swap");
  runParam->memory_swap = parse_utils::parse_memory(memory_swap);

  // cpu
  runParam->cpus = run.get<double>("cpus");

  // name
  runParam->name = run.get<string>("name");

  // id
  runParam->id = uuid::generate_uuid_v4();

  vector<string> rest = run.rest();

  // rest[0] = 'run'
  if (rest.size() == 1) {
    cerr << "you should add image name" << endl;
    exit(-1);
  }
  runParam->image = rest[1];

  rest.erase(rest.begin());
  rest.erase(rest.begin());
  runParam->exec = rest;

  cout << runParam->tostring() << endl;

  return runParam;
}
