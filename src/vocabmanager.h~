#ifndef VOCABMANAGER_H_
#define VOCABMANAGER_H_

#include <glib.h>
#include <swni/swni.h>
#include <stdlib.h>
#include <sqlite3.h>

#define VOCAB_TABLE "vocab_words"
#define VOCAB_WORD_COLUMN "word"
#define MAX_BUFFER_SIZE 1024

/**
 *  vocab_manager_init()i
 *
 *  Initialises vocab manager database 
 * 	@Must be called first before any other operation
 *
 *  Paramaters:
 *    the path to the database file as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_init(char* dbname);

/**
 *  vocab_manager_add_word()
 *
 *  Adds a new word to the vocab list. 
 * 
 *
 *  Paramaters:
 *    the word you wish to add as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_add_word(char* word);

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
int vocab_manager_check_word(char* word);

/**
 *  vocab_manager_remove_word()
 *
 *  Removes a word from the database
 * 
 *
 *  Paramaters:
 *    the word you wish to remove as a string
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_remove_word(char* word);

/**
 *  vocab_manager_remove_all_words()
 *
 *  Removes all the words in the database
 * 
 *
 *  Paramaters:
 *    none
 *  returns:
 *      -1 on failure and 0 on success
 **/
int vocab_manager_remove_all_words();

/**
 *  vocab_manager_get_word_defintion()
 *
 *  Get's a defintion of a word using libswni 
 * 
 *
 *  Paramaters:
 *   the word as a string 
 *  returns:
 *      a valid pointer, or NULL on failure
 **/
swni_t* vocab_manager_get_word_defintion(char* word);

/**
 *  vocab_manager_get_words_list()
 *
 *  Gets a list of all the words in the vocab list
 * 
 *
 *  Paramaters:
 *    A GSList pointer that will hold the list
 *  returns:
 *      -1 on failure and 0 on success
 **/
GSList* vocab_manager_get_words_list();

/** 
 *  vocab_manager_word_list_free()
 *
 *  Free's memory that was allocated to 
 *  the elements in the list, and will
 *  free the list container memory too
 * 
 *  Paramaters:
 *    a GSlist pointer 
 *  returns:
 *      none
 **/
 void vocab_manager_word_list_free(GSList* slist);

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
void vocab_manager_free_word_defition(swni_t* swni_def);

/**
 *  vocab_manager_create_database()
 *
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
int vocab_manager_create_database(char* dbname, gboolean overwrite);

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
void vocab_manager_cleanup();
#endif