
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

  static vector<string> split(const string s, const char split) {
    vector<string> res;
    res.push_back("");
    for (char ch : s) {
      if (ch == split) {
        res.push_back("");
      } else {
        res.back().push_back(ch);
      }
    }
    return res;
  }
};
