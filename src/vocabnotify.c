#include "vocabnotify.h"
#include <fcntl.h>


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
gboolean vocabnotify_init()
{
	first_run = vocabnotify_is_first_run();
	if(first_run) {
		vocabnotify_create_config_directory();
		if(!vocabnotify_create_defaults()) {
			
			return FALSE;
		}
	}

	if(vocab_manager_init(vocabnotify_get_vocab_db_file(), FALSE) == -1) {
		g_critical("Unable to initialise vocab manager!\n");
		return FALSE;
	}

	if(!setting_manager_init(vocabnotify_get_config_file(), FALSE)) {
		g_critical("Unable to initialise setting manager!\n");
		return FALSE;
	} 

	return TRUE;
}

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
gboolean vocabnotify_is_first_run()
{
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, NULL);
	if(g_file_test(conf_path,  G_FILE_TEST_IS_DIR)) {
		g_free(conf_path);
		return FALSE;
	} 
	
	g_free(conf_path);
	return TRUE;
}

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
 *   TRUE if successful, FALSE otherwise
 **/
gboolean vocabnotify_create_config_directory()
{
	int mode = 064;
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, NULL);
	if(g_mkdir(conf_path,  0700) == 0) {
		//g_free(conf_path);
		return TRUE;
	}


	g_free(conf_path);
	return FALSE;		
}

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
gchar* vocabnotify_get_config_directory()
{
	gchar* conf_path = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, NULL);
	return conf_path;
}

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
gchar* vocabnotify_get_vocab_db_file()
{
	gchar* vocab_db_file = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, G_DIR_SEPARATOR_S, VOCAB_DB_FILE,  NULL);
	return vocab_db_file;
}

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
gchar* vocabnotify_get_config_file()
{
	gchar* conf_file = g_strconcat(g_get_user_config_dir(), G_DIR_SEPARATOR_S, PROGRAM_NAME, G_DIR_SEPARATOR_S, CONFIG_FILE,  NULL);
	return conf_file;
}

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
settings_t vocabnotify_get_default_settings()
{
	return default_settings;
}

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
gboolean vocabnotify_create_defaults()
{  

	/* Create vocabulary database */
	if(vocab_manager_create_database(vocabnotify_get_vocab_db_file(), FALSE) == -1) {
		g_critical("Unable to create vocabulary database!\n");
		return FALSE;
	}

	/* Create default settings */
	if(!setting_manager_init(vocabnotify_get_config_file(), first_run)) {
		g_critical("Unable to initialise setting manager!\n");
		return FALSE;
	}

	if(!setting_manager_save_settings(&default_settings)) {
		g_critical("Unable to save settings in memory!\n");
	}

	if(!setting_manager_save_settings_to_file()) {
		g_critical("Unable to save settings to file!\n");
		return FALSE;
	}

	setting_manager_cleanup();
	vocab_manager_cleanup();
	return TRUE;
}
