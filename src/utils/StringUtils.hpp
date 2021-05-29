#include "../format/String.hpp"
#include "../format/Vector.hpp"

class StringUtils {
 public:
  static String join(const Vector<String>&strings) {
    String res;
    for (String s : strings) {
      res += s;
    }
    return res;
  }
};
