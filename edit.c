#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "edit.h"

// converting from little endian to big endian
static void convert_endian(int *size) 
{
    *size = ((*size & 0x000000FF) << 24) | ((*size & 0x0000FF00) << 8)  | ((*size & 0x00FF0000) >> 8)  | ((*size & 0xFF000000) >> 24);
}
// validate command line arguments
Status validate_edit_args(char *argv[], EditInfo * editInfo) 
{
    // check if required arugments present or not
    if(argv[2] == NULL || argv[3] == NULL || argv[4] == NULL) 
    {
        return failure;
    }
    // check whether file extension is (.mp3) for not
    if(strstr(argv[4], ".mp3") == NULL) 
    {
        return failure;
    }
    // store arguments into structure
    editInfo->tag = argv[2];
    editInfo->new_value = argv[3];
    editInfo->filename = argv[4];
    // return success
    return success;
}
// opening files for 1 at read mode and another for write mode
Status open_files(EditInfo *editInfo)
{
    // opening your original mp3 file
    editInfo->fptr_src = fopen(editInfo->filename, "rb");
    if(editInfo->fptr_src == NULL)
    {
        return failure;
    }
    // creating temporary file
    editInfo->fptr_dest = fopen("temp.mp3", "wb");
    if(editInfo->fptr_dest == NULL) 
    {
        return failure;
    }
    return success;
}
// copy header first 10 bytes your src to dest file
Status copy_header(FILE *src, FILE *dest)
{
    char header[10];
    fread(header, 1, 10, src);
    fwrite(header, 1, 10, dest);

    return success;
}
/* edit tag for read each frame from src file
    if matches user tag replaced old data with new value
    otherwise copies orginal frame to your dest file
*/
Status edit_tag(EditInfo *editInfo) 
{
    char frame_id[5];  // to store frame id like your 6 tags
    int size;  // for your frame size
    char buffer[1000];  // to store frame data
    while(fread(frame_id, 1, 4, editInfo->fptr_src) == 4)  
    {
        frame_id[4] = '\0'; // null terminate for string comprison

        if(frame_id[0] == 0) 
        {
            break;
        }
        // read 4 bytes frame size
        fread(&size, 4, 1, editInfo->fptr_src);
        convert_endian(&size);
        // check for size 
        if(size <= 0 || size > 1000)
        {
            break;
        }
        // skip 2 bytes for flag
        fseek(editInfo->fptr_src, 2, SEEK_CUR);
        // read frame data
        fread(buffer, size, 1, editInfo->fptr_src);
        // flag to check whether this frame should be edited for not using extra varaible
        int match = 0;
        // compare tag option with frame id
        if(strcmp(editInfo->tag, "-t") == 0 && strcmp(frame_id, "TIT2") == 0) 
        {
            match = 1;
        } 
        else if(strcmp(editInfo->tag, "-a") == 0 && strcmp(frame_id, "TPE1") == 0)
        {
            match = 1;
        }
        else if(strcmp(editInfo->tag, "-A") == 0 && strcmp(frame_id, "TALB") == 0) 
        {
            match = 1;
        }
        else if(strcmp(editInfo->tag, "-y") == 0 && strcmp(frame_id, "TYER") == 0) 
        {
            match = 1;
        } 
        else if(strcmp(editInfo->tag, "-g") == 0 && strcmp(frame_id, "TCON") == 0) 
        {
            match = 1;
        }
        else if(strcmp(editInfo->tag, "-c") == 0 && strcmp(frame_id, "COMM") == 0) 
        {
            match = 1;
        }
        // write frame id to destination file
        fwrite(frame_id, 1, 4, editInfo->fptr_dest);
        // if your tag matches
        if(match)
        {
            // write new frame size
            int new_size = strlen(editInfo->new_value) + 1; // 1 byte for encoding
            // creating one temporary variable size 
            int temp_size = new_size;
            // calling convert endian function to change little to big endian
            convert_endian(&temp_size);
            // write new size
            fwrite(&temp_size, 4, 1, editInfo->fptr_dest);
            // write 2 bytes for flag
            char flags[2] = {0};
            fwrite(flags, 1, 2, editInfo->fptr_dest);
            // write encoding byte to skip first byte 
            char encoding = 0;
            fwrite(&encoding, 1, 1, editInfo->fptr_dest);
            // write new tag value
            fwrite(editInfo->new_value, 1, strlen(editInfo->new_value), editInfo->fptr_dest);

            printf("Tag Updated SuccessFully\n");
        }
        // if not matches your tag
        else {
            // copy your original frame as it is
            int temp_size = size;
            // calling endianess function
            convert_endian(&temp_size);
            // write original size
            fwrite(&temp_size, 4, 1, editInfo->fptr_dest);
            // write flags
            char flags[2] = {0};
            fwrite(flags, 1, 2, editInfo->fptr_dest);
            // write original data
            fwrite(buffer, 1, size, editInfo->fptr_dest);
        }
    }
    return success;
}
// copying remaining data after finishing id3 frames, src to dest file
Status copy_reamining_data(FILE *src, FILE *dest)
{
    char ch;
    while(fread(&ch, 1, 1, src)) {
        fwrite(&ch, 1, 1, dest);
    }
    return success;
}
// do edit process for your main controller function
Status do_edit_process(char *argv[])
{
    EditInfo editInfo;

    if(validate_edit_args(argv, &editInfo) == failure)
    {
        printf("Invalid Arguments\n");
        return failure;
    }
    if(open_files(&editInfo) == failure) 
    {
        printf("File open Error.\n");
        return failure;
    }
    // copy ID3 header
    copy_header(editInfo.fptr_src, editInfo.fptr_dest);
    
    // edit required frame
    edit_tag(&editInfo);

    // copy rest of mp3 audio file data
    copy_reamining_data(editInfo.fptr_src, editInfo.fptr_dest);

    fclose(editInfo.fptr_src);
    fclose(editInfo.fptr_dest);

    // replace original file with your temporary file
    remove(editInfo.filename);
    rename("temp.mp3", editInfo.filename);
    printf("Edit Process Completed Successfully\n");
    return success;
}