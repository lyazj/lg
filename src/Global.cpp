#include "Global.h"

#include "Utils.h"

using namespace std;

ostream &operator<<(ostream &os, const wstring &wstr) { return os << ToLocaleString(wstr); }

ostream &operator<<(ostream &os, const fs::path &path) { return os << ToLocaleString(path); }

ostream &operator<<(ostream &os, const u32string &u32str) { return os << ToWString(u32str); }
