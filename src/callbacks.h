#ifndef CALLBACKS_H_
#define CALLBACKS_H_
#include <gtk/gtk.h>

/* Closes the main window */
void on_main_window_close_btn_clicked(GtkButton* button, gpointer user_data);

void on_main_window_add_btn_clicked(GtkButton* button, gpointer user_data);

void on_main_window_pref_btn_clicked(GtkButton* button, gpointer user_data);

void on_main_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data);

void on_preference_window_ok_btn_clicked(GtkButton* button, gpointer user_data);

void on_preference_window_close_btn_clicked(GtkButton* button, gpointer user_data);

void on_main_window_delete_all_btn_clicked(GtkButton* button, gpointer user_data);

gboolean on_tree_view_button_press_event(GtkWidget* widget, GdkEvent* event, gpointer user_data);

void on_tree_view_popup_menu_delete_activate(GtkMenuItem* menu_item, gpointer user_data);

void on_tree_view_popup_menu_show_definition_activate(GtkMenuItem* menu_item, gpointer user_data);

void on_definition_window_close_btn_clicked(GtkWidget* widget, gpointer user_data);

gboolean on_notification_display(gpointer user_data);

gboolean on_notification_dialog_timeout(gpointer user_data);

void on_main_window_notify_btn_toggle(GtkToggleToolButton *togglebutton, gpointer user_data);

void on_main_window_about_btn_clicked(GtkButton* button, gpointer user_data);

void on_status_icon_activate(GtkStatusIcon* status_icon, gpointer user_data);

void on_status_icon_menu_quit(GtkMenuItem* menu_item, gpointer user_data);

void on_status_icon_popup_menu(GtkStatusIcon* status_icon, guint button, guint activate_time, gpointer user_data);
#endif