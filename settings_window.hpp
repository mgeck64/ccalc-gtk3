#ifndef SETTINGS_WINDOW_HPP
#define SETTINGS_WINDOW_HPP

#include "ccalc/calc_args.hpp"

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/switch.h>
#include <gtkmm/grid.h>
#include <gtkmm/spinbutton.h>

class main_window;
class gcalc_app;

class settings_window: public Gtk::Window {
public:
    settings_window(gcalc_app& app, main_window& main_win);
    ~settings_window();

    auto update_from(
        const parser_options& parse_options,
        const parser_options& new_parse_options,
        const output_options& out_options,
        const output_options& new_out_options,
        bool force = false
        ) -> void;

private:
    gcalc_app& app;
    main_window& main_win;
    Gtk::Box win_vbox;
    Gtk::Box content_vbox;
    Gtk::Box input_defaults_vbox;
    Gtk::Box input_defaults_hbox;
    Gtk::Label input_defaults_lbl;
    Gtk::ComboBoxText default_number_type_code_cb;
    Gtk::ComboBoxText default_number_radix_cb;
    Gtk::Grid settings_grid;
    Gtk::Label output_base_lbl;
    Gtk::ComboBoxText output_radix_cb;
    Gtk::Label output_fp_normalized_lbl;
    Gtk::Switch output_fp_normalized_switch;
    Gtk::Label precision_lbl;
    Gtk::SpinButton precision_entry;
    Gtk::Label integer_word_size_lbl;
    Gtk::ComboBoxText int_word_size_cb;
    Gtk::Box bottom_hbox;
    Gtk::Button help_btn;
    Gtk::Button defaults_btn;
    Gtk::Button cancel_btn;

    auto on_setting_changed() -> void;
    bool suppress_on_setting_changed = false;
    class temporarily_suppress_on_setting_changed {
        bool& suppress_on_setting_changed;
    public:
        temporarily_suppress_on_setting_changed(settings_window& w)
            : suppress_on_setting_changed(w.suppress_on_setting_changed)
            {suppress_on_setting_changed = true;}
        ~temporarily_suppress_on_setting_changed() 
            {suppress_on_setting_changed = false;}
    };

    auto on_help_btn_clicked() -> void;
    auto on_defaults_btn_clicked() -> void;
};

#endif // SETTINGS_WINDOW_HPP