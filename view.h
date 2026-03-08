#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include "types.h"

// Structure to Store MP3 Tag Information
typedef struct
{
    FILE *fptr_mp3;      // File pointer for MP3 file
    char *filename;      // MP3 file name

    /* Tag Data Buffers */
    char title[100];      // TIT2 - title
    char artist[100];     // TPE1 - artist
    char album[100];      // TALB - album
    char year[10];        // TYER - year
    char genre[50];       // TCON - genre
    char comment[100];    // COMM - comment / compusor

} ViewInfo;

/* Step 1: Validate command line arguments */
Status validate_view_args(char *argv[], ViewInfo *viewInfo);

/* Step 2: Open MP3 file in read mode */
Status open_mp3_file(ViewInfo *viewInfo);

/* Step 3: Read and validate ID3 header (first 10 bytes) */
Status read_and_validate_header(ViewInfo *viewInfo);

/* Step 4: Read frame header (Tag ID + Size + Flags) */
Status read_frame_header(ViewInfo *viewInfo, char *frame_id, int *size);

/* Step 5: Convert Big Endian to Little Endian */
Status convert_endian(int *size);

/* Step 6: Read frame data */
Status read_frame_data(ViewInfo *viewInfo, int size, char *buffer);

/* Step 7: Main View Tag Controller Function */
Status view_tag(ViewInfo *viewInfo);

/* step 8: Display extracted tag details */
Status display_tag(ViewInfo *viewInfo);

/* step 9: constroller function processing for view operation */
Status do_view_process(char *argv[]);

#endif
