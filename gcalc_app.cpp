#include "gcalc_app.hpp"
#include "gcalc_basics.hpp"
#include "main_window.hpp"
#include "help_window.hpp"
#include "variables_window.hpp"
#include "settings_window.hpp"

#include <glibmm/miscutils.h>

gcalc_app::gcalc_app() :
    Gtk::Application(app_id),
    main_win(*this)
{
    Glib::set_application_name("gcalc");
}

auto gcalc_app::create() -> Glib::RefPtr<gcalc_app> {
    return Glib::RefPtr<gcalc_app>(new gcalc_app());
}

auto gcalc_app::on_activate() -> void {
    add_window(main_win);
    main_win.show();
}

auto gcalc_app::on_help_hide() -> void {
    std::cout << "gcalc_app::on_help_hide" << std::endl;
    if (help_win)
        remove_window(*help_win);
    help_win.reset();
    help_invoker_ = 0;
}

auto gcalc_app::help(Gtk::Window* invoker, int topic_idx, bool force_topic) -> void {
    if (help_win) {
        help_win->present();
        if (force_topic)
            help_win->show_topic(topic_idx);
    } else {
        help_win = std::make_unique<help_window>(topic_idx);
        add_window(*help_win);
        help_win->signal_hide().connect(sigc::mem_fun(*this, &gcalc_app::on_help_hide), false);
        help_win->show();
    }
    if (!help_invoker_ || help_invoker_ != &main_win)
        help_invoker_ = invoker;
}

auto gcalc_app::close_help() -> void {
    if (help_win)
        help_win->close();
}
