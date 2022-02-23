#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ccalc/calc_parser.hpp"
#include "settings_storage.hpp"

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/aboutdialog.h>
#include <gdk/gdkevents.h>

#include <vector>

class gcalc_app;
class variables_window;
class settings_window;

class main_window : public Gtk::Window {
public:
    main_window(gcalc_app& app);

    std::tuple<parser_options, output_options> options() const
    {return std::make_tuple(parser.options(), out_options);}

    auto options(const parser_options& parse_options, const output_options& out_options_) -> void;
 
private:
    gcalc_app& app;
    Gtk::HeaderBar header_bar;
    Gtk::Box clear_result_hbox;
    Gtk::Button clear_result_btn;
    Gtk::Label clear_result_lbl;
    Gtk::MenuButton more_btn;
    Gtk::Box win_vbox;
    Gtk::Box content_vbox;
    Gtk::Box expr_hbox;
    Gtk::Entry expr_entry;
    Gtk::Button expr_btn;
    Gtk::Label expr_btn_lbl;
    Gtk::Frame result_frame;
    Gtk::Box result_vbox;
    Gtk::Label result_lbl;
    Gtk::Box in_out_info_hbox;
    Gtk::Label in_info_lbl;
    Gtk::Label out_info_lbl;
    Gtk::Box menus_hbox;
    Gtk::MenuButton functions_a_btn;
    Gtk::MenuButton functions_b_btn;
    Gtk::Button settings_btn;

    settings_storage settings_storager;

    auto show_input_info() -> void;
    auto show_output_info() -> void;
    auto append_history(const Glib::ustring& expr_str) -> void;
    auto recall_history(bool direction_up) -> void;
    auto on_clear_result_btn_clicked() -> void;
    auto on_expr_entry_key_pressed(GdkEventKey* key) -> bool;
    auto on_expr_btn_clicked() -> void;
    auto on_function_action(const char* label) -> void;
    auto on_settings_btn_clicked() -> void;
    auto on_variables_btn_clicked() -> void;
    auto on_help_btn_clicked() -> void;
    auto on_about_btn_clicked() -> void;
    auto on_variables_changed() -> void;

    parser_options parse_options;
    output_options out_options;
    calc_parser parser;
    enum last_result_kinds {none_kind, value_kind, parse_error_kind} last_result_kind = none_kind;
    auto evaluate() -> void;
    auto update_if_options_changed(const output_options& new_out_options) -> void;

    using history_list = std::vector<Glib::ustring>;
    history_list history;
    history_list::size_type history_idx = 0;
    static constexpr history_list::size_type max_history_size = 32;

    std::unique_ptr<variables_window> variables_win;
    std::unique_ptr<settings_window> settings_win;
    std::unique_ptr<Gtk::AboutDialog> about_dlg;

    auto on_hide() -> void;
    auto on_client_hide(Gtk::Window* win) -> void;
};

#endif // MAIN_WINDOW_HPP