#include <stdlib.h>
#include <swni/swni.h>

#include "gui.h"
#include "callbacks.h"
#include "threads.h"
#include "setting_manager.h"
#include "vocabnotify.h"

static GtkBuilder* builder;
static GtkWidget* main_window;
static GtkTreeView* vocab_treeview;
static GtkEntry* word_entry;
static GtkDialog* progress_dialog;
static GtkWidget* preference_window;
static GtkWidget* treeview_popup_menu;
static GtkWidget* status_icon_menu;
static GtkWidget* definition_window;
static GtkWidget* about_dialog;
static GtkWidget* status_icon;

static settings_t* prefs;

static guint notification_idle_func_id;

/**
 *  gui_init()
 *	@Must be called first
 *
 *  Loads the user interface objects from glade file
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    FALSE on failure, TRUE on success
 **/
gboolean gui_init()
{
	builder = gtk_builder_new();

	GError* err = NULL;
	if(!gtk_builder_add_from_file(builder, UI_FILE, &err)) {
		g_critical ("Couldn't load builder file: %s", err->message);
		g_error_free(err);
		//g_free(builder);
		return FALSE;
	}

	main_window = GTK_WIDGET(gtk_builder_get_object(builder, MAIN_WINDOW));
	if(main_window == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", MAIN_WINDOW, UI_FILE);
		//g_free(gtk_builder);
		return FALSE;
	}
	gtk_window_set_default_size(GTK_WINDOW(main_window), 280, 350);

	vocab_treeview = GTK_TREE_VIEW(gtk_builder_get_object(builder, VOCAB_TREEVIEW));
	if(vocab_treeview == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", VOCAB_TREEVIEW, UI_FILE);
		//g_free(gtk_builder);
		return FALSE;
	}

	word_entry = GTK_ENTRY(gtk_builder_get_object(builder, WORD_ENTRY));
	if(word_entry == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", WORD_ENTRY, UI_FILE);
		//g_free(gtk_builder);
		return FALSE;
	}

	progress_dialog = GTK_DIALOG(gtk_builder_get_object(builder, PROGRESS_DIALOG));
	if(progress_dialog == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", PROGRESS_DIALOG, UI_FILE);
		//g_free(gtk_builder);
		return FALSE;
	}

	preference_window = GTK_WIDGET(gtk_builder_get_object(builder, PREFERENCE_WINDOW));
	if(preference_window == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", PREFERENCE_WINDOW, UI_FILE);
		//g_free(gtk_builder);
		return FALSE;
	}
	gtk_window_set_default_size(GTK_WINDOW(preference_window), 150, 100);

	definition_window = GTK_WIDGET(gtk_builder_get_object(builder, DEFINITION_WINDOW));
	if(definition_window == NULL)
	{
		g_critical("Widget \"%s\" is missing in file %s.", DEFINITION_WINDOW, UI_FILE);
		//g_free(gtk_builder);
		return FALSE;
	}

	gtk_window_set_default_size(GTK_WINDOW(definition_window), 480, 330);

	gui_init_tree_view();

	/* Load prefrences for prefrence window */
	gui_load_prefrences();

	/* initialize status icon */
	gui_init_status_icon();

	/* since the prefrence are loaded it's possible to now get the notification interval.*/
	gui_set_notification_time(FALSE);

	/* initialize notifier */
	notifier_dialog_init();

	/* init definition text view */
	gui_defintion_text_view_init(GTK_TEXT_VIEW(gui_get_widget(DEFINITION_TEXT_VIEW)));

	/* Connect the signal handlers */
	gtk_builder_connect_signals(builder, NULL);

	return TRUE;
}

/**
 *  gui_show_main_window()
 *	
 *
 *  Loads the main window using GtkBuilder
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    GtkWidget pointer to the window object
 **/
GtkWidget* gui_show_main_window()
{   
	/* Set the icon */
	gtk_window_set_default_icon_name ("vocabnotify");
	gtk_widget_show_all(main_window);
}

void gui_close_main_window()
{
	gtk_widget_hide(main_window);	
}

/**
 *   gui_show_preference_window()
 *	
 *   Shows the preference window
 *  
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 */
void gui_show_preference_window()
{
	GtkWidget* notification_intervals_spinbtn, *notification_expire_spinbtn;
	GtkWidget* notification_play_snd_chckbtn;
	notification_intervals_spinbtn = gui_get_widget("notification_intervals_spinbtn");
	notification_expire_spinbtn = gui_get_widget("notification_expire_spinbtn");
	notification_play_snd_chckbtn = gui_get_widget("notification_play_snd_chckbtn");

	/* Set up the spin buttons */
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(notification_intervals_spinbtn), 0, 3600000);   
	gtk_spin_button_set_increments(GTK_SPIN_BUTTON(notification_intervals_spinbtn), 1, 1);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(notification_expire_spinbtn), 0, 3600000);   
	gtk_spin_button_set_increments(GTK_SPIN_BUTTON(notification_expire_spinbtn), 1, 1);

	/* set the settings */
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(notification_intervals_spinbtn),
	                          prefs->notification_intervals_secs);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(notification_expire_spinbtn),
	                          prefs->notification_expire_secs);
	gtk_widget_show_all(GTK_WIDGET(preference_window));

	if(prefs->play_sound_on_notification) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(notification_play_snd_chckbtn), TRUE);
	}

}

/**
 *   gui_close_preference_window()
 *	
 *   Hides the preference window
 *  
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 */
void gui_close_preference_window()
{
	gtk_widget_hide(preference_window);
}


/**
 *  gui_get_entry_text_and_clear()
 *	
 *	returns the string entered in 
 *  the entry used to add new vocab, 
 *  and clears it afterwards.
 *
 *  Paramaters:
 *   none
 *  returns:
 *    a string
 **/
gchar* gui_get_entry_text_and_clear()
{
	gchar* text =  g_strdup(gtk_entry_get_text(word_entry));
	gtk_editable_delete_text( GTK_EDITABLE(word_entry ), 0, -1 );

	return text;
}

/**
 *  gui_show_progress_dialog()
 *	
 *	shows the progress dialog when a
 *  word is being added
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_show_progress_dialog()
{
	gtk_window_set_default_size(GTK_WINDOW(progress_dialog), 250, 100);
	gtk_widget_show_all(GTK_WIDGET(progress_dialog));
}
/**
 *  gui_hide_progress_dialog()
 *	
 *	
 *	 hide progress dialog, will usually
 *   be called in a thread
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_hide_progress_dialog()
{
	gtk_widget_hide(GTK_WIDGET(progress_dialog));
}

void gui_init_tree_view() 
{
	GtkTreeViewColumn* col;
	GtkCellRenderer* renderer;
	GtkListStore* store;

	col = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new();
	col = gtk_tree_view_column_new_with_attributes("Vocabulary",
	                                               renderer, "text", 0, NULL);
	gtk_cell_renderer_set_alignment(renderer, 0.5, 0);
	gtk_tree_view_column_set_alignment(col, 0.5);

	/* Append the title column to the treeview widget */
	gtk_tree_view_append_column(vocab_treeview, col);

	/* Set model */
	store = gtk_list_store_new(1, G_TYPE_STRING);

	gtk_tree_view_set_model(vocab_treeview, GTK_TREE_MODEL(store));

	/* initialise the popup menu */
	gui_create_tree_view_popup_menu();

	g_object_unref(store);

}

void gui_add_to_vocab_list(gchar* word) 
{
	GtkListStore* store;
	GtkTreeIter iter;
	store = GTK_LIST_STORE(gtk_tree_view_get_model(vocab_treeview));
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, word, -1);
}

/**
 *  gui_load_prefrences()
 *	
 *	Loads the prefrences from
 *	setting manager. and stores
 *  them in it's local setting_t
 *  variable.
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void gui_load_prefrences()
{
	/* Load the prefrences, for prefrence window */
	prefs = setting_manager_get_settings();
	if(prefs == NULL) {
		g_error("Error loading settings");
	}

}

/**
 *  gui_show_generic_msg_dialog()
 *	
 *	Shows a generic dialog boxes
 *	given the supplied arguments
 *  
 *  
 *
 *  Paramaters:
 *   the message type, the button type, title, and the message
 *  returns:
 *    the response 
 **/
int gui_show_generic_msg_dialog(GtkMessageType type,  GtkButtonsType buttons, gchar* title, gchar * msg)
{
	GtkWidget* dialog = gtk_message_dialog_new (gui_get_widget(MAIN_WINDOW),
	                                            GTK_DIALOG_DESTROY_WITH_PARENT,
	                                            type,
	                                            buttons,
	                                            title
	                                            );

	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), msg);

	int response = gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return response;
}

/**
 *  gui_clear_vocab_list()
 *	
 *	clears the treeview widget
 *	of list items.
 *  
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 **/
void gui_clear_vocab_list()
{
	GtkListStore* store;
	store = GTK_LIST_STORE(gtk_tree_view_get_model(vocab_treeview));
	gtk_list_store_clear(store);
}

/**
 *  gui_create_tree_view_popup_menu()
 *	
 *	Initialises and setups the menu,
 *	and connect signals. the menu will
 *  pop up when a row in the tree view
 *  is clicked
 *  
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 **/
void gui_create_tree_view_popup_menu()
{
	GtkWidget* menu_item;

	treeview_popup_menu = gtk_menu_new();

	menu_item = gtk_menu_item_new_with_label("Show Definition");
	gtk_menu_shell_append(GTK_MENU_SHELL(treeview_popup_menu), menu_item);
	/* connect the signal to handle the activate signal, when clicked*/
	g_signal_connect(G_OBJECT(menu_item),
	                 "activate",
	                 G_CALLBACK(on_tree_view_popup_menu_show_definition_activate),
	                 vocab_treeview);

	menu_item = gtk_menu_item_new_with_label("Delete");
	/* connect the signal to handle the activate signal, when clicked*/
	g_signal_connect(G_OBJECT(menu_item),
	                 "activate",
	                 G_CALLBACK(on_tree_view_popup_menu_delete_activate),
	                 vocab_treeview);
	gtk_menu_shell_append(GTK_MENU_SHELL(treeview_popup_menu), menu_item);

}

void gui_create_status_icon_menu()
{
	GtkWidget* menu_item;

	status_icon_menu = gtk_menu_new();

	/* Add Quit button */
	menu_item = gtk_menu_item_new_with_label("Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(status_icon_menu), menu_item);

	/* Connect the Quit menu item to a signal */
	g_signal_connect(G_OBJECT(menu_item), "activate",
	                 G_CALLBACK(on_status_icon_menu_quit),
	                 NULL);


}

/**
 *  gui_show_definition_window()
 *	
 *	Shows the definiton window, and  
 *	set's the title to include the
 *  word the definition is being show.
 *
 *  Paramaters:
 *   a string
 *  returns:
 *   none
 **/
void gui_show_definition_window(gchar* word)
{	


	/* set title for the window */
	GString* wnd_title = g_string_new("");
	g_string_printf(wnd_title, "%s - %s", PROGRAM_NAME_DESKTOP, word);
	gtk_window_set_title(GTK_WINDOW(definition_window), wnd_title->str);

	GtkTextBuffer* tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui_get_widget(DEFINITION_TEXT_VIEW)));
	GtkTextIter cur = {0};
	GtkTextMark *freq_marker = NULL;


	if(gui_create_defintions_text_buffer(word, tbuffer)) {
		gtk_dialog_run(GTK_DIALOG(definition_window));
	} else  {
		g_critical("Unable to construct the definition text buffer.\n");
	}

	/* cleaning up */
	g_string_free(wnd_title, TRUE);
}

gboolean gui_create_defintions_text_buffer(gchar* word, GtkTextBuffer* tbuffer) 
{

	g_assert(word != NULL);

	GtkTextIter cur = {0};
	GtkTextMark *freq_marker = NULL;

	/* get the defintion of the word */
	swni_t* swni_def = swni_get_definitions_list(word);
	if(swni_def == NULL) {
		g_critical("Unable to retrieve the definition of the word from libswni.\n");
		return FALSE;
	}

	/* Get the lists from swni_t pointer */
	GSList* defn_list = NULL;
	GSList* synonyms_list = NULL;
	defn_list = swni_def->definitions_list;
	synonyms_list = swni_def->synonyms_list; 

	GString* str_count = g_string_new("");
	if(defn_list) {

		/* Build the text */

		/* clear the original text, if there was any */
		gtk_text_buffer_set_text(tbuffer, "", -1);

		word[0] = tolower(word[0]);

		for(GSList* def_iter = defn_list; def_iter != NULL; def_iter = def_iter->next) {

			defn_list_t* def_list = (defn_list_t*) def_iter->data;
			gtk_text_buffer_get_end_iter(tbuffer, &cur);
			gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, word, -1, TAG_LEMMA, NULL);
			gtk_text_buffer_insert(tbuffer, &cur, " ~ ", -1);
			gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, def_list->class, -1, TAG_POS, NULL);

			freq_marker = gtk_text_buffer_create_mark(tbuffer, NULL, &cur, TRUE);

			for(GSList* s_def_iter = def_list->defn_list; s_def_iter != NULL; s_def_iter = s_def_iter->next) {
				defn_t* defn = (defn_t*) s_def_iter->data;

				gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, NEW_LINE, -1, TAG_POS, NULL);

				g_string_printf(str_count, "%s. ", defn->defn_pos);
				gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, str_count->str, -1, TAG_COUNTER, NULL);

				gtk_text_buffer_insert(tbuffer, &cur, defn->definition, -1);

				GSList* example = defn->examples;

				if(example) {
					gtk_text_buffer_insert(tbuffer, &cur, NEW_LINE, -1);
				}

				/* List examples */
				while(example) {

					//gtk_text_buffer_insert(tbuffer, &cur, NEW_LINE, -1);

					gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, (gchar*) example->data, -1, TAG_EXAMPLE, NULL);

					example = g_slist_next(example);

					if(example)
						gtk_text_buffer_insert(tbuffer, &cur, "; ", -1);
					//else if(s_def_iter->next)
					//gtk_text_buffer_insert(tbuffer, &cur, NEW_LINE, -1);

				}

			}

			/* Marker that marks the familiarity of a word */
			gtk_text_buffer_get_iter_at_mark(tbuffer, &cur, freq_marker);

			gtk_text_buffer_insert(tbuffer, &cur, "    ", -1);
			gtk_text_buffer_insert_with_tags_by_name(tbuffer, 
			                                         &cur, 
			                                         def_list->familiarity, 
			                                         -1, 
			                                         def_list->familiarity, 
			                                         NULL);

			/* Newline for next definition list */
			if(def_iter->next) {
				gtk_text_buffer_get_end_iter(tbuffer, &cur);
				gtk_text_buffer_insert(tbuffer, &cur, NEW_LINE, -1);
			}

		}

		/* Add the synonyms at the bottom */
		gtk_text_buffer_get_end_iter(tbuffer, &cur);
		gtk_text_buffer_insert(tbuffer, &cur, NEW_LINE, -1);
		gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, SYNONYMS, -1, TAG_SYNONYMS, NULL);

		GSList* synonym = synonyms_list;

		while(synonym) {
			gtk_text_buffer_insert_with_tags_by_name(tbuffer, &cur, (gchar*) synonym->data, -1, TAG_EXAMPLE, NULL);

			synonym = g_slist_next(synonym);

			if(synonym)
				gtk_text_buffer_insert(tbuffer, &cur, ", ", -1);
		}



	} else {
		g_string_free(str_count, TRUE);

		/* Free libswni stuff */
		swni_destroy_definitions_list(defn_list);
		swni_destroy_synonyms_list(synonyms_list);

		return FALSE;
	}

	/* Free libswni stuff before going */
	swni_destroy_definitions_list(defn_list);
	swni_destroy_synonyms_list(synonyms_list);

	g_string_free(str_count, TRUE);;

	return TRUE;
}

void gui_defintion_text_view_init(GtkTextView* textview)
{	
	GtkTextBuffer* tbuffer = gtk_text_view_get_buffer(textview);

	gtk_text_buffer_create_tag(tbuffer, TAG_LEMMA, "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag(tbuffer, TAG_POS, "foreground", "red", "style", PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag(tbuffer, TAG_COUNTER, "left_margin", 15, "weight", PANGO_WEIGHT_BOLD, "foreground", "gray", NULL);
	gtk_text_buffer_create_tag(tbuffer, TAG_EXAMPLE, "foreground", "blue", "font", "Serif Italic 10", "left_margin", 45, NULL);
	gtk_text_buffer_create_tag(tbuffer, TAG_MATCH, "foreground", "DarkGreen", "weight", PANGO_WEIGHT_SEMIBOLD, NULL);
	gtk_text_buffer_create_tag(tbuffer, TAG_HIGHLIGHT, "background", "black", "foreground", "white", NULL);
	gtk_text_buffer_create_tag(tbuffer, TAG_SYNONYMS, "weight", PANGO_WEIGHT_BOLD, NULL);

	int i;
	for(i = 0; i < FAMILIARITY_COUNT; i++)
	{
		gtk_text_buffer_create_tag(tbuffer, familiarity[i], "background", freq_colors[i], 
		                           "foreground", (i < 4)? "white" : "black", "font", "Monospace 9", NULL);
	}

}

/**
 *  gui_show_tree_view_popup_menu()
 *	
 *	Shows the popup menu, and it's children 
 *	
 *  
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 **/
void gui_show_tree_view_popup_menu(GdkEvent* event)
{	
	/* If there are no words in the list, there's no need to show the menu */
	GtkTreeIter iter;
	if(gui_tree_view_is_empty()) {
		return;
	}

	gtk_widget_show_all(treeview_popup_menu);
	gtk_menu_popup(GTK_MENU(treeview_popup_menu),
	               NULL,
	               NULL,
	               NULL,
	               NULL,
	               event->button.button,
	               gdk_event_get_time((GdkEvent*)event)); 
}

gboolean gui_tree_view_is_empty()
{
	/* If there are no words in the list, there's no need to show the menu */
	GtkTreeIter iter;
	if(!gtk_tree_model_get_iter_first(gtk_tree_view_get_model(vocab_treeview), &iter)) {
		return TRUE;
	}

	return FALSE;
}

/**
 *  gui_show_status_icon_popup_menu()
 *	
 *	Shows the popup menu, and it's children 
 *	
 *  
 *  
 *
 *  Paramaters:
 *   button, and activate time as unsigned integers
 *  returns:
 *   none
 **/
void  gui_show_status_icon_popup_menu(guint button, guint activate_time)
{	

	gtk_widget_show_all(status_icon_menu);
	gtk_menu_popup(GTK_MENU(status_icon_menu),
	               NULL,
	               NULL,
	               NULL,
	               NULL,
	               button,
	               activate_time); 
}


/**
 *  gui_notification_set_time()
 *	
 * This function will set the notification
 * time, and add a timmer function
 * which is our callback.
 * without this function the program would
 * not make those regular popups
 * 
 *  Paramaters:
 *   a boolean: Tells the function whether to add or remove timer
 *  returns:
 *   none
 **/
void gui_set_notification_time(gboolean remove_idle)
{	
	if(remove_idle) 
		g_source_remove(notification_idle_func_id);

	notification_idle_func_id = g_timeout_add_seconds (prefs->notification_intervals_secs, on_notification_display, prefs);
}

/**
 *  gui_notification_set_state()
 *	
 * Sets notification ON or OFF.
 * if 'state' is TRUE then notification
 * is turned ON else if it's FALSE then
 * notification is OFF.
 *  
 *
 *  Paramaters:
 *   a boolean
 *  returns:
 *   none
 **/
void gui_notification_set_state(gboolean state)
{	

	if(state) {
		notification_idle_func_id = g_timeout_add_seconds (prefs->notification_intervals_secs, on_notification_display, prefs);
	} else {
		g_source_remove(notification_idle_func_id);
	}

}

void gui_init_status_icon()
{	

	/* @NOTE this will be changed */
	status_icon = gtk_status_icon_new_from_icon_name("vocabnotify");
	gtk_status_icon_set_visible(GTK_STATUS_ICON(status_icon), TRUE);

	/* Create the menu */
	gui_create_status_icon_menu();

	/* connect signals */   
	g_signal_connect(G_OBJECT(status_icon), "activate", G_CALLBACK(on_status_icon_activate), NULL);
	/* connect the butten-press-event signal for handling displaying menu */
	g_signal_connect(G_OBJECT(status_icon), "popup-menu", G_CALLBACK(on_status_icon_popup_menu), NULL);
}

void gui_show_status_icon()
{
	//gtk_widget_show_all(GTK_WIDGET(status_icon));
}

/* Widget getter */
GtkWidget* gui_get_widget(gchar* widget_name)
{
	return GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
}


/* main function */
int main(int argc, char** argv) 
{

	gtk_init(&argc, &argv);

	if(!vocabnotify_init()) {
		g_critical("Unable to initialise vocabnotify!\n");
		exit(EXIT_FAILURE);
	}

	gui_init();
	gtk_main();

	exit(EXIT_SUCCESS);
}  
