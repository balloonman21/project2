#include "types.h"
#include <string.h>

extern struct NODE* root;
extern struct NODE* cwd;

struct NODE* findDirectory(struct NODE* parent, const char* dirName) {
    struct NODE* currentChild = parent->childPtr;

    // Traverse through children and siblings to find the directory
    while (currentChild != NULL) {
        if (strcmp(currentChild->name, dirName) == 0 && currentChild->fileType == 'd') {
            return currentChild; // Directory found
        }
        currentChild = currentChild->siblingPtr; // Move to the next sibling
    }

    return NULL; // Directory not found
}

//make directory
void mkdir(char pathName[]){

  
        // If no path provided, print error
    if (strcmp(pathName, "/") == 0 || strlen(pathName) == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[256];
    char dirName[256];

    // Call splitPath to get baseName and dirName, and the parent directory
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);

    // If the directory is invalid (splitPath returned NULL), exit
    if (parentDir == NULL) {
        return;
    }

    // Check if a directory with the same name already exists in the parent directory
    struct NODE* existingDir = findDirectory(parentDir, baseName);
    if (existingDir != NULL) {
        printf("MKDIR ERROR: directory %s already exists\n", baseName);
        return;
    }

    // Create a new directory node
    struct NODE* newDir = (struct NODE*)malloc(sizeof(struct NODE));
    strcpy(newDir->name, baseName);
    newDir->fileType = 'd'; // Directory type
    newDir->parentPtr = parentDir;
    newDir->childPtr = NULL;
    newDir->siblingPtr = NULL;

    // Insert the new directory into the parent's child/sibling structure
    if (parentDir->childPtr == NULL) {
        // If the parent has no children, set this as the first child
        parentDir->childPtr = newDir;
    } else {
        // Otherwise, insert it as the last sibling of the existing children
        struct NODE* sibling = parentDir->childPtr;
        while (sibling->siblingPtr != NULL) {
            sibling = sibling->siblingPtr;
        }
        sibling->siblingPtr = newDir;
    }

    // Print success message
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

    // TO BE IMPLEMENTED
    // NOTE THAT WITHOUT COMPLETING THIS FUNCTION CORRECTLY
    // rm, rmdir, ls, cd, touch COMMANDS WILL NOT EXECUTE CORRECTLY
    // SEE THE PROVIDED EXECUTABLE TO SEE THEIR EXPECTED BEHAVIOR

    // YOUR CODE HERE
    //
    // char* pathCopy;
    // strcpy(pathName,pathCopy);
    // char* token = strtok(pathName, "/");
    // while(token!=NULL){
    //     strcat(pathName, token);
    //     if(token == NULL){
            
    //     }
    // }
        // Initialize dirName and baseName
    // Initialize dirName and baseName
    strcpy(dirName, "");
    strcpy(baseName, "");

    // Check for root case ("/")
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        strcpy(baseName, "");
        return &root; // Return the root node
    }

    // Split the path into dirName and baseName
    char* lastSlash = strrchr(pathName, '/'); // Find the last occurrence of '/'
    if (lastSlash != NULL) {
        strncpy(dirName, pathName, lastSlash - pathName); // Copy everything before the last '/'
        dirName[lastSlash - pathName] = '\0'; // Add null terminator to dirName
        strcpy(baseName, lastSlash + 1); // Copy everything after the last '/'
    } else {
        // No slash found, meaning pathName is the baseName
        strcpy(baseName, pathName);
        strcpy(dirName, ""); // Current directory
        return &root; // Assuming root is the current directory
    }

    // Traverse the directories in dirName starting from the root
    struct NODE* currentDir = &root;
    char* token = strtok(dirName, "/");
    while (token != NULL) {
        struct NODE* nextDir = findDirectory(currentDir, token);
        if (nextDir == NULL) {
            // Directory does not exist, print error and return NULL
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }
        currentDir = nextDir; // Move to the next directory
        token = strtok(NULL, "/"); // Get the next directory in the path
    }

    // After traversal, currentDir will be the directory where baseName resides
    return currentDir;
}
