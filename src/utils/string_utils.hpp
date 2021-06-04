
class string_utils {
 public:
  static string join(const vector<string>& strings, string split) {
    string res;
    for (int i = 0; i < strings.size(); i++) {
      res += strings[i];
      if (i != 0) res += split;
    }
    return res;
  }
};
