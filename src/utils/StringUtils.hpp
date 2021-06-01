#include "../format/String.hpp"
#include "../format/Vector.hpp"

class StringUtils {
 public:
  static String join(const Vector<String>& strings, String split) {
    String res;
    for (int i = 0; i < strings.size(); i++) {
      res += strings[i];
      if (i != 0) res += split;
    }
    return res;
  }
};
