#include <string>

class ObjUtils {
 public:
  static int stringToInt(std::string s) {
    int res = 0;
    for (int i = 0; i < s.size(); i++) {
      res = res * 10 + s[i] - '0';
    }
    return res;
  }
};
