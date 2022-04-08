#include <gtkmm/application.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/fixed.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/messagedialog.h>

#include <stdlib.h>
#include <stdbool.h>
#include "sudoku_strategy.hpp"
#include "shuffle.hpp"

#define BLOCK_SIZE 70

Gtk::Entry *entries[N][N];
Gtk::Grid *grid;
Gtk::Fixed *fixed;
Glib::RefPtr<Gtk::Application> app;

int activate_entry_row, activate_entry_col;
bool is_label[N][N];

void gtk_css_set(Gtk::Widget widget);

void hint();

void check();

void new_game(const Glib::ustring &, int *);

void entry_changed(int i, int j);

void entry_state_changed(int i, int j);

void entry_insert_text(Gtk::Editable *editable);

void activate(const Glib::ustring &, int *);

void init_game();

void gtk_css_set(Gtk::Widget *widget)
{
    auto provider = Gtk::CssProvider::create();

    provider->load_from_path("./sudoku.css");
    auto context = widget->get_style_context();
    context->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void hint()
{
    if (!is_label[activate_entry_row][activate_entry_col])
    {
        // g_print("%d %d\n", activate_entry_row, activate_entry_col);

        int answer = sudos_answer[activate_entry_row][activate_entry_col];
        char str[2];
        sprintf(str, "%d", answer);
        Gtk::Entry *entry = entries[activate_entry_row][activate_entry_col];

        // g_print("%s\n", str);
        entry->set_text(str);
    }
}

void check()
{
    bool if_success = sudos_check();
    Gtk::Dialog *dialog;
    if (if_success)
    {
        dialog = new Gtk::MessageDialog(NULL, "You Win!");
        dialog->set_title("success");
        dialog->set_size_request(200, 200);
    }
    else
    {

        dialog = new Gtk::MessageDialog(NULL, "Keep it up!");
        dialog->set_title("fail");
        dialog->set_size_request(200, 200);
    }
    dialog->show();
}

void new_game(const Glib::ustring &, int *)
{
    init_game();
}

void entry_changed(int i, int j)
{
    int number = std::stoi(entries[i][j]->get_text());
    sudos_add(number, activate_entry_row, activate_entry_col);
}

void entry_state_changed(int i, int j)
{
    int number = std::stoi(entries[i][j]->get_text());
    sudos_add(number, activate_entry_row, activate_entry_col);
}

void entry_insert_text(Gtk::Editable *editable)
{
    int i;
    Glib::ustring text = editable->get_text();
    int length = text.length();

    for (i = 0; i < length; i++)
    {
        if (!(text[i] >= '1' && text[i] <= '9'))
        {
            g_signal_stop_emission_by_name(editable, "insert-text");
            return;
        }
    }
}

void activate(const Glib::ustring &, int *)
{
    auto builder = Gtk::Builder::create_from_file("./builder.glade");

    // get window
    auto window = builder->get_widget<Gtk::Window>("window");

    window->set_application(app);
    window->set_resizable(false);

    // set label content
    auto label = builder->get_widget<Gtk::Label>("title");
    label->set_label("数独 Cr.Civitasv Using C++");

    // fixed
    fixed = builder->get_widget<Gtk::Fixed>("fixed");

    // check button
    auto check_btn = builder->get_widget<Gtk::Button>("check");
    check_btn->signal_clicked().connect([]()
                                        { check(); });

    // new game button
    auto new_game_btn = builder->get_widget<Gtk::Button>("new_game");
    check_btn->signal_clicked().connect([](const Glib::ustring &a, int *b)
                                        { new_game(a, b); });

    // hint button
    auto hint_btn = builder->get_widget<Gtk::Button>("hint");
    check_btn->signal_clicked().connect([]()
                                        { hint(); });

    // grid
    grid = new Gtk::Grid();
    grid->set_column_spacing(0);
    grid->set_row_spacing(0);
    gtk_css_set(grid);

    fixed->put(*grid, 0, 0);
    init_game();
    window->show();
}

void init_game()
{
    // clear
    fixed->remove(*grid);
    grid = new Gtk::Grid();
    grid->set_column_spacing(0);
    grid->set_row_spacing(0);
    gtk_css_set(grid);

    fixed->put(*grid, 0, 0);
    grid->show();
    // generate sudos
    sudos_generate_real();
    // add entry to grid
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (sudos[i][j] == 0)
            {
                is_label[i][j] = false;
                entries[i][j] = new Gtk::Entry();
                entries[i][j]->set_size_request(BLOCK_SIZE, BLOCK_SIZE);
                entries[i][j]->set_alignment(0.5);
                entries[i][j]->set_width_chars(1);
                entries[i][j]->set_max_length(1);

                gtk_css_set(entries[i][j]);
                grid->attach(*entries[i][j], j, i, 1, 1);
                entries[i][j]->signal_changed().connect([i, j]()
                                                        { entry_changed(i, j); });
                entries[i][j]->signal_insert_text().connect([i, j]()
                                                            { entry_insert_text(entries[i][j]); });
                entries[i][j]->show();
            }
            else
            {
                is_label[i][j] = true;
                char title[2];
                sprintf(title, "%d", sudos[i][j]);
                Gtk::Label *label = new Gtk::Label(title);
                label->set_size_request(BLOCK_SIZE, BLOCK_SIZE);
                gtk_css_set(label);
                grid->attach(*label, j, i, 1, 1);
                label->show();
            }
        }
    }
}

int main(int argc,
         char **argv)
{
    app = Gtk::Application::create("com.civitasv.sudoku");
    int status;

    // // add callback on active application
    // app->signal_activate().connect([](const Glib::ustring &a, int *b)
    //                                 { activate(a, b); });

    // free memory
    return app->run(argc, argv);
}
