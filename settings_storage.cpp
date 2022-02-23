#include "settings_storage.hpp"
#include "gcalc_basics.hpp"
#include <filesystem>

namespace pt = boost::property_tree;

settings_storage::settings_storage(Gtk::Window &parent_win) :
    error_msg(parent_win, "", /*use_markup*/ false, Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_OK, /*modal*/ true)
{
    //FOO error_msg.set_hide_on_close(true);
    error_msg.signal_response().connect(sigc::hide(sigc::mem_fun(error_msg, &Gtk::Widget::hide)));
}

static auto file_path() -> std::string {
    std::string path;
    if (auto config_home = getenv("XDG_CONFIG_HOME"))
        path = config_home;
    else if (auto home = getenv("HOME")) {
        path = home;
        if (!path.empty())
            path += "/.config";
    }
    if (path.empty())
        throw pt::ptree_error(
            "Unable to determine the settings file's location; set XDG_CONFIG_HOME or HOME environment variable");
    return path;
}

static inline auto append_filename_to(std::string &path) -> std::string& {
    path += "/";
    path += app_id;
    path += ".conf";
    return path;
}

static auto to_radix(std::string_view str) -> calc_val::radices {
    if (str == "base2")
        return calc_val::base2;
    if (str == "base8")
        return calc_val::base8;
    if (str == "base10")
        return calc_val::base10;
    if (str == "base16")
        return calc_val::base16;
    throw pt::ptree_bad_data("Bad radix value in file", str);
}

static auto to_str(calc_val::radices radix) -> const char* {
    switch (radix) {
    case calc_val::base2:
        return "base2";
    case calc_val::base8:
        return "base8";
    case calc_val::base10:
        return "base10";
    case calc_val::base16:
        return "base16";
    default:
        assert(false);
        throw pt::ptree_error("Internal error"); // missing case(s)
    }
}

static auto to_number_type_code(std::string_view str) -> calc_val::number_type_codes {
    if (str == "complex_code")
        return calc_val::complex_code;
    if (str == "uint_code")
        return calc_val::uint_code;
    if (str == "int_code")
        return calc_val::int_code;
    throw pt::ptree_bad_data("Bad number_type_code value in file", str);
}

static auto to_str(calc_val::number_type_codes code) -> const char* {
    switch (code) {
    case calc_val::complex_code:
        return "complex_code";
    case calc_val::uint_code:
        return "uint_code";
    case calc_val::int_code:
        return "int_code";
    default:
        assert(false);
        throw pt::ptree_error("Internal error"); // missing case(s)
    }
}

static auto to_int_word_size(std::string_view str) -> calc_val::int_word_sizes {
    if (str == "int_bits_8")
        return calc_val::int_bits_8;
    if (str == "int_bits_16")
        return calc_val::int_bits_16;
    if (str == "int_bits_32")
        return calc_val::int_bits_32;
    if (str == "int_bits_64")
        return calc_val::int_bits_64;
    if (str == "int_bits_128")
        return calc_val::int_bits_128;
    throw pt::ptree_bad_data("Bad int_word_size value in file", str);
}

static auto to_precision(std::string_view str) -> unsigned {
    try {
        auto i = boost::lexical_cast<int>(str);
        if (i < 0)
            throw pt::ptree_bad_data("Bad precision value in file", str);
        return i;
    } catch (const boost::bad_lexical_cast&) {
        throw pt::ptree_bad_data("Bad precision value in file", str);
    }
}

static auto to_str(calc_val::int_word_sizes code) -> const char* {
    switch (code) {
    case calc_val::int_bits_8:
        return "int_bits_8";
    case calc_val::int_bits_16:
        return "int_bits_16";
    case calc_val::int_bits_32:
        return "int_bits_32";
    case calc_val::int_bits_64:
        return "int_bits_64";
    case calc_val::int_bits_128:
        return "int_bits_128";
    default:
        assert(false);
        throw pt::ptree_error("Internal error"); // missing case(s)
    }
}

auto settings_storage::show_err_msg(const pt::ptree_error& e, const char *msg, const std::string &file_pathname) -> void {
    error_msg.set_message(msg);
    Glib::ustring secondary;
    secondary.reserve(256);
    if (!file_pathname.empty()) {
        secondary += file_pathname;
        secondary += "\n";
    }
    secondary += e.what();
    error_msg.set_secondary_text(secondary);
    error_msg.show();
};

auto settings_storage::load(parser_options &parse_options, output_options &out_options) -> void {
    std::string file_pathname;
    try {
        file_pathname = file_path();
        append_filename_to(file_pathname);

        if (!std::filesystem::exists(std::filesystem::path(file_pathname)))
            return;

        pt::ptree tree;
        pt::read_ini(file_pathname, tree);

        std::string str;
        str.reserve(32);

        for (auto node1 : tree) {
            if (node1.first == "parser_options") {
                for (auto node2 : node1.second) {
                    if (node2.first == "default_number_radix" && node2.second.empty())
                        parse_options.default_number_radix = to_radix(node2.second.data());
                    else if (node2.first == "default_number_type_code" && node2.second.empty())
                        parse_options.default_number_type_code = to_number_type_code(node2.second.data());
                    else if (node2.first == "int_word_size" && node2.second.empty())
                        parse_options.int_word_size = to_int_word_size(node2.second.data());
                }
            } else if (node1.first == "output_options") {
                for (auto node2 : node1.second) {
                    if (node2.first == "output_fp_normalized" && node2.second.empty())
                        out_options.output_fp_normalized = node2.second.get_value<bool>();
                    else if (node2.first == "output_radix" && node2.second.empty())
                        out_options.output_radix = to_radix(node2.second.data());
                    else if (node2.first == "precision" && node2.second.empty())
                        out_options.precision = to_precision(node2.second.data());
                }
            }
        }
    } catch (const pt::ptree_bad_data& e) {
        show_err_msg(e, "The settings file has bad data", file_pathname);
    } catch (const pt::ptree_error& e) {
        show_err_msg(e, "Error loading the settings file", file_pathname);
    }
}

auto settings_storage::save(const parser_options &parse_options, const output_options &out_options) -> void {
    std::string file_pathname;
    try {
        pt::ptree tree;

        tree.put("parser_options.default_number_radix", to_str(parse_options.default_number_radix));
        tree.put("parser_options.default_number_type_code", to_str(parse_options.default_number_type_code));
        tree.put("parser_options.int_word_size", to_str(parse_options.int_word_size));

        tree.put("output_options.output_fp_normalized", out_options.output_fp_normalized);
        tree.put("output_options.output_radix", to_str(out_options.output_radix));
        tree.put("output_options.precision", out_options.precision);

        file_pathname = file_path();
        std::filesystem::create_directories(std::filesystem::path(file_pathname));
        pt::write_ini(append_filename_to(file_pathname), tree);
    } catch (const pt::ptree_error& e) {
        show_err_msg(e, "Error saving the settings file", file_pathname);
    }
}
