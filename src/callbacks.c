#include "callbacks.h"
#include "threads.h"
#include "gui.h"
#include "setting_manager.h"
#include "vocabmanager.h"
#include "notifier.h"
#include "vocabnotify.h"


void on_main_window_close_btn_clicked(GtkButton* button, gpointer user_data) 
{
	gui_close_main_window();
}

void on_main_window_add_btn_clicked(GtkButton* button, gpointer user_data)
{	
	g_thread_new("add_vocab_thread", add_vocab_thread, NULL);
}

void on_main_window_pref_btn_clicked(GtkButton* button, gpointer user_data)
{
	gui_show_preference_window();
}

void on_preference_window_ok_btn_clicked(GtkButton* button, gpointer user_data)
{
	/*@Note Setting Manager should already be initialised in gui.c:gui_init() */

	unsigned int notification_intervals_secs;
	unsigned int notification_expire_secs;
	gboolean play_sound_on_notification;

	GtkWidget* notification_intervals_spinbtn, *notification_expire_spinbtn;
	GtkWidget* notification_play_snd_chckbtn;
	notification_intervals_spinbtn = gui_get_widget("notification_intervals_spinbtn");
	notification_expire_spinbtn = gui_get_widget("notification_expire_spinbtn");
	notification_play_snd_chckbtn = gui_get_widget("notification_play_snd_chckbtn");

	/* get the values */
	notification_intervals_secs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(notification_intervals_spinbtn));
	notification_expire_secs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(notification_expire_spinbtn));
	play_sound_on_notification = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(notification_play_snd_chckbtn));

	settings_t* settings = g_malloc(sizeof(settings_t));
	settings->notification_intervals_secs = notification_intervals_secs;
	settings->notification_expire_secs = 	notification_expire_secs;
	settings->play_sound_on_notification = play_sound_on_notification;


	/* Saving new settings to file */
	if(!setting_manager_save_settings(settings)) {
		g_error("Error saving settings");
	}
	if(!setting_manager_save_settings_to_file()) {
		g_error("Error saving settings");
	}

	/* Reaload settings for prefrence window */
	gui_load_prefrences();

	/* set notification timing as there's a change in the prefrences */
	gui_set_notification_time(TRUE);

	/* Close the window */
	gui_close_preference_window();

	g_free(settings);

}

void on_preference_window_close_btn_clicked(GtkButton* button, gpointer user_data)
{
	gui_close_preference_window();
}

void on_main_window_delete_all_btn_clicked(GtkButton* button, gpointer user_data)
{

	/* Show warning dialog, before deleting */
	int response = gui_show_generic_msg_dialog(
	                                           GTK_MESSAGE_WARNING,
	                                           GTK_BUTTONS_YES_NO,
	                                           "Delete all words?",
	                                           WARNING_REMOVE_ALL_WORDS
	                                           );

	switch(response) {
		case GTK_RESPONSE_YES:
		{
			if(vocab_manager_remove_all_words() == -1) {
				/* handle error */
			}
			gui_clear_vocab_list();
			/* make delete button insensitive until, a word is added */
			gtk_widget_set_sensitive(gui_get_widget(DELETE_ALL_BTN), FALSE);
			break;
		}

		case GTK_RESPONSE_NO: 
		{
			/* does nothing */
		}
	}



}

void on_main_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{	
	gui_close_main_window();
}

gboolean on_tree_view_button_press_event(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{	

	if((event->type ==  GDK_BUTTON_PRESS) && (event->button.button == 3)) {
		gui_show_tree_view_popup_menu(event);
		return TRUE; /* handled */
	} 

	return FALSE; /* this was not handled */
}

void on_status_icon_popup_menu(GtkStatusIcon* status_icon, guint button, guint activate_time, gpointer user_data)
{
	gui_show_status_icon_popup_menu (button, activate_time);
}

void on_tree_view_popup_menu_delete_activate(GtkMenuItem* menu_item, gpointer user_data)
{
	GtkTreeView* tree_view =  GTK_TREE_VIEW(user_data);
	GtkTreeSelection* selection;
	GtkTreeModel* list_store;
	GtkTreeIter iter;


	selection = gtk_tree_view_get_selection(tree_view);
	if(gtk_tree_selection_get_selected(selection, &list_store, &iter)) {
		gchar* word;

		gtk_tree_model_get(list_store, &iter, 0, &word, -1);

		/* Issue a warning before removing the word */
		GString* fmted_str  = g_string_new("");
		g_string_printf(fmted_str, WARNING_REMOVE_WORD, word);
		int response = gui_show_generic_msg_dialog(
		                                           GTK_MESSAGE_WARNING,
		                                           GTK_BUTTONS_YES_NO,
		                                           "Delete word?",
		                                           fmted_str->str
		                                           );

		switch(response) {
			case GTK_RESPONSE_YES:
			{
				/* remove from the view */
				gtk_list_store_remove(GTK_LIST_STORE(list_store), &iter);

				/* remove from the database */
				if(vocab_manager_remove_word(word) == -1) {
					g_critical("Unable to remove %s from the database!\n", word);
				}

				/* check if the list is empty */
				if(!gtk_tree_model_get_iter_first(list_store, &iter)) {
					/* make delete button insensitive until, a word is added */
					gtk_widget_set_sensitive(gui_get_widget(DELETE_ALL_BTN), FALSE);
				}

				break;
			}

			case GTK_RESPONSE_NO:
			{
				/* does nothing */
			}

		}

		g_string_free(fmted_str, TRUE);

	} 

}

void on_tree_view_popup_menu_show_definition_activate(GtkMenuItem* menu_item, gpointer user_data)
{
	GtkTreeView* tree_view =  GTK_TREE_VIEW(user_data);
	GtkTreeSelection* selection;
	GtkTreeModel* list_store;
	GtkTreeIter iter;


	selection = gtk_tree_view_get_selection(tree_view);
	if(gtk_tree_selection_get_selected(selection, &list_store, &iter)) {
		gchar* word;
		gtk_tree_model_get(list_store, &iter, 0, &word, -1);
		gui_show_definition_window(word);
	}
}


void on_definition_window_close_btn_clicked(GtkWidget* widget, gpointer user_data)
{
	gtk_widget_hide(GTK_WIDGET(user_data));
}


gboolean on_notification_display(gpointer user_data)
{	
	settings_t* prefs = (settings_t*) user_data;

	guint notification_timeout_idle_func_id;

	/* Check first if there's another dialog running.
	 If that is so then don't start another dialog.*/
	if(notifier_dialog_is_running()) {

		return G_SOURCE_CONTINUE;
	}



	/* select a random word from the list */
	gchar* word;
	GSList* vocab_list = vocab_manager_get_words_list();
	if(vocab_list == NULL) {
		g_critical("There was an error getting the vocablist for notification, maybe it's emp!\n");
		return G_SOURCE_CONTINUE;
	}

	/* calculating list size */
	int size = 0;
	for(GSList* iter = vocab_list; iter != NULL; iter = iter->next) {
		size++;
	}

	/* still got to select a word from the list */
	GRand* grand = g_rand_new_with_seed(vocabnotify_get_random_seed ());
	int n = g_rand_int_range(grand, 0, size);
	g_rand_free(grand);

	word = (gchar*) g_slist_nth_data(vocab_list, n);

	/* Begin timeout function, that will close the dialog if the user
	 does not respond */
	notification_timeout_idle_func_id = g_timeout_add_seconds (prefs->notification_expire_secs, on_notification_dialog_timeout, NULL);

	/* Get the text buffer and fill it in */
	if(gui_create_defintions_text_buffer(word, gtk_text_view_get_buffer(notifier_dialog_get_text_view()))) {

		/***
		 * @Magic happens here
		 * 
		 * Finally we the popup display the dialog
		 * ***/

		/* But let's check and see if we can play the word  audio*/
		GString* wordaudio_file = g_string_new ("");
		g_string_sprintf(wordaudio_file, "%s%s.mp3", WORDSAUDIODIR, word);
		/* If only an audo file exists then do bother to play it */
		if(g_file_test (wordaudio_file->str, G_FILE_TEST_EXISTS) && prefs->play_sound_on_notification)
		{
			vocabnotify_play_audio (wordaudio_file->str);
		}
		g_string_free(wordaudio_file, TRUE);
	   
		notifier_dialog_show(word);
		/* End of magic */
		
	} else  {
		g_critical("Unable to construct the definition text buffer.\n");
	}

	/* Free memory allocated to elemtes in the list,
	 Should be changed to a thread sometimes */
	vocab_manager_word_list_free(vocab_list);

	/* The dialog was closed by the user, so remove the idle timeout func*/
	g_source_remove(notification_timeout_idle_func_id);

	gui_notification_set_state(FALSE);
	gui_notification_set_state(TRUE);

	return G_SOURCE_REMOVE;
}

gboolean on_notification_dialog_timeout(gpointer user_data)
{	
	if(!notifier_dialog_is_running()) {
		return G_SOURCE_REMOVE;
	}

	notifier_dialog_hide();

	return G_SOURCE_REMOVE;
}

void on_main_window_notify_btn_toggle(GtkToggleToolButton *togglebutton, gpointer user_data)
{
	gui_notification_set_state(gtk_toggle_tool_button_get_active(togglebutton));
}

void on_main_window_about_btn_clicked(GtkButton* button, gpointer user_data)
{
	const gchar* authors[] = {"Developers:",
		" Chol Nhial <chol.nhial.chol@gmail.com>",
		"", NULL};

	GtkWidget* about_dialog = gtk_about_dialog_new();

	/* Make it a child of the main window */
	gtk_window_set_transient_for(GTK_WINDOW(about_dialog), GTK_WINDOW(gui_get_widget(MAIN_WINDOW)));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(about_dialog), TRUE); 

	/* Make the dialog modal */
	gtk_window_set_modal(GTK_WINDOW(about_dialog), TRUE);

	/* @Note for testing purposes, the logo is set from a file location,
	 it will later be changed.*/
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_dialog), "vocabnotify");

	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), PROGRAM_NAME_DESKTOP);
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), authors);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), PROGRAM_VERSION);

	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about_dialog),
	                             "This program is free software; you can redistribute it\n"
	                             "and/or modify it under the terms of the GNU General Public\n"
	                             "License as published by the Free Software Foundation; either\n"
	                             "version 2 of the License, or (at your option) any later\n"
	                             "version.\n"
	                             "\n"
	                             "This program is distributed in the hope that it will be\n"
	                             "useful, but WITHOUT ANY WARRANTY; without even the implied\n"
	                             "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n"
	                             "PURPOSE.  See the GNU General Public License for more details."
	                             );

	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "A handy vocabulary manager with an off-line thesaurus based on WordNet");

	gtk_widget_show_all(GTK_WIDGET(about_dialog));

	int result = gtk_dialog_run(GTK_DIALOG(about_dialog));
	switch(result)
	{
		case(GTK_RESPONSE_OK):
		{
			gtk_widget_hide(GTK_WIDGET(about_dialog));

			break;
		}
		case(GTK_RESPONSE_DELETE_EVENT):
		{
			gtk_widget_hide(GTK_WIDGET(about_dialog));
			break;
		}
		default:
		{
			gtk_widget_hide(GTK_WIDGET(about_dialog));
		}
	}

	gtk_widget_destroy(about_dialog);
}

void on_status_icon_activate(GtkStatusIcon* status_icon, gpointer user_data)
{	
	/* Check if the window is already present */
	if(gtk_widget_is_visible (gui_get_widget(MAIN_WINDOW))) {
		gui_close_main_window ();
		return;
	}

	gui_show_main_window();

	/* if the status icon is being clicked for the first time then
	 load the vocab list, else */
	if(gui_tree_view_is_empty()) {
		/* Fill the treeview with the list of vocab 
		 from the database */
		g_thread_new("fill_vocab_list_thread", fill_vocab_list_thread, NULL);
	}

}

void on_status_icon_menu_quit(GtkMenuItem* menu_item, gpointer user_data)
{
	/* do a bit of cleaning up */
	vocab_manager_cleanup ();
	setting_manager_cleanup ();

	/* End the Gtk main loop, thus the application */
	gtk_main_quit();
}
