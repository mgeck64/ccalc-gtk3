#include "settings_window.hpp"
#include "gcalc_basics.hpp"
#include "gcalc_app.hpp"
#include "main_window.hpp"
#include "help_window.hpp"

#include <giomm/simpleactiongroup.h>

settings_window::settings_window(gcalc_app& app_, main_window& main_win_) :
    app(app_),
    main_win(main_win_),
    win_vbox(Gtk::Orientation::ORIENTATION_VERTICAL),
    content_vbox(Gtk::Orientation::ORIENTATION_VERTICAL),
    input_defaults_vbox(Gtk::Orientation::ORIENTATION_VERTICAL),
    input_defaults_hbox(Gtk::Orientation::ORIENTATION_HORIZONTAL),
    bottom_hbox(Gtk::Orientation::ORIENTATION_HORIZONTAL),
    help_btn("_Help"),
    defaults_btn("_Defaults"),
    cancel_btn("_Cancel")
{
    set_title("Settings");
    set_resizable(false);
    add(win_vbox);

    win_vbox.pack_start(content_vbox);
    set_margin(content_vbox, default_margin);

    content_vbox.pack_start(settings_grid);
    set_margin(settings_grid, default_margin);
    settings_grid.set_row_spacing(default_margin * 4);
    settings_grid.set_column_spacing(default_margin);

    settings_grid.attach(input_defaults_vbox, 1, 1, 2, 1);

    input_defaults_vbox.pack_start(input_defaults_lbl);
    input_defaults_lbl.set_text("Input Defaults");
    input_defaults_lbl.set_margin_bottom(default_margin);
    input_defaults_lbl.set_halign(Gtk::Align::ALIGN_START);

    input_defaults_vbox.pack_start(input_defaults_hbox);

    input_defaults_hbox.pack_start(default_number_type_code_cb);
    default_number_type_code_cb.set_margin_end(default_margin);
    default_number_type_code_cb.set_hexpand(true);
    default_number_type_code_cb.append("Signed Integer Type");
    default_number_type_code_cb.append("Unsigned Integer Type");
    default_number_type_code_cb.append("Complex Type");
    default_number_type_code_cb.signal_changed().connect(sigc::mem_fun(*this, &settings_window::on_setting_changed));

    input_defaults_hbox.pack_start(default_number_radix_cb);
    default_number_radix_cb.set_margin_start(default_margin);
    default_number_radix_cb.append("Binary");
    default_number_radix_cb.append("Octal");
    default_number_radix_cb.append("Decimal");
    default_number_radix_cb.append("Hexadecimal");
    default_number_radix_cb.signal_changed().connect(sigc::mem_fun(*this, &settings_window::on_setting_changed));

    settings_grid.attach(output_base_lbl, 1, 2, 1, 1);
    output_base_lbl.set_text("Output Base");
    output_base_lbl.set_halign(Gtk::Align::ALIGN_START);

    settings_grid.attach(output_radix_cb, 2, 2, 1, 1);
    output_radix_cb.set_halign(Gtk::Align::ALIGN_END);
    output_radix_cb.append("Binary");
    output_radix_cb.append("Octal");
    output_radix_cb.append("Decimal");
    output_radix_cb.append("Hexadecimal");
    output_radix_cb.signal_changed().connect(sigc::mem_fun(*this, &settings_window::on_setting_changed));

    settings_grid.attach(output_fp_normalized_lbl, 1, 3, 1, 1);
    output_fp_normalized_lbl.set_text("Normalized Scientific P Notation");
    output_fp_normalized_lbl.set_halign(Gtk::Align::ALIGN_START);

    settings_grid.attach(output_fp_normalized_switch, 2, 3, 1, 1);
    set_margin(output_fp_normalized_switch, default_margin);
    output_fp_normalized_switch.set_halign(Gtk::Align::ALIGN_END);
    output_fp_normalized_switch.property_active().signal_changed().connect(sigc::mem_fun(*this, &settings_window::on_setting_changed));

    settings_grid.attach(precision_lbl, 1, 4, 1, 1);
    precision_lbl.set_text("Floating Point Output Precision");
    precision_lbl.set_halign(Gtk::Align::ALIGN_START);

    settings_grid.attach(precision_entry, 2, 4, 1, 1);
    precision_entry.set_halign(Gtk::Align::ALIGN_END);
    precision_entry.set_range(0, std::numeric_limits<calc_val::float_type>::digits);
    precision_entry.set_increments(1, 10);
    precision_entry.signal_value_changed().connect(sigc::mem_fun(*this, &settings_window::on_setting_changed));

    settings_grid.attach(integer_word_size_lbl, 1, 5, 1, 1);
    integer_word_size_lbl.set_text("Integer Word Size");
    integer_word_size_lbl.set_halign(Gtk::Align::ALIGN_START);

    settings_grid.attach(int_word_size_cb, 2, 5, 1, 1);
    int_word_size_cb.set_halign(Gtk::Align::ALIGN_END);
    int_word_size_cb.append("8 Bits");
    int_word_size_cb.append("16 Bits");
    int_word_size_cb.append("32 Bits");
    int_word_size_cb.append("64 Bits");
    int_word_size_cb.append("128 Bits");
    int_word_size_cb.signal_changed().connect(sigc::mem_fun(*this, &settings_window::on_setting_changed));

    win_vbox.pack_start(bottom_hbox);
    bottom_hbox.set_vexpand(false);

    bottom_hbox.pack_start(help_btn);
    help_btn.set_use_underline(true);
    help_btn.signal_clicked().connect(sigc::mem_fun(*this, &settings_window::on_help_btn_clicked));
 
    bottom_hbox.pack_start(defaults_btn);
    defaults_btn.set_use_underline(true);
    defaults_btn.signal_clicked().connect(sigc::mem_fun(*this, &settings_window::on_defaults_btn_clicked));

    win_vbox.show();
    content_vbox.show();
    input_defaults_vbox.show();
    input_defaults_hbox.show();
    input_defaults_lbl.show();
    default_number_type_code_cb.show();
    default_number_radix_cb.show();
    settings_grid.show();
    output_base_lbl.show();
    output_radix_cb.show();
    output_fp_normalized_lbl.show();
    output_fp_normalized_switch.show();
    precision_lbl.show();
    precision_entry.show();
    integer_word_size_lbl.show();
    int_word_size_cb.show();
    bottom_hbox.show();
    help_btn.show();
    defaults_btn.show();
    cancel_btn.show();

    auto [parse_options, out_options] = main_win.options();
    update_from(parse_options, parse_options, out_options, out_options, /*force*/ true);
};

settings_window::~settings_window() {
    if (this == app.help_invoker())
        app.close_help();
}

auto settings_window::update_from(
    const parser_options& parse_options,
    const parser_options& new_parse_options,
    const output_options& out_options,
    const output_options& new_out_options,
    bool force
    ) -> void
// force = true: update all option widgets
// force = false: update option widgets only for options that have changed, thus
// any other pending changes by the user will be preserved
{
    auto t = temporarily_suppress_on_setting_changed(*this);

    if (force || parse_options.default_number_type_code != new_parse_options.default_number_type_code)
        switch (new_parse_options.default_number_type_code) {
            case calc_val::int_code: default_number_type_code_cb.set_active(0); break;
            case calc_val::uint_code: default_number_type_code_cb.set_active(1); break;
            case calc_val::complex_code: default_number_type_code_cb.set_active(2); break;
        }

    if (force || parse_options.default_number_radix != new_parse_options.default_number_radix)
        switch (new_parse_options.default_number_radix) {
            case calc_val::base2:  default_number_radix_cb.set_active(0); break;
            case calc_val::base8:  default_number_radix_cb.set_active(1); break;
            case calc_val::base10: default_number_radix_cb.set_active(2); break;
            case calc_val::base16: default_number_radix_cb.set_active(3); break;
        }

    if (force || out_options.output_radix != new_out_options.output_radix)
        switch (new_out_options.output_radix) {
            case calc_val::base2:  output_radix_cb.set_active(0); break;
            case calc_val::base8:  output_radix_cb.set_active(1); break;
            case calc_val::base10: output_radix_cb.set_active(2); break;
            case calc_val::base16: output_radix_cb.set_active(3); break;
        }

    if (force || out_options.output_fp_normalized != new_out_options.output_fp_normalized)
        output_fp_normalized_switch.set_active(new_out_options.output_fp_normalized);

    if (force || out_options.precision != new_out_options.precision)
        precision_entry.set_value(new_out_options.precision);

    if (force || parse_options.int_word_size != new_parse_options.int_word_size)
        switch (new_parse_options.int_word_size) {
            case calc_val::int_bits_8:   int_word_size_cb.set_active(0); break;
            case calc_val::int_bits_16:  int_word_size_cb.set_active(1); break;
            case calc_val::int_bits_32:  int_word_size_cb.set_active(2); break;
            case calc_val::int_bits_64:  int_word_size_cb.set_active(3); break;
            case calc_val::int_bits_128: int_word_size_cb.set_active(4); break;
        }
}

auto settings_window::on_setting_changed() -> void {
// this is a crude handler but which should be more than performant enough; the
// main_win.options call below is optimized to write the settings only if they
// have changed
    if (suppress_on_setting_changed)
        return;

    auto [parse_options, out_options] = main_win.options();

    switch (default_number_type_code_cb.get_active_row_number()) {
        case 0: parse_options.default_number_type_code = calc_val::int_code; break;
        case 1: parse_options.default_number_type_code = calc_val::uint_code; break;
        case 2: parse_options.default_number_type_code = calc_val::complex_code; break;
    }
    switch (default_number_radix_cb.get_active_row_number()) {
        case 0: parse_options.default_number_radix = calc_val::base2; break;
        case 1: parse_options.default_number_radix = calc_val::base8; break;
        case 2: parse_options.default_number_radix = calc_val::base10; break;
        case 3: parse_options.default_number_radix = calc_val::base16; break;
    }
    switch (int_word_size_cb.get_active_row_number()) {
        case 0: parse_options.int_word_size = calc_val::int_bits_8; break;
        case 1: parse_options.int_word_size = calc_val::int_bits_16; break;
        case 2: parse_options.int_word_size = calc_val::int_bits_32; break;
        case 3: parse_options.int_word_size = calc_val::int_bits_64; break;
        case 4: parse_options.int_word_size = calc_val::int_bits_128; break;
    }
    out_options.output_fp_normalized = output_fp_normalized_switch.get_active();

    auto i = precision_entry.get_value_as_int();
    assert(i >= 0);
    out_options.precision = i;

    switch (output_radix_cb.get_active_row_number()) {
        case 0: out_options.output_radix = calc_val::base2; break;
        case 1: out_options.output_radix = calc_val::base8; break;
        case 2: out_options.output_radix = calc_val::base10; break;
        case 3: out_options.output_radix = calc_val::base16; break;
    }

    main_win.options(parse_options, out_options);
}

auto settings_window::on_help_btn_clicked() -> void {
    app.help(this, help_window::settings_window_idx, /*force_topic*/ true);
}

auto settings_window::on_defaults_btn_clicked() -> void {
    auto parse_options = parser_options();
    auto out_options = output_options();
    update_from(parse_options, parse_options, out_options, out_options, /*force*/ true);
}
