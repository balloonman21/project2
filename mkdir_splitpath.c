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
    if (!newDir) {
        printf("MKDIR ERROR: memory allocation failed\n");
        return;
    }
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

// Initialize dirName and baseName to empty strings
    strcpy(dirName, "");
    strcpy(baseName, "");

    // Start from root if absolute path, else from cwd
    struct NODE* currentDir = (pathName[0] == '/') ? root : cwd;

    // Special case: if path is root "/"
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        strcpy(baseName, "");
        return root;
    }

    // Find the last '/' in the path
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash != NULL) {
        // Copy the directory part (everything before the last '/')
        strncpy(dirName, pathName, lastSlash - pathName);
        dirName[lastSlash - pathName] = '\0'; // Null-terminate dirName

        // Copy the file or directory name (everything after the last '/')
        strcpy(baseName, lastSlash + 1);
    } else {
        // If there is no '/', the entire pathName is the baseName
        strcpy(baseName, pathName);
        strcpy(dirName, "");
        return currentDir; // No directory path, stay in cwd or root
    }

    // If dirName is empty, we're in the current directory
    if (strlen(dirName) == 0) {
        return currentDir;
    }

    // Tokenize the dirName and traverse the directory tree
    char* token = strtok(dirName, "/");
    while (token != NULL) {
        struct NODE* nextDir = findDirectory(currentDir, token);
        if (nextDir == NULL) {
            // If directory does not exist, print error and return NULL
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }
        currentDir = nextDir;
        token = strtok(NULL, "/");
    }

    // Return the directory where baseName resides
    return currentDir;
}
