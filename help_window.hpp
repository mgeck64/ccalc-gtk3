#ifndef HELP_WINDOW_HPP
#define HELP_WINDOW_HPP

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>

class help_window : public Gtk::Window {
public:
    enum topic_idxs {quick_start_idx, representation_types_and_numeric_bases_idx,
        input_output_information_area_idx, scientific_notation_idx, prefixes_idx,
        implied_multiplication_idx, functions_idx, bitwise_operators_idx,
        variables_idx, settings_window_idx, settings_textual_idx, num_topics};

    help_window(int topic_idx);
    auto show_topic(int topic_idx) -> void;

private:
    Gtk::Box vbox;
    Gtk::Box top_hbox;
    Gtk::ComboBoxText topics_cb;
    Gtk::Button prev_topic_btn;
    Gtk::Button next_topic_btn;

    Gtk::ScrolledWindow quick_start_swin;
    Gtk::ScrolledWindow representation_types_and_numeric_bases_swin;
    Gtk::ScrolledWindow input_output_information_area_swin;
    Gtk::ScrolledWindow scientific_notation_swin;
    Gtk::ScrolledWindow prefixes_swin;
    Gtk::ScrolledWindow implied_multiplication_swin;
    Gtk::ScrolledWindow functions_swin;
    Gtk::ScrolledWindow bitwise_operators_swin;
    Gtk::ScrolledWindow variables_swin;
    Gtk::ScrolledWindow settings_window_swin;
    Gtk::ScrolledWindow settings_textual_swin;
    Gtk::ScrolledWindow num_topics_swin;

    Gtk::Label quick_start_lbl;
    Gtk::Label representation_types_and_numeric_bases_lbl;
    Gtk::Label input_output_information_area_lbl;
    Gtk::Label scientific_notation_lbl;
    Gtk::Label prefixes_lbl;
    Gtk::Label implied_multiplication_lbl;
    Gtk::Label functions_lbl;
    Gtk::Label bitwise_operators_lbl;
    Gtk::Label variables_lbl;
    Gtk::Label settings_window_lbl;
    Gtk::Label settings_textual_lbl;
    Gtk::Label num_topics_lbl;

    int last_topic_idx = -1;

    auto on_topics_changed() -> void;
    auto on_prev_topic_clicked() -> void;
    auto on_next_topic_clicked() -> void;
};

#endif // HELP_WINDOW_HPP