#ifndef VOCABNOTIFY_H_
#define VOCABNOTIFY_H_

#include <glib.h>
#include "setting_manager.h"

#define PROGRAM_NAME "vocabnotify" /* executable name too*/
#define PROGRAM_NAME_DESKTOP "VocabNotify"
#define PROGRAM_VERSION "0.1"
#define CONFIG_FILE "vocabnotify.conf"
#define VOCAB_DB_FILE "vocab.db"

/* @globals */
static settings_t default_settings = {1800, 5, TRUE};
static gboolean first_run = FALSE;
/**
 *  vocabnotify_init()
 *	
 *
 *  Initialises vocabmanager and setting_manager
 *  if the program is been run for the first time
 *  then it will create the defaults.
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    TRUE if it's the first time FALSE otherwise.
 **/
gboolean vocabnotify_init();

/**
 *  vocabnotify_is_first_run()
 *	
 *
 *  Checks if this is the first time the
 *  program has been ran.
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    TRUE if it's the first time FALSE otherwise.
 **/
gboolean vocabnotify_is_first_run();

/**
 *  vocabnotify_create_config_directory()
 *	
 *
 *  creates the configuration directory
 *  
 * 
 *
 *  Paramaters:
 *   none
 *  returns:
 *    TRUE if successful, FALSE otherwise
 **/
gboolean vocabnotify_create_config_directory();

/**
 *  vocabnotify_get_config_directory()
 *	
 * 	Returns a full path to the configuration
 *  directory
 *  
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *    a string
 **/
gchar* vocabnotify_get_config_directory();


/**
 *  vocabnotify_get_vocab_db_file()
 *	
 * 	Returns a full path to the vocabulary 
 *  database file 
 *  
 *
 *  Paramaters:
 *    none
 *  returns:
 *    a string
 **/
gchar* vocabnotify_get_vocab_db_file();

/**
 *  vocabnotify_get_config_file()
 *	
 * 	Returns a full path to the configuration
 *  file
 *  
 *
 *  Paramaters:
 *    none
 *  returns:
 *    a string
 **/
gchar* vocabnotify_get_config_file();

/**
 *  vocabnotify_get_default_settings()
 *	
 * 	Returns the default settings 
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *    settings_t
 **/
settings_t vocabnotify_get_default_settings();

/**
 *  vocabnotify_create_defaults()
 *	
 * 	Will create the vocab.db and vocabnotify.conf 
 *  and set the default settings
 *
 *  Paramaters:
 *    none
 *  returns:
 *    TRUE success, FALSE otherwise
 **/
gboolean vocabnotify_create_defaults();

/* Not part of the general interface */
#endif