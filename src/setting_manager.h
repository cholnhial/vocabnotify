#ifndef SETTING_MANAGER_H_
#define SETTING_MANAGER_H_
#include <glib.h>

typedef struct {
	unsigned int notification_intervals_secs;
	unsigned int notification_expire_secs;
	gboolean play_sound_on_notification;
} settings_t;

enum {
	NOTIFICATION_INTERVALS_SECS, 
	NOTIFICATION_EXPIRE_SECS, 
	NOTIFICATION_PLAY_SOUND,
	SETTINGS_COMMENT,
	GROUP_SETTINGS,
	SETTINGS_LIST_MAX
};

static const char*  const settings_strings[SETTINGS_LIST_MAX] = {
[NOTIFICATION_INTERVALS_SECS] = "Notification_intervals_secs",
[NOTIFICATION_EXPIRE_SECS] = "Notification_expire_secs",
[NOTIFICATION_PLAY_SOUND] = "Notification_play_sound",
[SETTINGS_COMMENT] = "Vocabnotify Preference File",
[GROUP_SETTINGS] =	"Settings"
};


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
gboolean setting_manager_init(gchar* settings_file, gboolean first_run);

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
gboolean setting_manager_save_settings(settings_t* settings);

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
gboolean setting_manager_save_settings_to_file();

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
settings_t* setting_manager_get_settings();

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
void setting_manager_cleanup();
#endif 