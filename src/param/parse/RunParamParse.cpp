#include "RunParamParse.hpp"
#include "cmdline.hpp"

RunParam *RunParamParse::parse(int argc, char *argv[]) {
    cmdline::parser run;

    run.add("interactive", 'i', "interactive");
    run.add("tty", 't', "tty");
    run.add("detach", 'd', "detach");
    run.footer("image...");

    run.parse_check(argc, argv);

    auto *runParam = new RunParam();

    runParam->setDetach(run.exist("detach"));
    runParam->setInteractive(run.exist("interactive"));
    runParam->setTty(run.exist("tty"));

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

    return runParam;
}

