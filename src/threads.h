#ifndef THREADS_H_
#define THREADS_H_

#include <glib.h>

/**
 *  add_vocab_thread()
 *	
 *	Adds a new vocabulary to the 
 *  database
 * 
 *
 *  Paramaters:
 *   gpointer user provided data
 *  returns:
 *   none
 **/
gpointer add_vocab_thread(gpointer user_data);

gpointer fill_vocab_list_thread(gpointer user_data);

gboolean add_vocab_thread_idle(gpointer user_data);

gboolean fill_vocab_list_thread_idle(gpointer user_data);
#endif