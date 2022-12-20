// Oudahya Ismaïl
// 000479390
// projet os
#include <stdio.h>
#include "db.h"


void db_init(database_t *db)
{
    // La fonction initialise la database donc on initialise db->data et les tailles physique et logique.
    db->data =(student_t*)malloc(sizeof(student_t));
    if(!db->data)
    {
        perror("Erreur de malloc!\n");
        exit(1);
    }
    db->lsize=0;
    db->psize=0;
}

void db_load(database_t *db,const char *path)
{
    // appel db_init pour initialiser et ensuite on charge la base de données en allouant de la mémoire et en parcourant
    // la base de données grâce à fread et en paramètre elle prends le taille de la structure student_t. Si il n'y a pas assez
    // de place alors on realloc , sans oublier d'incrémenter la taille physique et logique
    db_init(db);
    student_t *s;
    student_t *var_temp;
    s = (student_t*)malloc(sizeof(student_t));
    if (!s)
    {
        perror("Erreur de malloc!\n");
        exit(1);
    }
    FILE *pFile;
    pFile=fopen(path, "rb");
    while(fread(s,sizeof(student_t),1,pFile) != 0)
    {
        if (db->lsize == 0 )
        {
            db->data[db->psize] = *s;
            db->lsize+=1;
            db->psize+=1;
        }
        else{
            db->lsize +=1;
            var_temp = realloc(db->data,((db->lsize) * sizeof(student_t))*2); // je met dans une variable pour que quand le malloc à une erreur il efface pas toute la db
            if (!var_temp)
            {
                perror("Erreur de malloc!\n");
                exit(1);
            }
            db->data = var_temp;
            db->data[db->psize] = *s;
            db->psize +=1;
        }

    }
    fclose(pFile);


}

void db_add(database_t *db, student_t *s,FILE *fd,int *new_count)
{
    student_t *var_temp;
    *new_count = *new_count +1;
    db->lsize+=1;
    var_temp = realloc(db->data,(db->lsize) * sizeof(student_t));
    if (!var_temp) // gestion d'erreur pour ne pas perdre toute la base de données dans le cas ou l'allocation foire.
    {
        perror("Erreur de malloc!\n");
        exit(1);
    }
    db->data = var_temp;
    db->data[db->psize] = *s;
    fprintf(fd,"%u: %s %s in section %s, born on the %d/%d/%d\n", s->id, s->fname,s->lname, s->section, s->birthdate.tm_mday, s->birthdate.tm_mon+1,s->birthdate.tm_year+1900);
    db->psize+=1;
}

void db_save(database_t *db, const char *path)
{
    FILE *pFile;
    pFile=fopen(path,"wb");  // open for appending (file need not exist)

    for (int i=0;i<db->psize;i++)
    {
        fwrite(&db->data[i],sizeof(student_t),1,pFile);
    }

    fclose(pFile);
}

void db_update_id(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (db->data[i].id ==  (unsigned)atol(value_filter))
        {

            if (strcmp(field_to_update,"fname") ==0)
            {
                strcpy(db->data[i].fname,update_value);
            }
            if (strcmp(field_to_update,"lname") ==0)
            {
                strcpy(db->data[i].lname,update_value);
            }
            if (strcmp(field_to_update,"section") ==0)
            {
                strcpy(db->data[i].section,update_value);
            }
            if (strcmp(field_to_update,"id") ==0)
            {
                db->data[i].id = (unsigned)atol(update_value);
            }
            if (strcmp(field_to_update,"birthdate") ==0)
            {
                db->data[i].birthdate.tm_mday = s->birthdate.tm_mday;
                db->data[i].birthdate.tm_mon = s->birthdate.tm_mon;
                db->data[i].birthdate.tm_year = s->birthdate.tm_year;
            }
            write_file(db,fd,i);
            *new_count = *new_count +1;

        }
    }
}

void db_update_fname(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(db->data[i].fname,value_filter) ==0)
        {
            if (strcmp(field_to_update,"fname") ==0)
            {
                strcpy(db->data[i].fname,update_value);
            }
            if (strcmp(field_to_update,"lname") ==0)
            {
                strcpy(db->data[i].lname,update_value);
            }
            if (strcmp(field_to_update,"section") ==0)
            {
                strcpy(db->data[i].section,update_value);
            }
            if (strcmp(field_to_update,"id") ==0)
            {
                db->data[i].id = (unsigned)atol(update_value);
            }
            if (strcmp(field_to_update,"birthdate") ==0)
            {
                db->data[i].birthdate.tm_mday = s->birthdate.tm_mday;
                db->data[i].birthdate.tm_mon = s->birthdate.tm_mon;
                db->data[i].birthdate.tm_year = s->birthdate.tm_year;
            }
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}


void db_update_lname(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(db->data[i].lname,value_filter) ==0)
        {
            if (strcmp(field_to_update,"fname") ==0)
            {
                strcpy(db->data[i].fname,update_value);
            }
            if (strcmp(field_to_update,"lname") ==0)
            {
                strcpy(db->data[i].lname,update_value);
            }
            if (strcmp(field_to_update,"section") ==0)
            {
                strcpy(db->data[i].section,update_value);
            }
            if (strcmp(field_to_update,"id") ==0)
            {
                db->data[i].id = (unsigned)atol(update_value);
            }
            if (strcmp(field_to_update,"birthdate") ==0)
            {
                db->data[i].birthdate.tm_mday = s->birthdate.tm_mday;
                db->data[i].birthdate.tm_mon = s->birthdate.tm_mon;
                db->data[i].birthdate.tm_year = s->birthdate.tm_year;
            }
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_update_section(database_t *db, student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(db->data[i].section,value_filter) ==0)
        {
            if (strcmp(field_to_update,"fname") ==0)
            {
                strcpy(db->data[i].fname,update_value);
            }
            if (strcmp(field_to_update,"lname") ==0)
            {
                printf("ici lname\n");
                strcpy(db->data[i].lname,update_value);
            }
            if (strcmp(field_to_update,"section") ==0)
            {
                printf("ici section\n");
                strcpy(db->data[i].section,update_value);
            }
            if (strcmp(field_to_update,"id") ==0)
            {
                db->data[i].id = (unsigned)atol(update_value);
            }
            if (strcmp(field_to_update,"birthdate") ==0)
            {
                db->data[i].birthdate.tm_mday = s->birthdate.tm_mday;
                db->data[i].birthdate.tm_mon = s->birthdate.tm_mon;
                db->data[i].birthdate.tm_year = s->birthdate.tm_year;
            }
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_update_birthdate(database_t *db,student_t *s,char *value_filter,char *field_to_update,char *update_value,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (db->data[i].birthdate.tm_mday == s->birthdate.tm_mday && db->data[i].birthdate.tm_mon == s->birthdate.tm_mon && db->data[i].birthdate.tm_year == s->birthdate.tm_year)
        {
            if (strcmp(field_to_update,"fname") ==0)
            {
                strcpy(db->data[i].fname,update_value);
            }
            if (strcmp(field_to_update,"lname") ==0)
            {
                strcpy(db->data[i].lname,update_value);
            }
            if (strcmp(field_to_update,"section") ==0)
            {
                strcpy(db->data[i].section,update_value);
            }
            if (strcmp(field_to_update,"id") ==0)
            {
                db->data[i].id = (unsigned)atol(update_value);
            }
            if (strcmp(field_to_update,"birthdate") ==0)
            {
                char *mois_str=strtok(update_value, "/");
                char *jour_str=strtok(NULL, "/");
                char *annee_str=strtok(NULL, "/");

                int jour_int = atol(jour_str);
                int mois_int = atol(mois_str);
                int annee_int = atol(annee_str);

                db->data[i].birthdate.tm_mday = jour_int;
                db->data[i].birthdate.tm_mon = mois_int;
                db->data[i].birthdate.tm_year = annee_int;
            }
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_select_id(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++) {
        if (s->id == db->data[i].id) {
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_select_fname(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {

        if (strcmp(s->fname,db->data[i].fname) == 0)
        {
            *new_count = *new_count+1;
            write_file(db,fd,i);
        }
    }
}

void db_select_lname(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(s->lname,db->data[i].lname) == 0)
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_select_section(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(s->section,db->data[i].section) == 0)
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_select_birthday(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (s->birthdate.tm_mday == db->data[i].birthdate.tm_mday && s->birthdate.tm_mon == db->data[i].birthdate.tm_mon && s->birthdate.tm_year == db->data[i].birthdate.tm_year)
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
        }
    }
}

void db_delete_lname(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(s->lname,db->data[i].lname) == 0) // test pour le nom
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
            int indice=i;
            for(int j=indice;j < db->psize;j++)
            {
                db->data[j] = db->data[j+1];
            }
            db->psize = db->psize-1;

        }
    }
}

void db_delete_fname(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(s->fname,db->data[i].fname) == 0) // test pour le nom
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
            int indice=i;
            for(int j=indice;j < db->psize;j++)
            {
                db->data[j] = db->data[j+1];
            }
            db->psize = db->psize-1;

        }
    }
}

void db_delete_section(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (strcmp(s->section,db->data[i].section) == 0) // test pour le nom
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
            int indice=i;
            for(int j=indice;j < db->psize;j++)
            {
                db->data[j] = db->data[j+1];
            }
            db->psize = db->psize-1;

        }
    }
}

void db_delete_id(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++)
    {
        if (s->id == db->data[i].id)   // test pour le id
        {
            *new_count = *new_count +1;
            write_file(db,fd,i);
            int indice=i;
            for(int j=indice;j < db->psize;j++)
            {
                db->data[j] = db->data[j+1];
            }
            db->psize = db->psize-1;

        }
    }
}

void db_delete_birthdate(database_t *db,student_t *s,FILE *fd,int *new_count)
{
    for(int i=0;i<db->psize;i++) {
        if (s->birthdate.tm_mday == db->data[i].birthdate.tm_mday &&
            s->birthdate.tm_mon == db->data[i].birthdate.tm_mon &&
            s->birthdate.tm_year == db->data[i].birthdate.tm_year) {
            *new_count = *new_count + 1;
            write_file(db,fd,i);
            int indice = i;
            for (int j = indice; j < db->psize; j++) {
                db->data[j] = db->data[j + 1];
            }
            db->psize = db->psize - 1;
        }
    }
}

void write_file(database_t *db,FILE *fd,int count)
{
    fprintf(fd,"%u: %s %s in section %s, born on the %d/%d/%d\n", db->data[count].id, db->data[count].fname, db->data[count].lname,
            db->data[count].section, db->data[count].birthdate.tm_mday, db->data[count].birthdate.tm_mon+1, db->data[count].birthdate.tm_year+1900);

}



