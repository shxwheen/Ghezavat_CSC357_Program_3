#define _XOPEN_SOURCE 500 // need this or I get implicit declaration for strdup() . activates functions that may not be in standard C library
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
    char *name;
    size_t size;
    int isDir;
} dirElement;

// comparing function used to compare dirElements 
// for the qsort function
int compareDirElements(const void *x, const void *y){
    // dirElement structs to be compared
    dirElement *element1 = (dirElement*)x;
    dirElement *element2 = (dirElement*)y;
    // compare names, store result in z
    int z = strcmp(element1->name, element2->name);
    // return comparison result
    return z;
}

void printDir(char *path, int indent, int hiddenFlag, int sizeFlag){

    // open directory
    // check if opendir successful
    DIR *dir = opendir(path);
    if(dir == NULL){
        perror("failed to open dir");
        return;
    }

    // pointer to a built-in C directory struct 
    struct dirent *entry; 
    //pointer to array that will hold the dirElements
    dirElement *dirEntriesArrayPtr = NULL;
    // count of dirElements inside of the array
    int arrCount = 0;

    // while pointer to readdir() is NOT null
    while((entry = readdir(dir)) != NULL){
        // if statement to continue/skip "." or "..", indicating current and parent direcotry 
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }

        // skip hidden file names (start with ".") unless they're requested by "-a" switch and hiddenFlag = = 1
        if(hiddenFlag == 0 && entry->d_name[0] == '.'){
            continue;
        }

        // create new directory element and populate name field for now
        dirElement newDirElement;
        newDirElement.name = strdup(entry->d_name);
        // error handling for strdup
        if(newDirElement.name == NULL){
            perror("strdup() failed");
            free(dirEntriesArrayPtr);
            closedir(dir);
            return;
        }


        // total path length (null terminator + "/" included)
        int totalPathLength = strlen(entry->d_name)+strlen(path) + 2;
        // malloc for total path
        char *totalPath = malloc(totalPathLength);
        // memory reallocation memory handling
        if(totalPath == NULL){
            perror("failed to allocate memory for totalPath");
            free(dirEntriesArrayPtr);
            closedir(dir);
            return;
        }

        // write and save to totalPath
        sprintf(totalPath,"%s/%s", path, entry->d_name);

        // struct stat to store fileInfo
        struct stat fileInfo;
        // if stat is successful use, use fileInfo to get needed information
        if(stat(totalPath, &fileInfo) == 0){
            // set directory flag
            newDirElement.isDir = S_ISDIR(fileInfo.st_mode);
            // set dirElement size
            newDirElement.size = fileInfo.st_size;
        } else{
            // dirElement path invalid
            newDirElement.isDir = 0;
            newDirElement.size = -1;
        }
        free(totalPath);

        // reallocate memory for dirElement being appended to array
        dirElement *tempPtr = realloc(dirEntriesArrayPtr, sizeof(dirElement) * (arrCount+1));

        // memory reallocation error handling
        if(tempPtr == NULL){
            perror("failed to reallocate memory");
            free(dirEntriesArrayPtr);
            closedir(dir);
            return;
        } else{
            dirEntriesArrayPtr = tempPtr;
        }
        // append the new directory to array
        dirEntriesArrayPtr[arrCount] = newDirElement;
        // increment count for next item in array
        arrCount++;
    }


    // qort function on array
    qsort(dirEntriesArrayPtr, arrCount, sizeof(dirElement), compareDirElements);

    // loop for number of elements in array now that it is sorted
    for(int i = 0; i<arrCount; i++){
        // loop to create appropiate indentation for subdirectories
        for(int j = 0; j < indent; j++){
            printf("|   ");
        }
        // print entry names from current directory
        printf("|-- %s",dirEntriesArrayPtr[i].name);
        // print size only if dirElement is not a directory and size flag is high
        if(dirEntriesArrayPtr[i].isDir == 0 && sizeFlag){
            printf(" |size: %ld|", dirEntriesArrayPtr[i].size);
        }
        // new line for each dirElement
        printf("\n");

        // recursive call for subdirectories
        // checks if dirElement is a directory and makes sure it is not the current directory or parent directory
        if(strcmp(dirEntriesArrayPtr[i].name,".") != 0 && strcmp(dirEntriesArrayPtr[i].name,"..") != 0 && dirEntriesArrayPtr[i].isDir){
            // length for the path to the subdirectory
            int updatedPathLength = (strlen(path) + strlen(dirEntriesArrayPtr[i].name) + 2);
            // allocate memory to store path to subdirecory, combining current path with subdirecory's name and a slash
            char *updatedPath = malloc(updatedPathLength);
            // memory allocation error handling
            if(updatedPath == NULL){
                perror("memory allocation failed");
                free(dirEntriesArrayPtr);
                closedir(dir);
                return;
            }

            // updates path and stores in updatedPath; strings will store with a slash separating previous path and current dirElement name
            sprintf(updatedPath, "%s/%s", path, dirEntriesArrayPtr[i].name);
            
        
            // printDir call for subdirectories , will print contents of subdirectory ; if there's another subDir printDir() is called again for the new subDir
            // indent increases for each level of subDir 
            // base case: when no subDir left
            /* after reaching the base case for a specific directory, function returns to the previous level of recursion 
            and continues with the next dirElements on that level of recursion until all elements done*/
            printDir(updatedPath, indent + 1, hiddenFlag, sizeFlag);
            // free memory
            free(updatedPath);
        }

        // free memory
        free(dirEntriesArrayPtr[i].name);
    }
    // free memory
    free(dirEntriesArrayPtr);
    // close directory
    closedir(dir);
}

int main(int argc, char *argv[]){
    // initialize size and hidden files flags
    int hiddenFlag = 0;
    int sizeFlag = 0;

    // if too many arguments inputted
    if(argc>4){
        perror("too many inputs\n");
        return 1;
    }

    // current directroy
    // if no path specified, current working directory will be displayed
    char *path = ".";
    // set flags properly before printDir() call
    for (int i = 1; i<argc; i++){
        // if size switch is triggered
        if(strcmp(argv[i],"-s") == 0){
            // set size flag to output size 
            sizeFlag = 1;
            // if hidden file switch is triggered
        } else if(strcmp(argv[i],"-a") == 0){
            // set hidden flag to output hidden file name
            hiddenFlag = 1;
        } else {
            // path is whatever user input
            path = argv[i];
        }
    }

    // call to print tree
    // takes in path specified by user
    // indent begins at 0
    // flags are passed for -a -s swithes
    printDir(path,0,hiddenFlag,sizeFlag);

    return 0;

}