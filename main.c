/*
Name : SATTI BHARAT SAI NAGA BABU
Batch id : 25036A + 39
Project name : MP3 tag reader and editor
Description : This project is a command-line based application developed in C to read and edit ID3v2 metadata tags of MP3 files.

    -> The program allows the user to:
            1. View MP3 tag details such as Title, Artist, Album, Year, Genre, and Comments.
            2. Edit selected tag fields and update the MP3 file safely.

    -> The application works by reading the ID3 header and frames in binary mode, modifying the required frame data.
        writing the updated content into a temporary file. 
        After successful modification, the original file is replaced with the updated file.
*/

#include<stdio.h>
#include<string.h>
#include "view.h"
#include "edit.h"
#include "types.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    // step 1: check if sufficient arugments are passed 
    if(argc < 2) {
        printf("Error : Insufficient Arguments\n");
        printf("Usage -h for Help\n");
        return failure;
    }
    // step 2: check the operation type
    OperationType op_type = check_operation_type(argv[1]);
    // step 3: help operation function
    if(op_type == help)
    {
        printf("--------------------------------------------------------\n");
        printf("                MP3 Tag Reader & Editor\n");
        printf("--------------------------------------------------------\n");
        printf("View Operation:\n");
        printf("./a.out -v sample.mp3\n");
        printf("--------------------------------------------------------\n");
        printf("Edit Operation:\n");
        printf("./a.out -e -t \"Title\"   sample.mp3    (TIT2 - Title)\n");
        printf("./a.out -e -a \"Artist\"  sample.mp3    (TPE1 - Artist)\n");
        printf("./a.out -e -A \"Album\"   sample.mp3    (TALB - Album)\n");
        printf("./a.out -e -y \"Year\"    sample.mp3    (TYER - Year)\n");
        printf("./a.out -e -g \"Genre\"   sample.mp3    (TCON - Genre)\n");
        printf("./a.out -e -c \"Comment\" sample.mp3    (COMM - Comment)\n");
        printf("----------------------------------------------------------\n");
        // return success
        return success;
    }
    // step 4: check if the operation type is view or not
    else if (op_type == view)
    {
        if (argc < 3)
        {
            printf("Error : Please provide MP3 file name\n");
            return failure;
        }
        // Call complete controller function view processing function
        if (do_view_process(argv) == success)
        {
            printf("View Operation Successful\n");
        }
        else
        {
            printf("View Operation Failed\n");
        }
    } 
    // step 4: check if the operation is edit or not
    else if(op_type == edit) {
        /*For edit :  
            we need: ./a.out -e <field for tag> <new_value data> sample.mp3 
            So argc should be at least 5 
        */
        if(argc < 5) {
            printf("Error : Insufficient Arguments for edit operation\n");
            printf("Usage -h for help to correct format\n");
            return 1;
        }
        // Declare structure for edit details
        EditInfo editInfo;
        // call edit function
        if(do_edit_process(argv) == success) 
        {
            printf("Edit Operation Successfull\n");
        } else {
            printf("Edit Operation Failed\n");
        }
    }
    // step 5: if operation is not supported
    else {
        printf("Error : Unsupported Operation\n");
        printf("Usage -h for help\n");
        return failure;
    }
    // step 6 : return success
    return success;
}
// function to check the operation type based on the command line arugment
OperationType check_operation_type(char *symbol) 
{
    // compare the symbol with -h for help operation
    if(strcmp(symbol, "-h") == 0) {
        return help;
    }
    // compare the symbol with -v for view operation
    else if(strcmp(symbol, "-v") == 0) {
        return view;
    } 
    // compare the symbol with -e for view operation
    else if(strcmp(symbol, "-e") == 0) {
        return edit;
    }
    // if symbol does not match for -v or -e we have to return unsupported
    else {
        return unsupported;
    }
}
