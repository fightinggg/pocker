#pragma once

#include <string>
#include <ostream>
#include <vector>
#include "./Param.hpp"

using namespace std;

class RunParam : public Param {
private:
    bool tty;
    bool interactive;
    bool detach;
    string image;
    std::vector<string> exec;
public:
    bool isTty() const {
        return tty;
    }

    void setTty(bool tty) {
        RunParam::tty = tty;
    }

    bool isInteractive() const {
        return interactive;
    }

    void setInteractive(bool interactive) {
        RunParam::interactive = interactive;
    }

    bool isDetach() const {
        return detach;
    }

    void setDetach(bool detach) {
        RunParam::detach = detach;
    }

    const string &getImage() const {
        return image;
    }

    void setImage(const string &image) {
        RunParam::image = image;
    }

    const vector<string> &getExec() const {
        return exec;
    }

    void setExec(const vector<string> &exec) {
        RunParam::exec = exec;
    }

    friend ostream &operator<<(ostream &os, const RunParam &param) {
        os << "tty: " << param.tty << " interactive: " << param.interactive << " detach: " << param.detach << " image: "
           << param.image << " execSize: " << param.exec.size();
        return os;
    }
};