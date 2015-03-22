
#include <gtk/gtk.h>

#include "notifier.h"
#include "gui.h"
#include "vocabnotify.h"
#include "xfsm-fadeout.h"

static GtkWidget* notification_dialog;
static GtkWidget* notitifcation_text_view;
static GtkWidget* main_vbox;
static GtkWidget* scrolled_window;
static GtkWidget* notification_title_label;
static gboolean dialog_running = FALSE;

/**
 * notifier_dialog_screenshot_new()
 *   
 *  Don't let the function of this piece of code
 *  scare you. The purpose of this funciton is
 *  to darken the screen upon notification
 * 
 *  Parameters:
 *	  GdkScreen: the screen which the notification will be on
 *		by default this is the screen which is active
 *	Returns:
 *	  GdkPixbuf: the darken screen area
 * 
 **/
static GdkPixbuf *
notifier_dialog_screenshot_new (GdkScreen *screen)
{
	GdkRectangle  rect, screen_rect;
	GdkWindow    *window;
	GdkPixbuf    *screenshot;
	gint          x, y;

	g_return_val_if_fail (GDK_IS_SCREEN (screen), NULL);

	screen_rect.x = 0;
	screen_rect.y = 0;
	screen_rect.width = gdk_screen_get_width (screen);
	screen_rect.height = gdk_screen_get_height (screen);

	window = gdk_screen_get_root_window (screen);
	//gdk_drawable_get_size (GDK_DRAWABLE (window), &rect.width, &rect.height);
	rect.height = gdk_window_get_height(window);
	rect.width =  gdk_window_get_width(window);
	gdk_window_get_origin (window, &x, &y);

	rect.x = x;
	rect.y = y;

	if (!gdk_rectangle_intersect (&rect, &screen_rect, &rect))
		return NULL;

	screenshot = gdk_pixbuf_get_from_window    (window,
	                                            0, 0,
	                                            rect.width,
	                                            rect.height);

	gdk_display_beep (gdk_screen_get_display (screen));

	return screenshot;
}


/***
 *  notifier_dialog_init()
 * 
 *  This function is responsible for initializing 
 *  the popup window that is the heart of vocanotify
 *  
 *  Parameters:
 *	  none
 *  Returns:
 *	  none
 * 
 * */
void notifier_dialog_init()
{

	PangoAttrList *attrs;
	GtkWidget     *separator;

	notification_dialog = gtk_dialog_new();

	gtk_dialog_set_default_response (GTK_DIALOG (notification_dialog), GTK_RESPONSE_CLOSE);
	// gtk_dialog_set_has_separator (GTK_DIALOG (notification_dialog), FALSE);
	gtk_window_set_position (GTK_WINDOW (notification_dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(notification_dialog), 450, 380);
	// gtk_window_set_resizable (GTK_WINDOW (notification_dialog), FALSE);
	gtk_window_set_decorated(GTK_WINDOW(notification_dialog), FALSE);

	main_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, BORDER);
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG (notification_dialog))), main_vbox, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (main_vbox), BORDER);
	gtk_widget_show (main_vbox);

	notification_title_label = gtk_label_new ("");
	gtk_label_set_justify (GTK_LABEL (notification_title_label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start (GTK_BOX (main_vbox), notification_title_label, FALSE, TRUE, 0);
	gtk_widget_show (notification_title_label);


	attrs = pango_attr_list_new ();
	pango_attr_list_insert (attrs, pango_attr_scale_new (PANGO_SCALE_LARGE));
	pango_attr_list_insert (attrs, pango_attr_weight_new (PANGO_WEIGHT_BOLD));
	gtk_label_set_attributes (GTK_LABEL (notification_title_label), attrs);
	pango_attr_list_unref (attrs);

	separator = gtk_separator_new ( GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (main_vbox), separator, FALSE, TRUE, 0);
	gtk_widget_show (separator);

	/* Create the textview and put it into a scrolled window */

	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
	                               GTK_POLICY_AUTOMATIC,
	                               GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);

	/* init textview */
	notitifcation_text_view = gui_get_widget(NOTIFIER_DEFINITION_TEXT_VIEW);

	/* initialize the textview as a definiton one,
	 this is for the format */
	gui_defintion_text_view_init(notitifcation_text_view);

	/* add text view */
	gtk_container_add(GTK_CONTAINER(scrolled_window), notitifcation_text_view);

	gtk_widget_show_all(scrolled_window);

	/* add scrolled window to main vbox */
	gtk_box_pack_start (GTK_BOX (main_vbox), scrolled_window, TRUE, TRUE, 0);

	/* add a close button */

	gtk_dialog_add_button(GTK_DIALOG(notification_dialog), GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);

}


/***
 *  notifier_grap_input()
 * 
 *  It's obvious this function is just going
 *  to grab the keyboard allowing one to press
 *  the (ESC) key to let the window go away
	 
 *  
 *  Parameters:
 *	  none
 *  Returns:
 *	  none
 * 
 * */
static void notifier_grab_input()
{
	GdkDisplay* display = (GdkDisplay *) NULL;
	GdkDevice* pointer = (GdkDevice*) NULL;
	GdkDevice* keyboard = (GdkDevice*) NULL;
	GdkWindow* window;
	GdkDeviceManager* dmanager = (GdkDeviceManager*) NULL;
	/* Grab input */
	gtk_widget_show_now (GTK_WIDGET (notification_dialog));
	window = gtk_widget_get_window (GTK_WIDGET (notification_dialog));
	display = gdk_display_get_default();
	notifier_ungrab_input();
	dmanager =  gdk_display_get_device_manager(display);
	pointer = gdk_device_manager_get_client_pointer(dmanager);
	keyboard = gdk_device_get_associated_device (pointer);
	gdk_error_trap_push ();
	int res;
	if (gdk_device_grab (keyboard, window, GDK_OWNERSHIP_WINDOW, FALSE, gdk_window_get_events(window), NULL, GDK_CURRENT_TIME) != GDK_GRAB_SUCCESS)
		g_critical ("Failed to grab the keyboard for logout window");

	gdk_flush();

	if (gdk_error_trap_pop ())
	{
		g_critical ("Grab keyboard error\n");
	}

}

/***
 *  notifier_ungrap_input()
 * 
 *  This does the reverse of what we did above
 *   
 *  
 *  Parameters:
 *	  none
 *  Returns:
 *	  none
 * 
 * */
static void notifier_ungrab_input()
{
	GdkDisplay* display = (GdkDisplay *) NULL;
	GdkDevice* pointer = (GdkDevice*) NULL;
	GdkDevice* keyboard = (GdkDevice*) NULL;
	GdkWindow* window;
	GdkDeviceManager* dmanager = (GdkDeviceManager*) NULL;
	/* Ungrab input */
	window = gtk_widget_get_window (GTK_WIDGET (notification_dialog));
	display = gdk_display_get_default();
	dmanager =  gdk_display_get_device_manager(display);
	pointer = gdk_device_manager_get_client_pointer(dmanager);
	pointer = gdk_device_manager_get_client_pointer(dmanager);
	keyboard = gdk_device_get_associated_device (pointer);
	gdk_error_trap_push ();

	gdk_device_ungrab (keyboard, GDK_CURRENT_TIME);
	gdk_flush ();
	if (gdk_error_trap_pop ())
	{
		/* this probably means the device table is outdated,
		 * e.g. this device doesn't exist anymore.
		 */
		g_critical ("Ungrab pointer device error\n");
	}
}


/***
 *  notifier_window_add_border()
 * 
 *  Don't know if it's really used, but
 *  it's suppose to add a border around the 
 *  popup, some sort of styling
 * 
 *  Parameters:
 *	  GtkWindow: the popup window 
 *  Returns:
 *	  none
 * 
 * */
static void notifier_window_add_border (GtkWindow *window)
{
	GtkWidget *box1, *box2;

	gtk_widget_realize(GTK_WIDGET(window));

	box1 = gtk_event_box_new ();
	gtk_widget_modify_bg (box1, GTK_STATE_NORMAL,
	                      &gtk_widget_get_style(GTK_WIDGET(window))->bg [GTK_STATE_SELECTED]);
	gtk_widget_show (box1);

	box2 = gtk_event_box_new ();
	gtk_widget_show (box2);
	gtk_container_add (GTK_CONTAINER (box1), box2);

	gtk_container_set_border_width (GTK_CONTAINER (box2), 6);
	gtk_widget_reparent (gtk_bin_get_child(GTK_BIN (window)), box2);

	gtk_container_add (GTK_CONTAINER (window), box1);
}


/***
 *  notifier_dialog_show()
 * 
 *  This one is the most obvious,
 *  and also the pinical point of
 *  this module. It's main job
 *  it to put together all functions
 *  to display our nice little popup
 *  box which wouldn't have been possible
 *  without the XFCE (logout window) code.
 * 
 *  
 * 
 *  Parameters:
 *	  gchar*: the word to grab dictionary meaning for and display
 *  Returns:
 *	  none
 * 
 * */
void notifier_dialog_show(gchar* word)
{	
	GdkScreen* screen;
	GtkWidget* hidden;
	GdkDisplay* display = (GdkDisplay *) NULL;
	GdkDevice* device = (GdkDevice*) NULL;
	GdkDeviceManager* dmanager = (GdkDeviceManager*) NULL;
	GdkPixbuf        *screenshot = NULL;
	XfsmFadeout      *fadeout = NULL;

	gchar* notification_title = g_strdup_printf("%s - %s", PROGRAM_NAME_DESKTOP, word);

	/* set the actual title */

	gtk_label_set_text(GTK_LABEL(notification_title_label), notification_title);


	/* Prep before show dialog window */
	screen = gdk_screen_get_default ();


	/* display fadeout */
	fadeout = xfsm_fadeout_new (gdk_screen_get_display (screen));
	gdk_flush ();

	/* Raise the window and a bunch of other things */
	gtk_widget_realize (notification_dialog);
	gdk_window_set_override_redirect (gtk_widget_get_window(notification_dialog), TRUE);
	gdk_window_raise (gtk_widget_get_window(notification_dialog));


	/* Grab input */
	notifier_grab_input();

	/* Run the dialog */
	dialog_running = TRUE;

	int res = gtk_dialog_run(GTK_DIALOG(notification_dialog));

	switch(res) {
		case GTK_RESPONSE_DELETE_EVENT:
			gtk_widget_hide(notification_dialog);
			xfsm_fadeout_destroy(fadeout);
			break;
		case GTK_RESPONSE_CLOSE:
		{
			/* don't leave artifacts on the background window */
			gtk_widget_hide(notification_dialog);
			xfsm_fadeout_destroy(fadeout);
			break;
		}

	}

	dialog_running = FALSE;

	notifier_ungrab_input();

	g_free(notification_title);
}

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
void notifier_dialog_hide()
{

	gtk_dialog_response(notification_dialog, GTK_RESPONSE_CLOSE);

}

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
 *    returns a GtkTextView pointer
 **/

GtkTextView* notifier_dialog_get_text_view()
{
	return GTK_TEXT_VIEW(notitifcation_text_view);
}

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
gboolean notifier_dialog_is_running()
{
	return dialog_running;
}