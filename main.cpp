#include <gtkmm/application.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/fixed.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/label.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/messagedialog.h>
#include <glib.h>

#include <iostream>
#include <cstdbool>
#include "sudoku_strategy.hpp"
#include "shuffle.hpp"

#define BLOCK_SIZE 70

Gtk::Entry *entries[N][N];
Gtk::Window *window;
Gtk::Grid *grid;
Gtk::Fixed *fixed;
Glib::RefPtr<Gtk::Application> app;
Glib::RefPtr<Gtk::Builder> builder;

int activate_entry_row, activate_entry_col;
bool is_label[N][N];

void gtk_css_set(Gtk::Widget widget);

void hint();

void check();

void new_game();

void entry_changed();

bool entry_activate(GdkEventFocus *, int, int);

void entry_insert_text(const Glib::ustring &, int *);

void activate();

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
        dialog = new Gtk::MessageDialog(*window, "You Win!");
        dialog->set_title("success");
        dialog->set_size_request(200, 200);
    }
    else
    {
        dialog = new Gtk::MessageDialog(*window, "Keep it up!");
        dialog->set_title("fail");
        dialog->set_size_request(200, 200);
    }
    dialog->run();
    delete dialog;
}

void new_game()
{
    g_print("NEW GAME!!\n");
    init_game();
}

void entry_changed()
{
    g_print("%d %d\n", activate_entry_row, activate_entry_col);
    g_print("%s\n", entries[activate_entry_row][activate_entry_col]->get_text());
}

bool entry_activate(GdkEventFocus *focus, int i, int j)
{
    activate_entry_row = i;
    activate_entry_col = j;
    g_print("%d %d\n", i, j);
    return true;
}

void entry_insert_text(const Glib::ustring &text, int *ptr)
{
    int i;
    int length = text.length();
    std::cout << "TEXT:" << text << std::endl;
    const char *c_str = text.data();
    printf("TEXT_CHANGE:%s\n", c_str);
    bool res = true;
    for (i = 0; i < length; i++)
    {
        if (!(c_str[i] >= '1' && c_str[i] <= '9'))
        {
            res = false;
            break;
        }
    }

    if (!res)
        entries[activate_entry_row][activate_entry_col]->set_text("");
    else
    {
        entries[activate_entry_row][activate_entry_col]->set_text(text);
        int number = atoi(c_str);
        sudos_add(number, activate_entry_row, activate_entry_col);
    }
}

void activate()
{
    g_print("ACTIVATE!!\n");
    // set label content
    Gtk::Label *label = nullptr;
    builder->get_widget<Gtk::Label>("title", label);
    label->set_label("数独 Cr.Civitasv Using C++");

    // fixed
    builder->get_widget<Gtk::Fixed>("fixed", fixed);

    // check button
    Gtk::Button *check_btn = nullptr;
    builder->get_widget<Gtk::Button>("check", check_btn);
    check_btn->signal_clicked().connect([]()
                                        { check(); });

    // new game button
    Gtk::Button *new_game_btn = nullptr;
    builder->get_widget<Gtk::Button>("new_game", new_game_btn);
    new_game_btn->signal_clicked().connect([]()
                                           { new_game(); });

    // hint button
    Gtk::Button *hint_btn = nullptr;
    builder->get_widget<Gtk::Button>("hint", hint_btn);
    hint_btn->signal_clicked().connect([]()
                                       { hint(); });

    // grid
    grid = new Gtk::Grid();
    grid->set_column_spacing(0);
    grid->set_row_spacing(0);
    gtk_css_set(grid);

    fixed->put(*grid, 0, 0);
    init_game();
}

void init_game()
{
    // clear
    if (grid != NULL)
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
                entries[i][j]->signal_insert_text().connect(sigc::ptr_fun(&entry_insert_text));
                entries[i][j]->signal_focus_in_event().connect([i, j](GdkEventFocus *focus)
                                                               { return entry_activate(focus, i, j); });
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
    app = Gtk::Application::create(argc, argv, "com.civitasv.sudoku");

    builder = Gtk::Builder::create_from_file("./builder.glade");

    // get window
    window = nullptr;
    builder->get_widget("window", window);

    window->set_resizable(false);

    // add callback on active application
    app->signal_activate().connect(sigc::ptr_fun(&activate));

    return app->run(*window);
}
