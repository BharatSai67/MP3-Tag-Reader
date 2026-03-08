#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "view.h"

// Step 1: Validate command line arguments Checks whether MP3 file name is passed properly
Status validate_view_args(char *argv[], ViewInfo *viewInfo)
{
    // Check if filename argument exists
    if (argv[2] == NULL) {
        return failure;
    }
    // Check whether file extension is .mp3
    if (strstr(argv[2], ".mp3") == NULL) {
        return failure;
    }
    // Store filename inside structure
    viewInfo->filename = argv[2];
    return success;
}
// Step 2: Open MP3 file in binary read mode
Status open_mp3_file(ViewInfo *viewInfo)
{
    // Open file in read-binary mode
    viewInfo->fptr_mp3 = fopen(viewInfo->filename, "rb");
    // If file not opened 
    if (viewInfo->fptr_mp3 == NULL) {
        return failure;
    }
    return success;
}
// Step 3: Read and validate ID3 header (First 10 bytes) ID3v2 header format for "ID3" and next 7 bytes are (version, flags, size)
Status read_and_validate_header(ViewInfo *viewInfo)
{
    char header[4];
    // Read first 3 bytes
    fread(header, 3, 1, viewInfo->fptr_mp3);
    // Add null terminator
    header[3] = '\0';
    // Check whether header contains "ID3"
    if (strncmp(header, "ID3", 3) != 0) {
        return failure;
    }
    // Skip next 7 bytes (version + flags + size)
    fseek(viewInfo->fptr_mp3, 7, SEEK_CUR);
    // return success after successfully
    return success;
}
/* Step 4: Read Frame Header for Frame Header Structure (10 bytes total)
        4 bytes : Frame ID (TIT2, TPE1 & reamining 4 tags) 
        4 bytes : Frame size 2 bytes : Flags
*/
Status read_frame_header(ViewInfo *viewInfo, char *frame_id, int *size)
{
    // Read 4 bytes frame ID
    if (fread(frame_id, 1, 4, viewInfo->fptr_mp3) != 4) {
        return failure;
    }
    frame_id[4] = '\0';   // Null terminate frame ID
    // If empty frame ID -> stop reading
    if (frame_id[0] == '\0') {
        return failure;
    }
    if(fread(size, 4, 1, viewInfo->fptr_mp3) != 1) 
    {
        return failure;
    }
    // calling endainess function to converting bit to little endian
    convert_endian(size);
    // Skip 2 bytes flags
    fseek(viewInfo->fptr_mp3, 2, SEEK_CUR);
    // return success
    return success;
}
// Step 5: Convert Big Endian to Little Endian)
Status convert_endian(int *size)
{
    *size = ((*size & 0x000000FF) << 24) | ((*size & 0x0000FF00) << 8)  | ((*size & 0x00FF0000) >> 8)  | ((*size & 0xFF000000) >> 24);
    // return success after convertion
    return success;
}
// Step 6: Read Frame Data and Reads actual tag content (Title, Artist etc)
Status read_frame_data(ViewInfo *viewInfo, int size, char *buffer)
{
    // Check for invalid size
    if (size <= 0 || size >= 1000) {
        return failure;
    }
    // Read frame data
    if (fread(buffer, 1, size, viewInfo->fptr_mp3) != size) {
        return failure;
    }
    // Add null terminator
    buffer[size] = '\0';
    // return success
    return success;
}
// Step 7: Extract Required Tags from Frames and loops through frames and stores required fields
Status view_tag(ViewInfo *viewInfo)
{
    char frame_id[5];
    int size;
    char buffer[1000];
    // Initialize all tag fields as empty
    strcpy(viewInfo->title, "");
    strcpy(viewInfo->artist, "");
    strcpy(viewInfo->album, "");
    strcpy(viewInfo->year, "");
    strcpy(viewInfo->genre, "");
    strcpy(viewInfo->comment, "");
    // Loop until frames end
    while (1)
    {
        // Read frame header
        if (read_frame_header(viewInfo, frame_id, &size) == failure) {
            break;
        }
        // Read frame data
        if (read_frame_data(viewInfo, size, buffer) == failure) {
            break;
        }
        // First byte is encoding (skip it)
        char *data = buffer + 1;
        // Compare frame ID and store corresponding data
        if (strcmp(frame_id, "TIT2") == 0)
        {
            strcpy(viewInfo->title, data);
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            strcpy(viewInfo->artist, data);
        }
        else if (strcmp(frame_id, "TALB") == 0) 
        {
            strcpy(viewInfo->album, data);
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            strcpy(viewInfo->year, data);
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            strcpy(viewInfo->genre, data);
        }
        else if( strcmp(frame_id, "COMM") == 0) 
        {
            /* byte -> [0] encoding like skipping,
               byte -> [1] - [3] for language -> i want to print only for language
               byte -> [4] onwards starts comment
            */
            strcpy(viewInfo->comment, data);
        }
    }
    // return success after reading tags
    return success;
}
// Step 8: Display Extracted Tag Details
Status display_tag(ViewInfo *viewInfo)
{
    printf("------------------------------------------------------\n");
    printf("                    MP3 TAG DETAILS                   \n");
    printf("------------------------------------------------------\n");
    printf("Title    : %s\n", viewInfo->title);
    printf("Artist   : %s\n", viewInfo->artist);
    printf("Album    : %s\n", viewInfo->album);
    printf("Year     : %s\n", viewInfo->year);
    printf("Genre    : %s\n", viewInfo->genre);
    printf("Comment  : %s\n", viewInfo->comment);
    printf("-------------------------------------------------------\n");

    return success;
}
// Step 9: Main Controller Function and This function is called from main()
Status do_view_process(char *argv[])
{
    ViewInfo viewInfo;
    // Validate arguments
    if (validate_view_args(argv, &viewInfo) == failure) {
        printf("Error : Invalid Arugments\n");
        return failure;
    }
    // Open file
    if (open_mp3_file(&viewInfo) == failure) {
        printf("Error for file opening\n");
        return failure;
    }
    // Validate ID3 header
    if (read_and_validate_header(&viewInfo) == failure) {
        printf("ERROR: File is not a valid MP3 with ID3 tag.\n");
        fclose(viewInfo.fptr_mp3);
        return failure;
    }
    // Extract tags
    if (view_tag(&viewInfo) == failure) {
        printf("ERROR: Failed to read MP3 tag data.\n");
        return failure;
    }
    // Display tags
    display_tag(&viewInfo);
    //printf("MP3 tag view process completed successfully.\n");
    // closing file pointer after reading
    fclose(viewInfo.fptr_mp3);
    // return success
    return success;
}
