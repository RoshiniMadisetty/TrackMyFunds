/* Personal Finance Manager - GTK3 Version with User Registration and Login */
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRANSACTIONS 1000
#define MAX_USERS 100

typedef struct {
    char type[10];
    char category[50];
    float amount;
    char date[30];
} Transaction;

typedef struct {
    char username[50];
    char password[50];
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
    float total_income;
    float total_expense;
} User;

User users[MAX_USERS];
int user_count = 0;
User *current_user = NULL;

GtkWidget *income_label, *expense_label, *balance_label, *tree_view;
GtkWidget *window_main;
GtkWidget *stack;
GtkWidget *drawing_area;

typedef struct {
    GtkComboBoxText *type_combo;
    GtkEntry *category_entry;
    GtkEntry *amount_entry;
} WidgetsGroup;

typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
} RegisterWidgets;

typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
} LoginWidgets;

void get_current_time(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 30, "%d-%m-%Y %I:%M:%S %p", t);
}

void save_users() {
    FILE *f = fopen("users.txt", "w");
    if (!f) return;
    for (int i = 0; i < user_count; i++) {
        fprintf(f, "%s %s\n", users[i].username, users[i].password);
    }
    fclose(f);
}

void load_users() {
    FILE *f = fopen("users.txt", "r");
    if (!f) return;
    while (fscanf(f, "%49s %49s", users[user_count].username, users[user_count].password) == 2) {
        users[user_count].transaction_count = 0;
        users[user_count].total_income = 0.0f;
        users[user_count].total_expense = 0.0f;
        user_count++;
        if (user_count >= MAX_USERS)
            break;
    }
    fclose(f);
}

void save_transactions() {
    if (!current_user) return;
    char filename[100];
    sprintf(filename, "%s_transactions.txt", current_user->username);
    FILE *f = fopen(filename, "w");
    if (!f) return;
    for (int i = 0; i < current_user->transaction_count; i++) {
        fprintf(f, "%s\t%s\t%.2f\t%s\n",
                current_user->transactions[i].type,
                current_user->transactions[i].category,
                current_user->transactions[i].amount,
                current_user->transactions[i].date);
    }
    fclose(f);
}

void clear_transaction_list() {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
    gtk_list_store_clear(store);
}

void load_transactions() {
    if (!current_user) return;
    clear_transaction_list();

    char filename[100];
    sprintf(filename, "%s_transactions.txt", current_user->username);
    FILE *f = fopen(filename, "r");
    if (!f) return;

    current_user->transaction_count = 0;
    current_user->total_income = 0.0f;
    current_user->total_expense = 0.0f;

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
    GtkTreeIter iter;
    char type[10], category[50], date[30];
    float amount;
    while (fscanf(f, "%9[^\t]\t%49[^\t]\t%f\t%29[^\n]\n", type, category, &amount, date) == 4) {
        strcpy(current_user->transactions[current_user->transaction_count].type, type);
        strcpy(current_user->transactions[current_user->transaction_count].category, category);
        current_user->transactions[current_user->transaction_count].amount = amount;
        strcpy(current_user->transactions[current_user->transaction_count].date, date);

        if (strcmp(type, "INCOME") == 0)
            current_user->total_income += amount;
        else
            current_user->total_expense += amount;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           0, type,
                           1, category,
                           2, amount,
                           3, date,
                           -1);
        current_user->transaction_count++;
        if (current_user->transaction_count >= MAX_TRANSACTIONS)
            break;
    }
    fclose(f);
}

void update_summary_labels() {
    if (!current_user) return;
    char buffer[100];
    sprintf(buffer, "Income: %.2f", current_user->total_income);
    gtk_label_set_text(GTK_LABEL(income_label), buffer);
    sprintf(buffer, "Expenses: %.2f", current_user->total_expense);
    gtk_label_set_text(GTK_LABEL(expense_label), buffer);
    sprintf(buffer, "Balance: %.2f", current_user->total_income - current_user->total_expense);
    gtk_label_set_text(GTK_LABEL(balance_label), buffer);
}

void on_add_transaction(GtkButton *button, gpointer user_data) {
    WidgetsGroup *widgets = (WidgetsGroup *)user_data;

    const char *type = gtk_combo_box_text_get_active_text(widgets->type_combo);
    const char *category = gtk_entry_get_text(widgets->category_entry);
    const char *amount_str = gtk_entry_get_text(widgets->amount_entry);
    float amount = atof(amount_str);

    if (!type || strlen(category) == 0 || amount <= 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                                   "Invalid input! Please select a type and provide a valid category and amount.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (current_user->transaction_count >= MAX_TRANSACTIONS) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                   "Transaction limit reached, cannot add more.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    char date[30];
    get_current_time(date);

    Transaction *t = &current_user->transactions[current_user->transaction_count];
    strcpy(t->type, type);
    strcpy(t->category, category);
    t->amount = amount;
    strcpy(t->date, date);
    current_user->transaction_count++;

    if (strcmp(type, "INCOME") == 0)
        current_user->total_income += amount;
    else
        current_user->total_expense += amount;

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, type,
                       1, category,
                       2, amount,
                       3, date,
                       -1);

    update_summary_labels();
    save_transactions();

    gtk_entry_set_text(widgets->category_entry, "");
    gtk_entry_set_text(widgets->amount_entry, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->type_combo), 0);

    gtk_widget_queue_draw(drawing_area);
}

void on_delete_transaction(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *type;
        gchar *category;
        gfloat amount;
        gchar *date;

        gtk_tree_model_get(model, &iter,
                           0, &type,
                           1, &category,
                           2, &amount,
                           3, &date,
                           -1);

        int index = -1;
        for (int i = 0; i < current_user->transaction_count; i++) {
            if (strcmp(current_user->transactions[i].type, type) == 0 &&
                strcmp(current_user->transactions[i].category, category) == 0 &&
                current_user->transactions[i].amount == amount &&
                strcmp(current_user->transactions[i].date, date) == 0) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            if (strcmp(current_user->transactions[index].type, "INCOME") == 0)
                current_user->total_income -= current_user->transactions[index].amount;
            else
                current_user->total_expense -= current_user->transactions[index].amount;

            for (int j = index; j < current_user->transaction_count - 1; j++) {
                current_user->transactions[j] = current_user->transactions[j+1];
            }
            current_user->transaction_count--;

            GtkListStore *store = GTK_LIST_STORE(model);
            gtk_list_store_remove(store, &iter);

            update_summary_labels();
            save_transactions();
            gtk_widget_queue_draw(drawing_area);
        }

        g_free(type);
        g_free(category);
        g_free(date);
    }
}

gboolean on_draw_graph(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    if (!current_user) return FALSE;

    const int width = gtk_widget_get_allocated_width(widget);
    const int height = gtk_widget_get_allocated_height(widget);
    const int margin = 40;

    cairo_set_source_rgb(cr, 0.8, 0.9, 1); // Light blue background
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, margin, margin, width - 2*margin, height - 2*margin);
    cairo_stroke(cr);

    typedef struct {
        char category[50];
        float amount;
    } CatAmount;

    CatAmount expense_cats[100];
    int expense_cat_count = 0;
    CatAmount income_cats[100];
    int income_cat_count = 0;

    // Aggregate expenses separately by category
    for (int i = 0; i < current_user->transaction_count; i++) {
        Transaction *t = &current_user->transactions[i];
        if (strcmp(t->type, "EXPENSE") == 0) {
            int found = 0;
            for (int j = 0; j < expense_cat_count; j++) {
                if (strcmp(expense_cats[j].category, t->category) == 0) {
                    expense_cats[j].amount += t->amount;
                    found = 1;
                    break;
                }
            }
            if (!found && expense_cat_count < 100) {
                strcpy(expense_cats[expense_cat_count].category, t->category);
                expense_cats[expense_cat_count].amount = t->amount;
                expense_cat_count++;
            }
        }
    }

    // Sum all income amounts as one total for graph, with “Total Income” label
    float total_income = 0.0;
    for (int i = 0; i < current_user->transaction_count; i++) {
        Transaction *t = &current_user->transactions[i];
        if (strcmp(t->type, "INCOME") == 0)
            total_income += t->amount;
    }
    strcpy(income_cats[0].category, "Total Income");
    income_cats[0].amount = total_income;
    income_cat_count = 1;

    // Find max for scale
    float max_amount = total_income;
    for (int i = 0; i < expense_cat_count; i++) {
        if (expense_cats[i].amount > max_amount)
            max_amount = expense_cats[i].amount;
    }
    if (max_amount < 1.0f) max_amount = 1.0f;

    int bar_width = 20;
    int gap = 15;
    int start_x = margin + 50;
    int origin_y = height - margin - 30;

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12);

    for (int i = 0; i < expense_cat_count; i++) {
        float bar_height = (expense_cats[i].amount / max_amount) * (height - 2 * margin - 80);
        cairo_set_source_rgb(cr, 0.7, 0, 0); // Darker red
        cairo_rectangle(cr, start_x, origin_y - bar_height, bar_width, bar_height);
        cairo_fill(cr);

        cairo_text_extents_t te;

        // Category label center below bar
        cairo_text_extents(cr, expense_cats[i].category, &te);
        cairo_move_to(cr, start_x + (bar_width - te.width) / 2 - te.x_bearing, origin_y + 15);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_show_text(cr, expense_cats[i].category);

        // Amount label above bar
        char amount_str[20];
        snprintf(amount_str, sizeof(amount_str), "%.0f", expense_cats[i].amount);
        cairo_text_extents(cr, amount_str, &te);
        cairo_move_to(cr, start_x + (bar_width - te.width) / 2 - te.x_bearing, origin_y - bar_height - 5);
        cairo_show_text(cr, amount_str);

        start_x += bar_width + gap;
    }

    start_x += 40; // space before income bar

    float bar_height = (income_cats[0].amount / max_amount) * (height - 2 * margin - 80);
    cairo_set_source_rgb(cr, 0, 0.45, 0); // Darker green
    cairo_rectangle(cr, start_x, origin_y - bar_height, bar_width, bar_height);
    cairo_fill(cr);

    cairo_text_extents_t te;

    // Income label center below bar
    cairo_text_extents(cr, income_cats[0].category, &te);
    cairo_move_to(cr, start_x + (bar_width - te.width) / 2 - te.x_bearing, origin_y + 15);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_show_text(cr, income_cats[0].category);

    // Income amount label above bar
    char amount_str[20];
    snprintf(amount_str, sizeof(amount_str), "%.0f", income_cats[0].amount);
    cairo_text_extents(cr, amount_str, &te);
    cairo_move_to(cr, start_x + (bar_width - te.width) / 2 - te.x_bearing, origin_y - bar_height - 5);
    cairo_show_text(cr, amount_str);

    // Legend
    cairo_set_source_rgb(cr, 0.7, 0, 0);
    cairo_rectangle(cr, margin + 10, margin + 10, 20, 15);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, margin + 35, margin + 22);
    cairo_show_text(cr, "Expenses");

    cairo_set_source_rgb(cr, 0, 0.45, 0);
    cairo_rectangle(cr, margin + 110, margin + 10, 20, 15);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, margin + 135, margin + 22);
    cairo_show_text(cr, "Income");

    return FALSE;
}

void switch_to_login(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "login");
}

void switch_to_register(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "register");
}

void on_register_submit(GtkButton *button, gpointer user_data) {
    RegisterWidgets *widgets = (RegisterWidgets *)user_data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(widgets->password_entry));

    if (strlen(username) == 0 || strlen(password) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
            "Please enter both username and password.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                "Username already exists.");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return;
        }
    }

    if (user_count >= MAX_USERS) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Maximum user limit reached.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    users[user_count].transaction_count = 0;
    users[user_count].total_income = 0.0f;
    users[user_count].total_expense = 0.0f;
    user_count++;
    save_users();

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Registration successful! Please login.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    switch_to_login(NULL, NULL);
}

void on_login_submit(GtkButton *button, gpointer user_data) {
    LoginWidgets *widgets = (LoginWidgets *)user_data;
    const char *username = gtk_entry_get_text(GTK_ENTRY(widgets->username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(widgets->password_entry));

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 &&
            strcmp(users[i].password, password) == 0) {
            current_user = &users[i];
            GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
            gtk_list_store_clear(store);
            load_transactions();
            update_summary_labels();
            gtk_stack_set_visible_child_name(GTK_STACK(stack), "main");
            gtk_widget_show(window_main);
            return;
        }
    }
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Invalid username or password.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_logout(GtkButton *button, gpointer user_data) {
    current_user = NULL;
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "start");
}

void activate(GtkApplication *app, gpointer user_data) {
    window_main = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_main), "Finance Manager");
    gtk_window_set_default_size(GTK_WINDOW(window_main), 900, 650);
    gtk_window_set_resizable(GTK_WINDOW(window_main), TRUE);

    stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window_main), stack);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "window, stack {"
        "  background-color: #0047AB;"  /* cobalt blue */
        "}"
        "button {"
        "  background-color: #29508A;"
        "  border-radius: 6px;"
        "  padding: 6px;"
        "  font-weight: bold;"
        "  min-width: 80px;"
        "  color: #FFFFFF;"
        "}"
        "button:hover {"
        "  background-color: #4070C0;"
        "}"
        "entry, combo-box {"
        "  padding: 6px 12px;"
        "  border-radius: 10px;"
        "  border: 1px solid #BBB;"
        "  min-width: 180px;"
        "  color: #000000;"
        "  background-color: #FFFFFF;"
        "}"
        "label {"
        "  color: #FFFFFF;"
        "}"
        , -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(window_main);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Start Page
    GtkWidget *start_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    GtkWidget *welcome_label = gtk_label_new("<span size='xx-large' weight='bold' foreground='white'>Welcome to Finance Manager</span>");
    gtk_label_set_use_markup(GTK_LABEL(welcome_label), TRUE);
    GtkWidget *btn_register_start = gtk_button_new_with_label("Register");
    GtkWidget *btn_login_start = gtk_button_new_with_label("Login");
    gtk_widget_set_size_request(btn_register_start, 80, -1);
    gtk_widget_set_size_request(btn_login_start, 80, -1);
    gtk_box_pack_start(GTK_BOX(start_box), welcome_label, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(start_box), btn_register_start, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(start_box), btn_login_start, FALSE, FALSE, 10);
    gtk_widget_set_halign(welcome_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(btn_register_start, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(btn_login_start, GTK_ALIGN_CENTER);
    gtk_stack_add_named(GTK_STACK(stack), start_box, "start");

    // Register Page
    GtkWidget *register_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    GtkWidget *reg_label = gtk_label_new("<span size='large' foreground='white'><b>Register</b></span>");
    gtk_label_set_use_markup(GTK_LABEL(reg_label), TRUE);
    GtkWidget *reg_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(reg_username), "Username");
    GtkWidget *reg_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(reg_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(reg_password), FALSE);
    GtkWidget *btn_register_submit = gtk_button_new_with_label("Register");
    GtkWidget *btn_back_register = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(btn_register_submit, 80, -1);
    gtk_widget_set_size_request(btn_back_register, 80, -1);
    gtk_box_pack_start(GTK_BOX(register_box), reg_label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(register_box), reg_username, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(register_box), reg_password, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(register_box), btn_register_submit, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(register_box), btn_back_register, FALSE, FALSE, 10);
    gtk_widget_set_halign(reg_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(btn_register_submit, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(btn_back_register, GTK_ALIGN_CENTER);
    gtk_stack_add_named(GTK_STACK(stack), register_box, "register");

    // Login Page
    GtkWidget *login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    GtkWidget *login_label = gtk_label_new("<span size='large' foreground='white'><b>Login</b></span>");
    gtk_label_set_use_markup(GTK_LABEL(login_label), TRUE);
    GtkWidget *login_username = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_username), "Username");
    GtkWidget *login_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(login_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(login_password), FALSE);
    GtkWidget *btn_login_submit = gtk_button_new_with_label("Login");
    GtkWidget *btn_back_login = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(btn_login_submit, 80, -1);
    gtk_widget_set_size_request(btn_back_login, 80, -1);
    gtk_box_pack_start(GTK_BOX(login_box), login_label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(login_box), login_username, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(login_box), login_password, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(login_box), btn_login_submit, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(login_box), btn_back_login, FALSE, FALSE, 10);
    gtk_widget_set_halign(login_label, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(btn_login_submit, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(btn_back_login, GTK_ALIGN_CENTER);
    gtk_stack_add_named(GTK_STACK(stack), login_box, "login");

    // Main UI block
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

    GtkWidget *form = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *type_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(type_combo), "INCOME");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(type_combo), "EXPENSE");
    gtk_combo_box_set_active(GTK_COMBO_BOX(type_combo), 0);

    GtkWidget *category_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(category_entry), "Category");
    GtkWidget *amount_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(amount_entry), "Amount");
    GtkWidget *add_button = gtk_button_new_with_label("Add Transaction");
    GtkWidget *delete_button = gtk_button_new_with_label("Delete Selected");
    gtk_widget_set_size_request(add_button, 80, -1);
    gtk_widget_set_size_request(delete_button, 120, -1);

    gtk_box_pack_start(GTK_BOX(form), type_combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form), category_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(form), amount_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(form), add_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form), delete_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), form, FALSE, FALSE, 0);

    GtkListStore *store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_FLOAT, G_TYPE_STRING);
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column;

    column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    column = gtk_tree_view_column_new_with_attributes("Category", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    column = gtk_tree_view_column_new_with_attributes("Amount", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    column = gtk_tree_view_column_new_with_attributes("Date", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), tree_view);
    gtk_box_pack_start(GTK_BOX(main_box), scroll, TRUE, TRUE, 0);

    GtkWidget *summary = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
    income_label = gtk_label_new("Income: 0.00");
    expense_label = gtk_label_new("Expenses: 0.00");
    balance_label = gtk_label_new("Balance: 0.00");
    gtk_box_pack_start(GTK_BOX(summary), income_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(summary), expense_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(summary), balance_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), summary, FALSE, FALSE, 0);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 800, 250);
    gtk_box_pack_start(GTK_BOX(main_box), drawing_area, FALSE, FALSE, 10);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw_graph), NULL);

    // Add graph legend labels below drawing area for proper aligned text
    GtkWidget *graph_labels_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);
    gtk_box_set_homogeneous(GTK_BOX(graph_labels_box), FALSE);

    GtkWidget *expenses_label = gtk_label_new(NULL);
    GtkWidget *income_label_graph = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(expenses_label), "<span foreground='darkred' weight='bold'>Red bars: Expenses by Category</span>");
    gtk_label_set_markup(GTK_LABEL(income_label_graph), "<span foreground='darkgreen' weight='bold'>Green bar: Total Income</span>");

    gtk_box_pack_start(GTK_BOX(graph_labels_box), expenses_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(graph_labels_box), income_label_graph, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), graph_labels_box, FALSE, FALSE, 5);

    GtkWidget *btn_logout = gtk_button_new_with_label("Logout");
    gtk_widget_set_size_request(btn_logout, 80, -1);
    gtk_box_pack_start(GTK_BOX(main_box), btn_logout, FALSE, FALSE, 0);

    gtk_stack_add_named(GTK_STACK(stack), main_box, "main");

    WidgetsGroup *widgets = g_malloc(sizeof(WidgetsGroup));
    widgets->type_combo = GTK_COMBO_BOX_TEXT(type_combo);
    widgets->category_entry = GTK_ENTRY(category_entry);
    widgets->amount_entry = GTK_ENTRY(amount_entry);

    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_transaction), widgets);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_transaction), NULL);
    g_signal_connect(btn_logout, "clicked", G_CALLBACK(on_logout), NULL);
    g_signal_connect(btn_register_start, "clicked", G_CALLBACK(switch_to_register), NULL);
    g_signal_connect(btn_login_start, "clicked", G_CALLBACK(switch_to_login), NULL);
    g_signal_connect(btn_back_register, "clicked", G_CALLBACK(switch_to_login), NULL);
    g_signal_connect(btn_back_login, "clicked", G_CALLBACK(switch_to_register), NULL);

    RegisterWidgets *reg_widgets = g_malloc(sizeof(RegisterWidgets));
    reg_widgets->username_entry = reg_username;
    reg_widgets->password_entry = reg_password;
    g_signal_connect(btn_register_submit, "clicked", G_CALLBACK(on_register_submit), reg_widgets);

    LoginWidgets *login_widgets = g_malloc(sizeof(LoginWidgets));
    login_widgets->username_entry = login_username;
    login_widgets->password_entry = login_password;
    g_signal_connect(btn_login_submit, "clicked", G_CALLBACK(on_login_submit), login_widgets);

    load_users();

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "start");
    gtk_widget_show_all(window_main);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.FinanceGTK", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
