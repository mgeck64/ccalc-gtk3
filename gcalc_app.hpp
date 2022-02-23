#ifndef GCALC_APP_HPP
#define GCALC_APP_HPP

#include "main_window.hpp"

#include <gtkmm/application.h>
#include <gtkmm/window.h>

class help_window;

class gcalc_app: public Gtk::Application {
public:
    static auto create() -> Glib::RefPtr<gcalc_app>;
 
    auto help(Gtk::Window* invoker, int topic_idx, bool force_topic) -> void;
    auto help_invoker() const -> Gtk::Window* {return help_invoker_;}
    auto close_help() -> void;

private:
    main_window main_win;
    std::unique_ptr<help_window> help_win;

    Gtk::Window* help_invoker_ = 0;

    gcalc_app();
    auto on_activate() -> void;
    auto on_help_hide() -> void;
};

#endif // GCALC_APP_HPP