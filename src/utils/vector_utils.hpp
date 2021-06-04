#include <string>
#include <vector>
using namespace std;

class vector_utils {
 public:
  template <class T>
  static string to_string(const vector<T>& array) {
    string res;
    for (auto& x : array) {
      res += x.to_string();
    }
    return "[ " + res + "]";
  }
};
