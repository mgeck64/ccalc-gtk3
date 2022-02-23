#ifndef VARIABLES_DIALOG_HPP
#define VARIABLES_DIALOG_HPP

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>

#include "ccalc/calc_parser.hpp"
#include "ccalc/calc_args.hpp"

class variables_window : public Gtk::Window {
public:
    variables_window();

    void set(calc_parser::variables_itr begin, calc_parser::variables_itr end, const output_options& out_options);

private:
    Gtk::ScrolledWindow variables_scroller;
    Gtk::Label variables_lbl;
};

#endif // VARIABLES_DIALOG_HPP