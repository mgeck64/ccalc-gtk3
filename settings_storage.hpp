#ifndef SETTINGS_STORAGE_HPP
#define SETTINGS_STORAGE_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <gtkmm/messagedialog.h>

#include "ccalc/calc_args.hpp"

class settings_storage {
private:
    Gtk::MessageDialog error_msg;
    auto show_err_msg(const boost::property_tree::ptree_error& e, const char* msg, const std::string& file_pathname) -> void;

public:
    settings_storage(Gtk::Window& parent_win);
    // note: because this can show a modal error dialog, make this a member of
    // parent_win to make sure the dialog will persist (i.e., don't use this as
    // local variable else dialog won't persist after function returns)

    auto load(parser_options& parse_options, output_options& out_options) -> void;
    // each field in parse_options and out_options is expected to have a
    // suitable default value, which will be retained if the settings file is
    // missing or doesn't contain a value for that field. in the event of an
    // error, the file may be partially loaded, and parse_options and
    // out_options will reflect that; a dialog will report the error to the
    // user but the program will be allowed to continue

    auto save(const parser_options& parse_options, const output_options& out_options) -> void;
};

#endif // SETTINGS_STORAGE_HPP