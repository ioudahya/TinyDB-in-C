// Oudahya Ismaïl
// 000479390
// projet os
#ifndef _DB_H
#define _DB_H
#include <time.h>
#include <stdlib.h>
#include "student.h"

/**
 * Database structure type.
 */
typedef struct {
    student_t *data; /** The list of students **/
    size_t lsize;    /** The logical size of the list **/
    size_t psize;    /** The physical size of the list **/
} database_t;

/** 
 *  Add a student to the database.
 * TODO: implement this function.
 **/
void db_add(database_t *db, student_t *s,FILE *fd,int *new_count);

/**
 * Delete a student from the database.
 * TODO: implement this function.
 **/
//void db_delete(database_t *db, student_t *s,FILE *fd,int *count);

/**
 * Save the content of a database_t to the specified file.
 * TODO: implement this function
 **/
void db_save(database_t *db, const char *path);

/**
 * Load the content of a database of students from a file.
 * TODO: implement this function.
 **/
void db_load(database_t *db, const char *path);

/**
 * Initialise a database_t structure.
 * Typical use:
 * ```
 * database_t db;
 * db_init(&db);
 * ```
 * TODO: implement this function.
 **/
void db_init(database_t *db);
/**
 * Ces fonctions concerne les updates de la database dependant des resultats du parsing on met à jour
 * la database donc on la parcours comme une liste et on remplace.
 * @param db  struct database
 * @param s  struct student
 * @param fd fichier à ouvrir
 * @param count compteur qui est incrémenter à chaque étudiant trouvé.
 */
void db_update_id(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count);
void db_update_fname(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count);
void db_update_lname(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count);
void db_update_section(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count);
void db_update_birthdate(database_t *db,student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count);


/**
 * Ces fonctions sont assez simple d'utilisation , on parcours la database comme une liste et on vérifie le cela correspond
 * aux resultats du parsing et ensuite on le rajoute aux fichiers.
 * @param db  struct database
 * @param s  struct student
 * @param fd fichier à ouvrir
 * @param count compteur qui est incrémenter à chaque étudiant trouvé.
 */
void db_select_id(database_t *db,student_t *s,FILE *fd,int *count);
void db_select_fname(database_t *db,student_t *s,FILE *fd,int *count);
void db_select_lname(database_t *db,student_t *s,FILE *fd,int *count);
void db_select_section(database_t *db,student_t *s,FILE *fd,int *count);
void db_select_birthday(database_t *db,student_t *s,FILE *fd,int *count);
/**
 * Elle permet de supprimer un étudiant dans la base de données et de tout bouger pour qu'on ait pas de
 * trou dans la base de données et ces différentes fonctions gère chaque informations que ce soit le nom,prénom,id...
 * @param db la database
 * @param s  struct student
 * @param fd fichier à ouvrir
 * @param new_count  compteur qui est incrémenter à chaque étudiant trouvé.
 */
void db_delete_birthdate(database_t *db,student_t *s,FILE *fd,int *new_count);
void db_delete_id(database_t *db,student_t *s,FILE *fd,int *new_count);
void db_delete_fname(database_t *db,student_t *s,FILE *fd,int *new_count);
void db_delete_lname(database_t *db,student_t *s,FILE *fd,int *new_count);
void db_delete_section(database_t *db,student_t *s,FILE *fd,int *new_count);
/**
 *
 * @param db
 * @param fd
 * @param count
 * cette fonction permet d'écrire dans le fichier gere_database.txt
 */
void write_file(database_t *db,FILE *fd,int count);

#endif
