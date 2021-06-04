#include "run_param_parse.hpp"

#include "../../utils/string_utils.hpp"
#include "../../utils/uuid_utils.hpp"
#include "../bean/run_param.hpp"
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
  run.add<string>("volume", 'v', "Bind mount a volume", false, "");

  run.add<double>("cpus", 0, "Number of CPUs", false, 1);

  run.footer("image [image args]");

  run.parse_check(argc, argv);

  auto *argc_run_param = new run_param();

  argc_run_param->detach = run.exist("detach");
  argc_run_param->interactive = run.exist("interactive");
  argc_run_param->tty = run.exist("tty");

  // memory
  string memory = run.get<string>("memory");
  argc_run_param->memory = parse_utils::parse_memory(memory);

  // memory swap
  string memory_swap = run.get<string>("memory-swap");
  argc_run_param->memory_swap = parse_utils::parse_memory(memory_swap);

  // cpu
  argc_run_param->cpus = run.get<double>("cpus");

  // name
  argc_run_param->name = run.get<string>("name");

  // id
  argc_run_param->id = uuid::generate_uuid_v4();

  // volumn
  string volumes_map = run.get<string>("volume");
  if (volumes_map != "") {
    vector<string> opts = string_utils::split(volumes_map, ':');
    if (opts.size() != 2) {
      cerr << " please using '-v /path/to/src:/path/to/target'" << endl;
      exit(-1);
    } else {
      volume_mapping vmp;
      vmp.from = opts[0];
      vmp.to = opts[1];
      argc_run_param->volumes.push_back(vmp);
    }
  }

  vector<string> rest = run.rest();

  // rest[0] = 'run'
  if (rest.size() == 1) {
    cerr << "you should add image name" << endl;
    exit(-1);
  }
  argc_run_param->image = rest[1];

  rest.erase(rest.begin());
  rest.erase(rest.begin());
  argc_run_param->exec = rest;

  cout << argc_run_param->to_string() << endl;

  return argc_run_param;
}
