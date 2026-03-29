#include "Global.h"

#include "Utils.h"

using namespace std;

ostream &operator<<(ostream &os, const fs::path &path) { return os << ToLocaleString(path); }
