#include "main_window.hpp"
#include "gcalc_app.hpp"
#include "gcalc_basics.hpp"
#include "help_window.hpp"
#include "settings_window.hpp"

#include "ccalc/calc_parse_error.hpp"
#include "ccalc/calc_outputter.hpp"

#include <giomm/simpleactiongroup.h>

main_window::main_window(gcalc_app& app_) :
    app{app_},
    win_vbox(Gtk::Orientation::ORIENTATION_VERTICAL),
    content_vbox(Gtk::Orientation::ORIENTATION_VERTICAL),
    expr_hbox(Gtk::Orientation::ORIENTATION_HORIZONTAL),
    variables_space(Gtk::Orientation::ORIENTATION_VERTICAL),
    in_out_info_hbox(Gtk::Orientation::ORIENTATION_HORIZONTAL),
    menus_hbox(Gtk::Orientation::ORIENTATION_HORIZONTAL),
    settings_btn("_Settings"),
    settings_storager(*this)
{
    set_titlebar(header_bar);
    header_bar.set_title(app_name);
    header_bar.set_show_close_button(true);

    // this elaboration with using clear_result_hbox and clear_result_lbl is so
    // the button will appear smaller than if the button was packed directly and
    // the label was set directly:
    header_bar.pack_start(clear_result_hbox);
    clear_result_hbox.pack_start(clear_result_btn);
    clear_result_btn.add(clear_result_lbl);
    clear_result_lbl.set_markup("<small>CLR</small>");
    clear_result_btn.set_tooltip_text("Clear Expression and Result");
    clear_result_btn.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_clear_result_btn_clicked));

    {
        auto more_action_group = Gio::SimpleActionGroup::create();
        insert_action_group("more", more_action_group);

        header_bar.pack_start(more_btn);
        more_btn.set_image_from_icon_name("open-menu-symbolic");
        set_margin(more_btn, 0);
        more_btn.set_tooltip_text("More");
        more_btn.set_hexpand(false);

        more_menu = Gio::Menu::create();
        more_btn.set_menu_model(more_menu);

        more_action_group->add_action("variables", sigc::mem_fun(*this, &main_window::on_variables_item_clicked));
        more_menu->append("Show Variables", "more.variables");
        show_variables_position = 0;

        more_action_group->add_action("help", sigc::mem_fun(*this, &main_window::on_help_item_clicked));
        more_menu->append("Help", "more.help");
        app.set_accel_for_action("more.help", "F1");

        more_action_group->add_action("about", sigc::mem_fun(*this, &main_window::on_about_item_clicked));
        more_menu->append("About", "more.about");

        more_btn.show();
    }

    add(win_vbox);

    win_vbox.pack_start(content_vbox);
    set_margin(content_vbox, default_margin);

    content_vbox.pack_start(expr_hbox, Gtk::PackOptions::PACK_SHRINK);

    expr_hbox.pack_start(expr_entry);
    set_margin(expr_entry, default_margin);
    expr_entry.set_vexpand(false);
    expr_entry.signal_activate().connect(sigc::mem_fun(*this, &main_window::on_expr_btn_clicked));
    signal_key_press_event().connect(sigc::mem_fun(*this, &main_window::on_expr_entry_key_pressed), false);

    expr_hbox.pack_start(expr_btn, Gtk::PackOptions::PACK_SHRINK);
    set_margin(expr_btn, default_margin);
    expr_btn.add(expr_btn_lbl);
    expr_btn_lbl.set_markup("<big>=</big>");
    // putting the label in the button this way instead of in the ctor that
    // takes a string causes the button to have smaller width, which we want;
    // also we can use markup in the label text

    expr_btn.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_expr_btn_clicked));

    content_vbox.pack_start(result_frame, Gtk::PackOptions::PACK_SHRINK);
    set_margin(result_frame, default_margin);
    result_frame.add(result_lbl);
    set_margin(result_lbl, default_margin);
    result_lbl.set_halign(Gtk::Align::ALIGN_START);
    result_lbl.set_valign(Gtk::Align::ALIGN_START);
    result_lbl.set_selectable(true);
    result_lbl.set_line_wrap(true);
    result_lbl.set_line_wrap_mode(Pango::WrapMode::WRAP_WORD_CHAR);

    content_vbox.pack_start(variables_space);
    variables_space.pack_start(variables_frame);
    variables_frame.add(variables_scroller);
    variables_scroller.add(variables_lbl);
    variables_scroller.set_min_content_height(100);
    set_margin(variables_frame, default_margin);
    set_margin(variables_lbl, default_margin);
    variables_lbl.set_halign(Gtk::Align::ALIGN_START);
    variables_lbl.set_valign(Gtk::Align::ALIGN_START);
    variables_lbl.set_selectable(true);
    variables_lbl.set_line_wrap(true);
    variables_lbl.set_line_wrap_mode(Pango::WrapMode::WRAP_WORD_CHAR);

    content_vbox.pack_start(in_out_info_hbox, Gtk::PackOptions::PACK_SHRINK);
    in_out_info_hbox.pack_start(in_info_lbl, Gtk::PackOptions::PACK_SHRINK);
    in_out_info_hbox.pack_end(out_info_lbl, Gtk::PackOptions::PACK_SHRINK);
    set_margin(in_info_lbl, default_margin);
    in_info_lbl.set_halign(Gtk::Align::ALIGN_START);
    set_margin(out_info_lbl, default_margin);
    out_info_lbl.set_halign(Gtk::Align::ALIGN_END);

    win_vbox.pack_start(menus_hbox, Gtk::PackOptions::PACK_SHRINK);

    {
        // note: the popup menus set up here were originally implemented using
        // Gio::Menu but the popovers were being rendered clipped in Manjaro KDE
        // and XFCE

        auto append = [&](Gtk::Menu& menu, Gtk::MenuItem& item, const char* label) {
            item.set_label(label);
            menu.append(item);
            item.signal_activate().connect(sigc::bind(sigc::mem_fun(*this, &main_window::on_function_activate), label));
            item.show();
        };

        menus_hbox.pack_start(functions_a_btn);
        functions_a_btn.set_label("_Funcs");
        functions_a_btn.set_tooltip_text("General Functons");
        set_margin(functions_a_btn, 0);
        functions_a_btn.set_hexpand(true);
        functions_a_btn.set_use_underline(true);
        functions_a_btn.set_popup(functions_a_menu);
    
        append(functions_a_menu, exp_item, "exp() - e raised to the argument's power");
        append(functions_a_menu, ln_item, "ln() - Natural (base e) log");
        append(functions_a_menu, log10_item, "log10() - Base 10 log");
        append(functions_a_menu, log2_item, "log2() - Base 2 log");
        append(functions_a_menu, sqrt_item, "sqrt() - Square root");
        append(functions_a_menu, cbrt_item, "cbrt() - Cubic root");
        append(functions_a_menu, gamma_item, "gamma() - Gamma function");
        append(functions_a_menu, lgamma_item, "lgamma() - Log gamma function");
        append(functions_a_menu, arg_item, "arg() - Phase angle");
        append(functions_a_menu, norm_item, "norm() - Squared magnitude");
        append(functions_a_menu, conj_item, "conj() - Conjugate");
        append(functions_a_menu, proj_item, "proj() - Projection onto the Riemann sphere");

        menus_hbox.pack_start(functions_b_btn);
        functions_b_btn.set_label("_Trig");
        set_margin(functions_b_btn, 0);
        functions_b_btn.set_tooltip_text("Trigonometric Functons");
        functions_b_btn.set_hexpand(true);
        functions_b_btn.set_use_underline(true);
        functions_b_btn.set_popup(functions_b_menu);

        append(functions_b_menu, sin_item, "sin() - Sine");
        append(functions_b_menu, cos_item, "cos() - Cosine");
        append(functions_b_menu, tan_item, "tan() - Tangent");
        append(functions_b_menu, asin_item, "asin() - Arc sin");
        append(functions_b_menu, acos_item, "acos() - Arc cos");
        append(functions_b_menu, atan_item, "atan() - Arc tan");
        append(functions_b_menu, sinh_item, "sinh() - Hyperbolic sin");
        append(functions_b_menu, cosh_item, "cosh() - Hyperbolic cos");
        append(functions_b_menu, tanh_item, "tanh() - Hyperbolic tan");
        append(functions_b_menu, asinh_item, "asinh() - Inverse hyperbolic sin");
        append(functions_b_menu, acosh_item, "acosh() - Inverse hyperbolic cos");
        append(functions_b_menu, atanh_item, "atanh() - Inverse hyperbolic tan");
    }

    menus_hbox.pack_start(settings_btn);
    set_margin(settings_btn, 0);
    settings_btn.set_hexpand(true);
    settings_btn.set_use_underline(true);
    settings_btn.signal_clicked().connect(sigc::mem_fun(*this, &main_window::on_settings_btn_clicked));

    header_bar.show();
    clear_result_hbox.show();
    clear_result_btn.show();
    clear_result_lbl.show();
    more_btn.show();
    win_vbox.show();
    content_vbox.show();
    expr_hbox.show();
    expr_entry.show();
    expr_btn.show();
    expr_btn_lbl.show();
    result_frame.show();
    result_lbl.show();
    variables_space.show();
    variables_frame.hide(); // variables frame is initially hidden
    variables_scroller.show();
    variables_lbl.show();
    in_out_info_hbox.show();
    in_info_lbl.show();
    out_info_lbl.show();
    menus_hbox.show();
    functions_a_btn.show();
    functions_b_btn.show();
    settings_btn.show();

    settings_storager.load(parse_options, out_options);
    parser.options(parse_options);

    show_input_info();
    show_output_info();
}

auto main_window::show_input_info() -> void {
    assert(parse_options == parser.options());

    Glib::ustring buf;
    buf.reserve(128);

    buf = "Input: ";
    switch (parse_options.default_number_type_code) {
        case calc_val::int_code: buf += "int "; break;
        case calc_val::uint_code: buf += "uint "; break;
        case calc_val::complex_code: buf += "cplx "; break;
    }
    switch (parse_options.default_number_radix) {
        case calc_val::base2: buf += "binary"; break;
        case calc_val::base8: buf += "octal"; break;
        case calc_val::base10: buf += "decimal"; break;
        case calc_val::base16: buf += "hex"; break;
    }
    in_info_lbl.set_text(buf);
}

auto main_window::show_output_info() -> void {
    Glib::ustring buf;
    buf.reserve(128);

    buf = "Output: ";
    switch (out_options.output_radix) {
        case calc_val::base2: buf += "binary"; break;
        case calc_val::base8: buf += "octal"; break;
        case calc_val::base10: buf += "decimal"; break;
        case calc_val::base16: buf += "hex"; break;
    }
    out_info_lbl.set_text(buf);
}

auto main_window::append_history(const Glib::ustring& expr_str) -> void {
    if (is_blank(expr_str))
        return;
    static_assert(max_history_size > 0);
    if (history.size() >= max_history_size)
        history.erase(history.begin());
    history.push_back(expr_str);
    history_idx = history.size(); // 1 past last so pressing up-arrow will recover it
}

auto main_window::recall_history(bool direction_up) -> void {
    bool beep_and_return = false;

    if (direction_up) {
        if (history_idx > 0)
            --history_idx;
        else
            beep_and_return = true;
    } else {
        if (history_idx < history.size()) // allow 1 past last element
            ++history_idx;
        else
            beep_and_return = true;
    }

    if (beep_and_return) {
        if (auto p = get_display())
            gdk_display_beep(p->gobj());
        return;
    }

    static_assert(std::is_unsigned_v<decltype(history_idx)>);
    if (history_idx < history.size()) {
        const auto& text = history[history_idx];
        expr_entry.set_text(text);
        expr_entry.set_position(text.size());
    } else
        expr_entry.set_text(Glib::ustring());
    if (last_result_kind == parse_error_kind) {
        result_lbl.set_text(Glib::ustring());
        last_result_kind = none_kind;
    }
}

auto main_window::on_expr_entry_key_pressed(GdkEventKey* key) -> bool {
    if (!key)
        return false;
    switch (key->keyval) {
        case GDK_KEY_parenright: {
            auto text = expr_entry.get_text();
            auto pos = expr_entry.get_position();
            if (decltype(text)::size_type(pos) < text.size() && text[pos] == ')') {
                expr_entry.set_position(pos + 1);
                return true;
            }
            return false;
        }
        case GDK_KEY_Up:
            recall_history(/*direction_up*/ true);
            return true;
        case GDK_KEY_Down:
            recall_history(/*direction_up*/ false);
            return true;
        default:
            return false;
    }
}

auto main_window::on_clear_result_btn_clicked() -> void {
    expr_entry.set_text(Glib::ustring());
    result_lbl.set_label(Glib::ustring());
    last_result_kind = none_kind;

    resize(1, 1); // actually, resizes to minimum width/height; see also
    // https://gitlab.gnome.org/GNOME/gtkmm/-/issues/112

    expr_entry.grab_focus_without_selecting();
}

auto main_window::on_expr_btn_clicked() -> void {
    evaluate();
    expr_entry.grab_focus_without_selecting();
}

auto main_window::on_function_activate(const char* label) -> void {
    std::string_view text = label;
    auto rparen_pos = text.find(')');
    if (rparen_pos != decltype(text)::npos) {
        expr_entry.delete_selection(); // incase there's a selection
        int cursor_pos = expr_entry.get_position();
        expr_entry.insert_text(text.data(), rparen_pos + 1, cursor_pos);
        expr_entry.set_position(cursor_pos - 1);
    }
    expr_entry.grab_focus_without_selecting();
}

auto main_window::on_settings_btn_clicked() -> void {
    if (settings_win)
        settings_win->present();
    else {
        settings_win = std::make_unique<settings_window>(app, *this);
        settings_win->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &main_window::on_client_hide), settings_win.get()), false);
        settings_win->show();
    }
}

auto main_window::on_variables_item_clicked() -> void {
    if (variables_frame.get_visible()) {
        variables_frame.hide();
        variables_lbl.set_label(Glib::ustring());
        if (more_menu) {
            more_menu->remove(show_variables_position);
            more_menu->insert(show_variables_position, "Show Variables", "more.variables");
        }
        resize(1, 1); // actually, resizes to minimum width/height
    } else {
        variables_frame.show();
        display_variables();
        if (more_menu) {
            more_menu->remove(show_variables_position);
            more_menu->insert(show_variables_position, "Hide Variables", "more.variables");
        }
    }
}

auto main_window::display_variables() -> void {
    auto begin = parser.variables_begin();
    auto end = parser.variables_end();

    if (begin == end) {
        variables_lbl.set_label("No variables are defined.");
        return;
    }

    std::stringstream buf;
    for (auto itr = begin; itr != end;) {
        buf << itr->first;
        buf << " = ";
        buf << calc_outputter(out_options)(itr->second);
        if (++itr != end)
            buf << "\n";
    }
    variables_lbl.set_label(buf.str());
}

auto main_window::on_help_item_clicked() -> void {
    app.help(this, help_window::quick_start_idx, false);
}

auto main_window::on_about_item_clicked() -> void {
    if (about_dlg)
        about_dlg->present();
    else {
        about_dlg = std::make_unique<Gtk::AboutDialog>();
        about_dlg->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &main_window::on_client_hide), about_dlg.get()), false);
        about_dlg->signal_response().connect(sigc::mem_fun(*this, &main_window::on_about_dlg_response)); // sometimes the Close button will be presented
        about_dlg->set_transient_for(*this);

        about_dlg->set_logo_icon_name(app_icon);
        about_dlg->set_program_name(app_name);
        about_dlg->set_version(app_version);
        about_dlg->set_copyright(app_copyright);
        about_dlg->set_comments("An advanced text-based calculator");
        about_dlg->set_website("https://github.com/mgeck64/ccalc-gtk3");
        about_dlg->set_license_type(Gtk::License::LICENSE_GPL_3_0);

        Glib::OptionGroup::vecustrings authors;
        authors.emplace_back("Mark Geck");
        about_dlg->set_authors(authors);

        about_dlg->show();
    }

    expr_entry.grab_focus_without_selecting();
}

auto main_window::on_about_dlg_response(int /*response_id*/) -> void {
    // getting non-sensical response id GTK_RESPONSE_DELETE_EVENT for close
    // button under Manjaro KDE (expected GTK_RESPONSE_CLOSE), so default
    // response for any case will be to simply delete the dialog (i.e., assume
    // response_id is unreliable)
    about_dlg.reset();
}

auto main_window::on_variables_changed() -> void {
    if (variables_frame.is_visible())
        display_variables();
}

auto main_window::evaluate() -> void {
    auto expr_str = expr_entry.get_text();

    if (!expr_str.is_ascii()) {
        result_lbl.set_text("Only ASCII characters are allowed.");
        return;
    }

    auto new_out_options = out_options;
    try {
        auto result = parser.evaluate(
            std::string_view(expr_str.data(), expr_str.size()),
            std::bind(&main_window::on_help_item_clicked, this),
            new_out_options,
            std::bind(&main_window::on_variables_changed, this));
        std::ostringstream out;
        out << calc_outputter(new_out_options)(result);
        result_lbl.set_text(out.str());
        last_result_kind = value_kind;
        expr_entry.set_text(Glib::ustring());
        append_history(expr_str);
    } catch (const calc_parse_error& e) {
        expr_entry.select_region(e.token().view_offset, e.token().view_offset + e.token().view.size());
        result_lbl.set_text(e.error_str());
        last_result_kind = parse_error_kind;
    } catch (const calc_parser::void_expression) { // this is not an error
        expr_entry.set_text(Glib::ustring());
        if (last_result_kind == parse_error_kind) {
            result_lbl.set_text(Glib::ustring());
            last_result_kind = none_kind;
        }
        append_history(expr_str);
    }
    update_if_options_changed(new_out_options);
}

auto main_window::options(const parser_options& parse_options, const output_options& out_options) -> void {
    parser.options(parse_options);
    update_if_options_changed(out_options);
}

auto main_window::update_if_options_changed(const output_options& new_out_options) -> void {
    auto new_parse_options = parser.options();

    if (settings_win)
        settings_win->update_from(parse_options, new_parse_options, out_options, new_out_options);

    bool options_changed = false;

    if (parse_options.int_word_size != new_parse_options.int_word_size) {
        options_changed = true;
        parse_options.int_word_size = new_parse_options.int_word_size;
        if (last_result_kind == value_kind) {
            std::ostringstream out;
            out << calc_outputter(new_out_options)(parser.trimmed_last_val());
            result_lbl.set_text(out.str());
        }
        // note: variables don't change when int_word_size changes
    }
    if (parse_options != new_parse_options) {
        options_changed = true;
        parse_options = new_parse_options;
        show_input_info();
    }
    if (out_options != new_out_options) {
        options_changed = true;
        out_options = new_out_options;
        if (last_result_kind == value_kind) { // output format changed; redisplay
            std::ostringstream out;
            out << calc_outputter(out_options)(parser.last_val());
            result_lbl.set_text(out.str());
        }
        show_output_info();
        on_variables_changed(); // output format changed; redisplay
    }
    if (options_changed)
        settings_storager.save(new_parse_options, new_out_options);
}

auto main_window::on_hide() -> void {
    //settings_win.reset();
    //about_dlg.reset();

    // for unknown reason, destroying the settings window here, or allowing it
    // to be destroyed after this event, causes a segfault. this is frustrating.
    // thus we release the pointer here, hide the window, and let the pointer
    // dangle; this seems to work ok and should be ok since the main window
    // won't be unhidden, and either the app is closing or the help window is
    // being displayed and the app will be closed when the help window is
    // closed. for consistency's sake, we apply this logic to the other windows.
    // sigh. note: in gtkmm 4, this was handled in on_close_request, in which
    // deleting the windows caused no problem, but on_close_request is
    // unavailable in gtkmm 3

    auto p_settings_win = settings_win.release();
    auto p_about_dlg = about_dlg.release();

    if (p_settings_win)
        p_settings_win->hide();
    if (p_about_dlg)
        p_about_dlg->hide();

    Gtk::Window::on_hide();
}

auto main_window::on_client_hide(Gtk::Window* win) -> void {
    assert(win);
    if (win == settings_win.get())
        settings_win.reset();
    else if (win == about_dlg.get())
        about_dlg.reset();
}