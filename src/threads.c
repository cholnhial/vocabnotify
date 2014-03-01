#include "threads.h"
#include "gui.h"
#include "vocabmanager.h"

/**
 *  add_vocab_thread()
 *	
 *	Adds a new vocabulary to the 
 *  database
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *   none
 **/
gpointer add_vocab_thread(gpointer user_data)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE, add_vocab_thread_idle, NULL, NULL);
}

gboolean add_vocab_thread_idle(gpointer user_data)
{	
	gui_show_progress_dialog();

	gchar* word = gui_get_entry_text_and_clear();
	if(!g_strcmp0(word, "")) {
			gui_hide_progress_dialog();
			gui_show_generic_msg_dialog(GTK_MESSAGE_INFO,
									GTK_BUTTONS_CLOSE,
									"Nothing to add",
									ERROR_NO_WORD_TO_ADD 
									);
		g_free(word);
		return FALSE;
	}

	/* check if this is a valid word */
	swni_t* swni = vocab_manager_get_word_defintion(word);

	if(!swni->definitions_list) {
		gui_hide_progress_dialog();
		gui_show_generic_msg_dialog(GTK_MESSAGE_ERROR,
									GTK_BUTTONS_CLOSE,
									"Can't add to list",
									ERROR_ADDING_WORD
									);
		g_free(word);
		return FALSE;
	}
	vocab_manager_free_word_defition(swni);

	/* Capitalise the first char before adding it to the view */
	GString* g_str_word = g_string_new(word);
	g_string_down(g_str_word);
	g_str_word->str[0] = toupper(g_str_word->str[0]);


	/* Check if this word is already in the database */
	if(vocab_manager_check_word(g_str_word->str) == -1) {
		GString* fmted_str  = g_string_new("");
		g_string_printf(fmted_str, ERROR_WORD_IN_LIST, word);
		gui_hide_progress_dialog();
		gui_show_generic_msg_dialog(GTK_MESSAGE_ERROR,
									GTK_BUTTONS_CLOSE,
									"Already in the list",
									fmted_str->str
									);
		g_free(word);
		g_string_free(g_str_word, TRUE);
		g_string_free(fmted_str, TRUE);
		return FALSE;
	}

	/* Add the word to the UI, TreeView */
	gui_add_to_vocab_list(g_str_word->str);

	/* Add the word to the vocab list in the database */
	if(vocab_manager_add_word(g_str_word->str) == -1) {
		/*@NOTE Problem adding word, show
		a error dialog. */
	}

	/* Now that a word has been added, activate the delete all btn */
	gtk_widget_set_sensitive(gui_get_widget(DELETE_ALL_BTN), TRUE);

	gui_hide_progress_dialog();
	g_free(word);
	g_string_free(g_str_word, TRUE);

	return FALSE;	
}

gpointer fill_vocab_list_thread(gpointer user_data)
{
	g_idle_add_full(G_PRIORITY_HIGH_IDLE, fill_vocab_list_thread_idle, NULL, NULL);
}

gboolean fill_vocab_list_thread_idle(gpointer user_data)
{
	gui_show_progress_dialog();

	/* Fill the ListStore */
	GSList* vocab_list =   vocab_manager_get_words_list();
	if(vocab_list == NULL) {
		g_critical("Error getting vocabulary list, maybe it's empty");
		/* make delete button insensitive until, a word is added */
		gtk_widget_set_sensitive(gui_get_widget(DELETE_ALL_BTN), FALSE);
		gui_hide_progress_dialog();
		return FALSE;
	}


	/*@Note Memory cleanup needed, perhaps vocab_manager_word_list_free() */
	for(GSList* iter = vocab_list; iter; iter = iter->next) {
		gui_add_to_vocab_list( (gchar*) iter->data);
	}

	/* free memory allocated to vocab_list */
	vocab_manager_word_list_free(vocab_list);

	gui_hide_progress_dialog();

	return FALSE;
}
