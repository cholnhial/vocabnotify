#ifndef NOTIFIER_H_
#define NOTIFIER_H_

#define BORDER   6

/**
 *  notifier_dialog_init()
 *
 *  creates the notification dialog
 *  and makes it ready for us
 *
 *  Paramaters:
 *    none
 *  returns:
 *    none
 **/
void notifier_dialog_init();

/**
 *  notifier_dialog_show()
 *
 *  Shows the dialog window and sets
 *  the word as a part of the title
 *
 *  Paramaters:
 *   A string
 *  returns:
 *    none
 **/
void notifier_dialog_show(gchar* word);

/**
 *  notifier_dialog_hide()
 *
 *  Hides the notification dialog.
 *  It will also restore the display
 *  to it's normal state before fadeout
 *
 *  Paramaters:
 *   none
 *  returns:
 *    none
 **/
void notifier_dialog_hide();

/**
 *  notifier_dialog_get_text_view()
 *
 *  Returns the text view widget 
 *  of the notification dialog.
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *   returns a GtkTextView pointer
 **/
GtkTextView* notifier_dialog_get_text_view();

/**
 *  notifier_dialog_is_running()
 *
 *  Checks if the notification dialog
 *  is running, returns TRUE if so, FALSE
 *  otherwise.
 *  
 *
 *  Paramaters:
 *   none
 *  returns:
 *    a boolean value TRUE or FALSE
 **/
gboolean notifier_dialog_is_running();

static void notifier_ungrab_input();

#endif
