#include "setting_manager.h"
#include <string.h>

/* globals */
static GKeyFile* config_file = NULL;
static gboolean initialised = FALSE;
static settings_t* settings = NULL;
static gchar* settings_file_path = NULL;

/* Functions Not apart of the interface */
static gboolean is_initialised();

/**
 *  setting_manager_init()
 *	@Must be called first
 *
 *  Loads application settings from a file
 * 
 *
 *  Paramaters:
 *   the settings file path
 *  returns:
 *    FALSE on failure, TRUE on success
 **/
gboolean setting_manager_init(gchar* settings_file, gboolean first_run)
{
	config_file = g_key_file_new();
	if(!config_file) {
		g_error("Error creating GKeyFile to initialise preferences");
	}

	settings = g_malloc(sizeof(settings_t));
	settings_file_path = g_strdup(settings_file);

	if(!first_run) {
		if(g_key_file_load_from_file(config_file,
	 	settings_file,
	 	G_KEY_FILE_KEEP_COMMENTS,
	 	NULL)) {
			settings->notification_intervals_secs = g_key_file_get_integer(config_file,
									  settings_strings[GROUP_SETTINGS],
									  settings_strings[NOTIFICATION_INTERVALS_SECS],
									  							 NULL);
			settings->notification_expire_secs = g_key_file_get_integer(config_file,
											 settings_strings[GROUP_SETTINGS],
											 settings_strings[NOTIFICATION_EXPIRE_SECS],
																	  NULL);
			settings->play_sound_on_notification = g_key_file_get_boolean(config_file,
											 settings_strings[GROUP_SETTINGS],
											 settings_strings[NOTIFICATION_PLAY_SOUND],
																	  NULL);

		} else {
			return FALSE;
		}
	}

	initialised = TRUE;

	return TRUE;
}

/**
 *  setting_manager_save_settings()
 *
 *  Saves the application settings in memory
 * 
 *
 *  Paramaters:
 *    a pointer to the settings
 *  returns:
 *    FALSE on failure, TRUE on success
 **/
gboolean setting_manager_save_settings(settings_t* settingsp)
{
	g_assert(settings != NULL);
	g_assert(config_file != NULL);
	if(!is_initialised()) {
		return FALSE;
	}

	g_key_file_set_comment(config_file, 
						    NULL, 
							NULL,
	 						settings_strings[SETTINGS_COMMENT], NULL);

	g_key_file_set_integer(config_file, 
							settings_strings[GROUP_SETTINGS], 
							settings_strings[NOTIFICATION_INTERVALS_SECS],
	 						settingsp->notification_intervals_secs);
	g_key_file_set_integer(config_file, 
							settings_strings[GROUP_SETTINGS],
							settings_strings[NOTIFICATION_EXPIRE_SECS],
	 						settingsp->notification_expire_secs);
	g_key_file_set_boolean(config_file, 
							settings_strings[GROUP_SETTINGS],
							settings_strings[NOTIFICATION_PLAY_SOUND],
	 						settingsp->play_sound_on_notification);

	settings->notification_intervals_secs = settingsp->notification_intervals_secs;
	settings->notification_expire_secs = settingsp->notification_expire_secs;
	settings->play_sound_on_notification = settingsp->play_sound_on_notification;

	return TRUE;
}

/**
 *  setting_manager_save_settings_to_file()
 *
 *  Saves the application to the configuration file
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *    FALSE on failure, TRUE on success
 **/
gboolean setting_manager_save_settings_to_file()
{
	if(!is_initialised()) {
		return FALSE;
	}
	gsize file_len = 0;
	gchar* file_contents = g_key_file_to_data(config_file, &file_len, NULL);

	GError* err = NULL;

	if(!file_contents) {
		g_error("Error saving preferences to file");
	}

	if(!g_file_set_contents(settings_file_path, file_contents, file_len, &err)) {
		g_warning("Failed to save preferences to file");
		g_error_free(err);
		err = NULL;
	}

	g_free(file_contents);
	file_contents = NULL;
}

/**
 *  setting_manager_get_settings()
 *
 *  Gets the application settings that
 *  have been loaded into memory
 *
 *  Paramaters:
 *    none
 *  returns:
 *    A setting_t pointer, NULL on failure.
 **/
settings_t* setting_manager_get_settings()
{
	settings_t* local_settings = g_malloc(sizeof(settings_t));

	memcpy(local_settings, settings, sizeof(settings_t));

	return local_settings;
}

/**
 *  setting_manager_cleanup()
 *
 *  Frees any memory that was allocated.
 *  After this setting manager should not
 *  be used.
 *
 *  Paramaters:
 *    none
 *  returns:
 *    none
 **/
void setting_manager_cleanup()
{
	g_free(settings);
	g_free(settings_file_path);
	g_key_file_free(config_file);
	initialised = FALSE;
}
/* Not a part of the interface */
gboolean is_initialised() 
{
	return initialised;
}
