#include <string>

class obj_utils {
 public:
  static int string_to_int(std::string s) {
    int res = 0;
    for (int i = 0; i < s.size(); i++) {
      res = res * 10 + s[i] - '0';
    }
    return res;
  }
};
