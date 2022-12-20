// Oudahya Ismaïl
// 000479390
// projet os
#define __USE_XOPEN
#define _GNU_SOURCE
#include "db.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "parsing.h"
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
static volatile bool STOP = false; // var global static volatile pour pouvoir gérer l'arrêt du while fgets dans le cas du ctrl-c

struct Argument{ // cette structure permet d'initialiser les variables à utiliser dans les threads donc ce sont leur paramètres
    database_t db;
    student_t* s;
    int flags;
    struct timespec start;
    struct timespec end;
    FILE* fd;
    char* command;
    char const** arg;
    char* texte_buffer;
    char* print_log;
};
void *test_select(void *prog);
void *test_delete(void *prog);
void *test_update(void *prog);
void test_insert(database_t *db,student_t *s,int flags,FILE *fd,char *command,char const*argv[],char *texte_buffer,char *print_log);


void IMPRIMER()
{
    printf("Welcome to the Tiny Database!\nLoading the database...\nDone!\nPlease enter your requests.\n");
}

void sigint_handler(int sig)
{
    STOP=true;
}

int main(int argc, char const *argv[]) {
    signal(SIGINT,sigint_handler); // permet d'envoyer le signal du Ctrl-c à la classe sigint_handler

    pthread_t threads[3];
    if (argc<=1) // si l'utilisateur ne rentre pas de base de données , alors on crée une nouvelle base de données avec le nom database.bin
    {
        FILE *file_database;
        file_database = fopen("database.bin", "wb");
        fclose(file_database);
        char *nom_fichier="database.bin";
        argv[1] = nom_fichier;
    }
    database_t db;
    student_t *s;
    s = (student_t *) malloc(sizeof(student_t));
    if (!s) // vérification si le malloc à bien été réaliser sinon on arrête totalement le programme.
    {
        perror("Erreur de malloc!\n");
        exit(1);
    }
    IMPRIMER();
    db_load(&db, argv[1]);

    int flags=0;
    char buffer[512];
    while (STOP==false && fgets(buffer, 512, stdin)) { // Le while fgets permet de recuperer un stdin donc une entrée
        // écrit par l'utilisateur ou bien en entrant un texte.txt grâce à " < "
        // si ctrl-c à été entrer en tant que commande , alors le while s'arrête tout en sauvegardant la database
        char *command;
        size_t destination_size = strlen(buffer)*sizeof(char);
        command=malloc(sizeof(buffer)); // je fais un genre de copie pour eviter d'utiliser strcpy.
        if (!command)
        {
            perror("Erreur de malloc!\n");
            exit(1);
        }
        strncpy(command,buffer,destination_size);
        char *commande;
        struct timeval usec;
        gettimeofday(&usec, NULL);
        long long int times_usec =((long long int) usec.tv_sec) * 1000000ll + (long long int) usec.tv_usec;
        FILE *fd = stdout;
        char buffer_str_log[512];
        commande = strtok_r(command, " ", &command);
        if (STOP == false)
        {
            sprintf(buffer_str_log, "logs/%lld-%s.txt", times_usec, commande); // si le ctrl-c n'a pas été déclancher alors il met dans le buffer le nom du fichier à crée.
            fd = fopen(buffer_str_log, "a+");
            for (int i = 0; i < 512; i++) { // cette boucle permet de vérifier si un passage à la ligne n'est pas dans les différents buffer car celà rompt les comparaisons de string
                if (buffer[i] == '\n')
                    buffer[i] = '\0';
                if (command[i] == '\n')
                    command[i] = '\0';
            }
            printf("Running query \"%s\"\n", buffer);
        }
        struct Argument argument; // initialise des differents paramètres de la struct.
        argument.db=db;
        argument.s=s;
        argument.flags=flags;
        argument.command=command;
        argument.fd = fd;
        argument.arg = argv;
        argument.texte_buffer=buffer;
        argument.print_log = buffer_str_log;
        int ret_val;
        if (strcmp(commande, "select") == 0) { // si on reçoit comme commande select
            ret_val=pthread_create(&threads[0],NULL,test_select,&argument); // on crée un thread avec les paramètres &argument
            if (ret_val != 0) // errno
            {
                fprintf(stderr,"Création du thread: %s\n",strerror(ret_val));
                return EXIT_FAILURE;
            }
            pthread_join(threads[0],NULL); // on attends qu'il finit
        }
        else if (strcmp(commande, "delete") == 0) {
            ret_val=pthread_create(&threads[1],NULL,test_delete,&argument);
            if (ret_val != 0) // errno
            {
                fprintf(stderr,"Création du thread: %s\n",strerror(ret_val));
                return EXIT_FAILURE;
            }
            pthread_join(threads[1],NULL);
        }
        else if (strcmp(commande, "update") == 0) {
            ret_val=pthread_create(&threads[2],NULL,test_update,&argument);
            if (ret_val != 0) // errno
            {
                fprintf(stderr,"Création du thread: %s\n",strerror(ret_val));
                return EXIT_FAILURE;
            }
            pthread_join(threads[2],NULL);
        }
        else if (strcmp(commande, "insert") == 0) {
            test_insert(&db, s, flags,fd, command, argv, buffer, buffer_str_log);
        }

    }
    if (STOP == true)
    {
        printf("Waiting for requests to terminate...\nComitting database changes to the disk...\n");
        db_save(&db,argv[1]);
        printf("Done.\n");
    }
    free(s); // free le malloc de student
    free(db.data);// free le malloc de database
    return EXIT_SUCCESS;
}

void *test_select(void *prog)
{
/**
 * Cette fonction permet de lancer le parsing pour définir la recherche c'est à dire le nom,prénom,id... à rechercher dans la database
 * ici on ouvre aussi un fichier gerer_database.txt qui permet de stocker tout les étudiants trouver durant la recherche.
 * On les écriras dans le fichier de base qui regroupe toute les informations.
 * Les différents appel de fonction dépendra des informations reçu du parsing donc que ce soit un fname,lname...
 * On calculera aussi grâce à clock_gettime le temps utilisé par le processeur pour récuperer les informations.
 * Ensuite on supprimera le fichier qui a permis de gerer la recherche des étudiants.
 */

    struct Argument *argument = (struct Argument *)prog;
    FILE *parcours_fd;
    char field_to_update[64];
    char update_value[64];
    int new_count = 0;
    parcours_fd=fopen("gerer_database.txt","a");
    int get_students;
    argument->flags=parse_selectors(argument->command,field_to_update,update_value);
    if (argument->flags == 1) {
        clock_gettime(CLOCK_MONOTONIC, &argument->start);

        if (strcmp(field_to_update, "fname") == 0) {
            strcpy(argument->s->fname, update_value);
            db_select_fname(&argument->db, argument->s, parcours_fd, &new_count);
        }
        if (strcmp(field_to_update,"lname")==0){
            strcpy(argument->s->lname,update_value);
            db_select_lname(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"section")==0){
            strcpy(argument->s->section,update_value);
            db_select_section(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"id") ==0){
            argument->s->id = (unsigned)atol(update_value);
            db_select_id(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"birthdate") ==0){
            strptime(update_value, "%m/%d/%Y", &argument->s->birthdate);
            db_select_birthday(&argument->db,argument->s,parcours_fd,&new_count);
        }
        clock_gettime(CLOCK_MONOTONIC, &argument->end);
        fclose(parcours_fd);
        u_int64_t tm_secondes = (argument->end.tv_sec - argument->start.tv_sec )*1000;
        float nano_secondes = (argument->end.tv_nsec - argument->start.tv_nsec)/1000000.0;
        double cpu_time_used_ms = (double) (tm_secondes+nano_secondes);
        fprintf(argument->fd,"Query \"%s\"completed in %lfms with %d results\n",argument->texte_buffer,cpu_time_used_ms,new_count);
        parcours_fd = fopen("gerer_database.txt","r");
        while ((get_students=fgetc(parcours_fd)) != EOF) // permet d'écrire les étudiants dans le fichier.
        {
            fprintf(argument->fd,"%c",get_students);
        }
        fclose(argument->fd); // ferme le fichier
        fclose(parcours_fd);
        remove("gerer_database.txt"); // supprime le fichier
        printf("%s\n",argument->print_log);
    }

    pthread_exit(NULL);

}

void *test_delete(void *prog)
{
/**
 * ici les paramètres sont similaire au parsing du select sauf que les appels de fonctions sont totalement différentes
 * car ici on appel les fonctions de delete.
 */
    struct Argument *argument = (struct Argument *)prog;
    FILE *parcours_fd;
    parcours_fd= fopen("gerer_database.txt","a");
    if (parcours_fd == NULL) // gestion d'erreur d'ouverture du fichier
    {
        perror("Erreur de gestion du fichier!\n");
        exit(1);
    }
    int new_count = 0;
    int get_students;
    char field_to_update[64];
    char update_value[64];
    argument->flags=parse_selectors(argument->command,field_to_update,update_value);
    if (argument->flags == 1)
    {
        clock_gettime(CLOCK_MONOTONIC, &argument->start);
        if (strcmp(field_to_update,"fname") ==0){
            strcpy(argument->s->fname, update_value);
            db_delete_fname(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"lname")==0){
            strcpy(argument->s->lname,update_value);
            db_delete_lname(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"section")==0){
            strcpy(argument->s->section,update_value);
            db_delete_section(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"id")==0){
            argument->s->id = (unsigned)atol(update_value);
            db_delete_id(&argument->db,argument->s,parcours_fd,&new_count);
        }
        if (strcmp(field_to_update,"birthdate")==0){
            strptime(update_value, "%m/%d/%Y", &argument->s->birthdate);
            db_delete_birthdate(&argument->db,argument->s,parcours_fd,&new_count);
        }
        clock_gettime(CLOCK_MONOTONIC, &argument->end);
        fclose(parcours_fd);
        u_int64_t tm_secondes = (argument->end.tv_sec - argument->start.tv_sec )*1000;
        float nano_secondes = (argument->end.tv_nsec - argument->start.tv_nsec)/1000000.0;
        double cpu_time_used_ms = (double) (tm_secondes+nano_secondes);
        fprintf(argument->fd,"Query \"%s\"completed in %fms with %d results.\n",argument->texte_buffer,cpu_time_used_ms,new_count);
        parcours_fd = fopen("gerer_database.txt","r");
        if (parcours_fd == NULL)
        {
            perror("Erreur de gestion du fichier!\n");
            exit(1);
        }
        while ((get_students=fgetc(parcours_fd)) != EOF)
        {
            fprintf(argument->fd,"%c",get_students);
        }
        fclose(argument->fd);
        fclose(parcours_fd);
        remove("gerer_database.txt");
        db_save(&argument->db,argument->arg[1]);
        printf("%s\n",argument->print_log);
    }
    pthread_exit(NULL); // return du thread

}

void *test_update(void *prog)
{
    /**
 * ici les paramètres sont différentes du parsing pour le select ou delete.On prendra beaucoup plus de paramètres pour
     * pouvoir update le/les étudiant(s) de la database.
 */
    struct Argument *argument = (struct Argument *)prog;
    FILE *parcours_fd;
    parcours_fd= fopen("gerer_database.txt","w");
    if (parcours_fd == NULL)
    {
        perror("Erreur de gestion du fichier!\n");
        exit(1);
    }
    int new_count = 0;
    int get_students;
    char field_filter[64];
    char value_filter[64];
    char field_to_update[64];
    char update_value[64];
    argument->flags=parse_update(argument->command,field_filter,value_filter,field_to_update,update_value);
    for(int j=0;j<64;j++) // remplace le retour à la ligne pour ne pas avoir de soucis de condition
    {
        if (update_value[j] == '\n')
            update_value[j] = '\0';
    }
    if (argument->flags == 1) {
        clock_gettime(CLOCK_MONOTONIC, &argument->start);
        if (strcmp(field_filter,"birthdate") ==0 || strcmp(field_to_update,"birthdate") ==0){
            strptime(update_value, "%m/%d/%Y", &argument->s->birthdate);
            db_update_birthdate(&argument->db,argument->s,value_filter,field_to_update,update_value,parcours_fd,&new_count);}
        if (strcmp(field_filter, "id") == 0)
            db_update_id(&argument->db, argument->s, value_filter, field_to_update, update_value, parcours_fd, &new_count);
        if (strcmp(field_filter,"fname") ==0)
            db_update_fname(&argument->db,argument->s,value_filter,field_to_update,update_value,parcours_fd,&new_count);
        if (strcmp(field_filter,"lname") ==0)
            db_update_lname(&argument->db,argument->s,value_filter,field_to_update,update_value,parcours_fd,&new_count);
        if (strcmp(field_filter,"section") ==0)
            db_update_section(&argument->db,argument->s,value_filter,field_to_update,update_value,parcours_fd,&new_count);
        clock_gettime(CLOCK_MONOTONIC, &argument->end);
        fclose(parcours_fd);
        u_int64_t tm_secondes = (argument->end.tv_sec - argument->start.tv_sec )*1000;
        float nano_secondes = (argument->end.tv_nsec - argument->start.tv_nsec)/1000000.0;
        double cpu_time_used_ms = (double) (tm_secondes+nano_secondes);
        fprintf(argument->fd,"Query \"%s\"completed in %fms with %d results.\n",argument->texte_buffer,cpu_time_used_ms,new_count);
        parcours_fd = fopen("gerer_database.txt","r");
        if (parcours_fd == NULL)
        {
            perror("Erreur de gestion du fichier!\n");
            exit(1);
        }
        while ((get_students=fgetc(parcours_fd)) != EOF)
        {
            fprintf(argument->fd,"%c",get_students);
        }
        fclose(argument->fd);
        fclose(parcours_fd);
        remove("gerer_database.txt");
        db_save(&argument->db,argument->arg[1]);
        printf("%s\n",argument->print_log);
    }
    pthread_exit(NULL); // return du thread

}

void test_insert(database_t *db,student_t *s,int flags,FILE *fd,char *command,char const*argv[],char *texte_buffer,char *print_log) {
/**
 * ici le test_insert ne prends pas un compte l'utilisation d'un thread car tout ce qu'il fait c'est d'inserer à la fin de la database
 * le parsing ici donnera ses paramètres aux students déclaré en paramètres et suit un appel à la fonction qui l'ajoutera
 */
    struct timespec start;
    struct timespec end;
    FILE *parcours_fd;
    parcours_fd= fopen("gerer_database.txt","w");
    if (parcours_fd == NULL)
    {
        perror("Erreur de gestion du fichier!\n");
        exit(1);
    }
    int get_students;
    flags = parse_insert(command, s->fname, s->lname, &s->id, s->section, &s->birthdate);
    int new_count = 0;
    if (flags == 1) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        db_add(db, s,parcours_fd,&new_count);
        clock_gettime(CLOCK_MONOTONIC, &end);
        fclose(parcours_fd);
        u_int64_t tm_secondes = (end.tv_sec - start.tv_sec )*1000;
        float nano_secondes = (end.tv_nsec - start.tv_nsec)/1000000.0;
        double cpu_time_used_ms = (double) (tm_secondes+nano_secondes);
        fprintf(fd, "Query \"%s\"completed in %lfms with %d results.\n", texte_buffer, cpu_time_used_ms, new_count);
        parcours_fd = fopen("gerer_database.txt","r");
        if (parcours_fd == NULL)
        {
            perror("Erreur de gestion du fichier!\n");
            exit(1);
        }
        while ((get_students=fgetc(parcours_fd)) != EOF)
        {
            fprintf(fd,"%c",get_students);
        }
        fclose(fd);
        fclose(parcours_fd);
        remove("gerer_database.txt");
        db_save(db,argv[1]); // à la fin on sauvegarde la database avec argv qui est la database
        printf("%s\n",print_log);
    }
}
