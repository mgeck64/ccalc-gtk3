#include "gcalc_basics.hpp"

auto is_blank(const Glib::ustring& str) -> bool {
    for (auto c : str)
        if (!isspace(c))
            return false;
    return true;
}
