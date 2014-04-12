#include <assert.h>

#include "vocabmanager.h"
#include "database.h"
/* globals */
static gboolean initialised = FALSE;
static sqlite3* db = NULL;
static GMutex db_access_mutex;

/* Functions Not apart of the interface */
static int is_initialised();
static void wait_for_db_access();
static void  lock_db_access();
static void  unlock_db_access();

/**
 *  Initialises vocab manager database 
 * 	@Must be called first before any other operation
 *
 *  Paramaters:
 *    the path to the database file as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_init(char* dbname) 
{
	db = database_open(dbname, FALSE);
	if(db == NULL) {
		return -1;
	}

	g_mutex_init(&db_access_mutex);

	initialised = TRUE;

	return 0;
}

/**
 *  Creates a sqlite database file that is the database
 * 
 *
 *  Paramaters:
 *    A path to the database file as a string, a boolean
 *    whether to overwrite existing database file.
 *
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_create_database(char* dbname, gboolean overwrite)
{

	if(overwrite == TRUE) {
		db = database_open(dbname, TRUE);
		if(db == NULL) {
			return -1;
		}
	} 
	else {
		db = database_open(dbname, FALSE);
		if(db == NULL) {
			return -1;
		}
	}

	char* query = "CREATE TABLE " VOCAB_TABLE "(word TEXT PRIMARY KEY)";
	if(database_execute(db, query) == -1) {
		
		return -1;
	}

	return 0;
}

/**
 *  Adds a new word to the vocab list. 
 * 
 *
 *  Paramaters:
 *    the word you wish to add as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_add_word(char* word)
{
	if (!is_initialised()) {
		return -1;
	}

	wait_for_db_access();
	lock_db_access();

	char* query = g_malloc(MAX_BUFFER);
	g_sprintf(query, "INSERT INTO %s VALUES('%s')", VOCAB_TABLE, word);
	/* execute the query */
	if(database_execute(db, query) == -1) {
		unlock_db_access();
		g_free(query);
		//printf("%s\n", database_get_last_error(db));
		return -1;
	}

	g_free(query);
	unlock_db_access();
	return 0;
}

/**
 *  vocab_manager_check_word()
 *
 *  Checks if a word is already in 
 *  the list.
 *
 *  Paramaters:
 *    the word you wish to add as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_check_word(char* word)
{
	if (!is_initialised()) {
		return -1;
	}

	wait_for_db_access();
	lock_db_access();
	int oid = -1;
	char* query = g_malloc(MAX_BUFFER);
	g_sprintf(query, "SELECT oid FROM %s WHERE word='%s'", VOCAB_TABLE, word);
	/* execute the query */
	if(database_generic_get(db, query, word, 1, INT, &oid, 0) == -1) {
		unlock_db_access();
		g_free(query);
		//printf("%s\n", database_get_last_error(db));
		return -1;
	}

	/* checks if it's a valid word */
	if(oid > -1) {
		unlock_db_access();
		g_free(query);
		return -1;
	}

	g_free(query);
	unlock_db_access();
	return 0;

}
/**
 *  Removes a word from the database
 * 
 *
 *  Paramaters:
 *    the word you wish to remove as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_remove_word(char* word)
{
	if (!is_initialised()) {
		return -1;
	}

	wait_for_db_access();
	lock_db_access();

	char* query = g_malloc(MAX_BUFFER);
	g_sprintf(query, "DELETE FROM %s WHERE %s = '%s'", VOCAB_TABLE, VOCAB_WORD_COLUMN, word);
	/* execute the query */
	if(database_execute(db, query) == -1) {
		unlock_db_access();
		g_free(query);
		return -1;
	}

	g_free(query);
	unlock_db_access();

	return 0;
}

/**
 *  Removes all the words in the database
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_remove_all_words()
{
	if (!is_initialised()) {
		return -1;
	}

	wait_for_db_access();
	lock_db_access();

	char* query = "DROP TABLE " VOCAB_TABLE;
	/* execute the query */
	if(database_execute(db, query) == -1) {
		unlock_db_access();
		return -1;
	}

	/* recreate the table, but this time 
	it's going to be empty */

	query = "CREATE TABLE " VOCAB_TABLE "(word TEXT PRIMARY KEY)";
	if(database_execute(db, query) == -1) {
		unlock_db_access();
		return -1;
	}

	unlock_db_access();
	return 0;
}

/**
 *  Get's a defintion of a word using libswni 
 * 
 *
 *  Paramaters:
 *   the word as a string 
 *  returns:
 *      a valid pointer, or NULL on failure
 **/
swni_t* vocab_manager_get_word_defintion(char* word)
{	wait_for_db_access();
	lock_db_access();
	swni_t* swni_defs = swni_get_definitions_list(word);
	unlock_db_access();
	return swni_defs;
}

/**
 *  vocab_manager_get_words_list()
 *
 *  Gets a list of all the words in the vocab list
 * 
 *
 *  Paramaters:
 *    A GSList pointer that will hold the list
 *  returns:
 *      NULL on failure, a GList filled.
 **/
GSList* vocab_manager_get_words_list()
{	
	if (!is_initialised()) {
		return -1;
	}

	wait_for_db_access();
	lock_db_access();

	GSList* list;
	char* query = "SELECT word FROM " VOCAB_TABLE;
	if((list = database_generic_get_str_list(db, query)) == NULL) {
		unlock_db_access();
		return NULL;
	}

	unlock_db_access();

	return list;
}

/** 
 *  vocab_manager_free_word_defition()
 *
 *  Free's memory that was allocated by libswni
 * 
 *
 *  Paramaters:
 *    A pointer to  swni definition
 *  returns:
 *      none
 **/
void vocab_manager_free_word_defition(swni_t* swni_def) 
{	
	assert(swni_def != NULL);
	
	swni_wni_free();
	swni_destroy_definitions_list(swni_def->definitions_list);
	swni_destroy_synonyms_list(swni_def->synonyms_list);
}

/**
 *  Free's allocated memory, after this it can
 *  not be used
 * 	
 *
 *  Paramaters:
 *    none
 *  returns:
 *      none
 **/
void vocab_manager_cleanup()
{	
	if(!is_initialised()) {
		sqlite3_close(db);
		initialised = FALSE;
		return;
	}
	wait_for_db_access();
	lock_db_access();
	sqlite3_close(db);
	g_mutex_free(&db_access_mutex);
	initialised = FALSE;
}

/** 
 *  vocab_manager_word_list_free()
 *
 *  Free's memory that was allocated to 
 *  the elements in the list, and will
 *  free the list container memory too,
 *  this function does not need 'vocab_manager_init()'
 *  to work.
 * 
 *  Paramaters:
 *    a GSlist pointer 
 *  returns:
 *      none
 **/
 void vocab_manager_word_list_free(GSList* slist)
 {  
 	assert(slist != NULL);

 	for(GSList* iter = slist; iter != NULL; iter = iter->next) {
        g_free(iter->data);
    }
    g_slist_free(slist);
 }

/* Not apart of the interface */
int is_initialised()
{
	return initialised;
}

void  lock_db_access() {

	g_mutex_lock(&db_access_mutex);
}

void  unlock_db_access() {

	g_mutex_unlock(&db_access_mutex);
}

void wait_for_db_access() {
	/* Wait until the mutex is unlocked */
	while(!g_mutex_trylock(&db_access_mutex));
	g_mutex_unlock(&db_access_mutex);
}