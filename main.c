#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <sqlite3.h>

#include "md5.h"


int arr_size = 0;
int old_c = 0;
int new_c = 0;
int *old_ids = NULL;
int *new_ids = NULL;

void exec_query(sqlite3 *db, char* sql, int (*callback)(void* data, int argc, char** argv, char** colname)) {
    char* err_msg;
    if (sqlite3_exec(db, sql, callback, 0, &err_msg) != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    } 
}

int get_old_db_ids(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    old_ids[old_c++] = atoi(argv[0]);
    return 0;
}

int get_new_db_ids(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    new_ids[new_c++] = atoi(argv[0]);
    return 0;
}

int get_arr_size(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;

    if (argv[0] != NULL && atoi(argv[0]) > 0) {
        arr_size = atoi(argv[0]);
        if ((old_ids = malloc(sizeof(int) * atoi(argv[0]))) == NULL) {
            fprintf(stderr, "Array for old ids was not allocated");
            exit(EXIT_FAILURE);
        }
        if ((new_ids = malloc(sizeof(int) * atoi(argv[0]))) == NULL) {
            fprintf(stderr, "Array for new ids was not allocated");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Array size is less than one");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int main (int argc, char **argv) {
    if (argc < 4) {
        printf("Program usage: program old_db_path new_db_path path_to_images");
        return EXIT_FAILURE;
    }

    sqlite3 *db;

    if (sqlite3_open(argv[1], &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database %s: %s\n", argv[2], sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }
    exec_query(db, "SELECT COUNT(*) FROM mangas WHERE favorite = 1", get_arr_size);
    exec_query(db, "SELECT _id FROM mangas WHERE favorite = 1 ORDER BY url", get_old_db_ids);
    sqlite3_close(db);

    if (sqlite3_open(argv[2], &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database %s: %s\n", argv[2], sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }
    exec_query(db, "SELECT _id FROM mangas WHERE favorite = 1 ORDER BY url", get_new_db_ids);
    sqlite3_close(db);

    char* old_id = (char*)malloc(33 * sizeof(char));
    char* new_id = (char*)malloc(33 * sizeof(char));
    char* old_dest = (char*)malloc(255 * sizeof(char));
    char* new_dest = (char*)malloc(255 * sizeof(char));
    if (new_id == NULL || old_id == NULL || old_dest == NULL || new_dest == NULL) {
        fprintf(stderr, "Cannot allocate a string");
        return EXIT_FAILURE;
    }
    old_id[0] = '\0';
    new_id[0] = '\0';
    old_dest[0] = '\0';
    new_dest[0] = '\0';
    
    FILE *file;

    for (int i = 0; i < arr_size; i++) {
        strcat(old_dest, argv[3]);
        if (old_dest[strlen(old_dest)] != '/') {
            strcat(old_dest, "/");
        }
        strcpy(new_dest, old_dest);

        sprintf(old_id, "%d", old_ids[i]);
        old_id = get_md5(old_id);
        strcat(old_dest, old_id);

        if((file = fopen(old_dest, "r"))) {
            fclose(file);

            sprintf(new_id, "%d", new_ids[i]);
            new_id = get_md5(new_id);
            strcat(new_dest, new_id);

            if (rename(old_dest, new_dest) == 0) {
                printf("%s -> %s\n", old_id, new_id);
            } else {
                printf("%s XX %s\n", old_id, new_id);
            }
        }
        old_dest[0] = '\0';
        new_dest[0] = '\0';
    }

    return EXIT_SUCCESS; 
}