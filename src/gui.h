#ifndef GUI_H_
#define GUI_H_
#include <gtk/gtk.h>

#define UI_FILE "/home/chol/Documents/Projects/vocabnotify/src/vocabnotify.ui"

/* Widgets */
#define MAIN_WINDOW "main_window"
#define PREFERENCE_WINDOW "preference_window"
#define PROGRESS_DIALOG "progress_dialog"
#define PROGRESS_DIALOG_SPINNER "progress_dialog_spinner"
#define DEFINITION_WINDOW "definition_window"
#define DEFINITION_TEXT_VIEW "definition_text_view"
#define NOTIFIER_DEFINITION_TEXT_VIEW "notifier_definition_text_view"
#define WORD_ENTRY "word_entry"
#define VOCAB_TREEVIEW "vocab_treeview"
#define DELETE_ALL_BTN "delete_all_btn"


/* GtkTextView tag constants for definition window, and other */
#define	TAG_LEMMA		"tag_lemma"
#define TAG_POS			"tag_pos"
#define TAG_COUNTER		"tag_counter"
#define TAG_EXAMPLE		"tag_example"
#define TAG_HIGHLIGHT	"tag_highlight"
#define TAG_MATCH		"tag_match"
#define TAG_SYNONYMS	"tag_synonyms"
#define NEW_LINE		"\r\n"

/* Messages and Strings*/
#define ERROR_ADDING_WORD "Check your spelling and try again"
#define ERROR_NO_WORD_TO_ADD "Type a word into the field, and click the add button again"
#define WARNING_REMOVE_ALL_WORDS "Are you sure you want to delete all the words in the list?"
#define WARNING_REMOVE_WORD "Are you sure you want to delete '%s' from the list?"
#define ERROR_WORD_IN_LIST "'%s' is already in the list"
#define SYNONYMS "Synonym(s): "
/* limits */
#define	MAX_SENSE_DIGITS	5
#define FAMILIARITY_COUNT   8

/* general */
static gchar *familiarity[] = {"extremely rare","very rare","rare","uncommon","common", "familiar","very familiar","extremely familiar"};
static gchar *freq_colors[] = {"Black", "SaddleBrown", "FireBrick", "SeaGreen", "DarkOrange", "gold", "PaleGoldenrod", "PeachPuff1"};

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
gboolean gui_init();

/**
 *  gui_show_main_window()
 *	
 *
 *  Shows the main window
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    GtkWidget pointer to the window object
 **/
GtkWidget* gui_show_main_window();

void gui_close_main_window();

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
void gui_show_preference_window();

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
void gui_close_preference_window();

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
void gui_show_progress_dialog();

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
gchar* gui_get_entry_text_and_clear();

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
void gui_hide_progress_dialog();

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
void gui_load_prefrences();

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
int gui_show_generic_msg_dialog(GtkMessageType type,  GtkButtonsType buttons, gchar* title, gchar * msg);


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
void gui_clear_vocab_list();



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
void gui_create_tree_view_popup_menu();


/**
 *  gui_show_tree_view_popup_menu()
 *	
 *	Shows the popup menu, and it's children 
 *	
 *  
 *  
 *
 *  Paramaters:
 *   GdkEvent pointer
 *  returns:
 *   none
 **/
void gui_show_tree_view_popup_menu(GdkEvent* event);

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
void  gui_show_status_icon_popup_menu(guint button, guint activate_time);

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
void gui_show_definition_window(gchar* word);

/**
 *  gui_set_notification_time()
 *	
 *	sets the notification time
 *  from the prefrences struct,
 *  'remove_idle' should be true if
 *  any idle notification function is 
 *  set to run.
 *  
 *
 *  Paramaters:
 *   a boolean
 *  returns:
 *   none
 **/
void gui_set_notification_time(gboolean remove_idle);

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
void gui_notification_set_state(gboolean state);

void gui_defintion_text_view_init(GtkTextView* textview);

void gui_add_to_vocab_list(gchar* word);

gboolean gui_tree_view_is_empty();

gboolean gui_create_defintions_text_buffer(gchar* word, GtkTextBuffer* tbuffer);

void gui_create_status_icon_menu();

void gui_init_status_icon();

void gui_show_status_icon();

void gui_init_tree_view();

void gui_fill_vocab_list();

void gui_show_about_dialog();

/* Widget getter */
GtkWidget* gui_get_widget(gchar* widget_name);


#endif