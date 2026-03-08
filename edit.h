#ifndef EDIT_H
#define EDIT_H

#include<stdio.h>
#include "types.h"

// structure for editing mp3 tags
typedef struct 
{
    char *filename;  // for name of the mp3 to be edited
    char *tag;     // for tag option to user provided
    char *new_value; // new value data for tag to edit

    FILE *fptr_src;  // file pointer for original file at read mode
    FILE *fptr_dest; // file pointer for temporary file at write mode

} EditInfo;

/* validates command line arugments */
Status validate_edit_args(char *argv[], EditInfo *editInfo);

/* opening both files read and write mode */
Status open_files(EditInfo *editInfo);

/* copies first 10 bytes of ID3 header */
Status copy_header(FILE *src, FILE *dest);

/* Reading each frame from your source file because it match mean we have to replace */
Status edit_tag(EditInfo *editInfo);

/* for copying reamining data after editing */
Status copy_reamining_data(FILE *src, FILE *dest);

/* main your controller function for edit operation */
Status do_edit_process(char *argv[]);

#endif