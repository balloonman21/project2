#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


extern struct NODE* root;
extern struct NODE* cwd;

struct NODE* findDirectory(struct NODE* parent, const char* dirName) {
    struct NODE* current = parent->childPtr;
    while (current != NULL) {
        if (strcmp(current->name, dirName) == 0 && current->fileType == 'd') {
            return current;
        }
        current = current->siblingPtr;
    }
    return NULL;
}


// Helper function to check if a directory or file already exists in the parent directory
int checkIfExists(struct NODE* parent, const char* baseName) {
    struct NODE* current = parent->childPtr;
    while (current != NULL) {
        if (strcmp(current->name, baseName) == 0) {
            return 1; // Found the directory or file
        }
        current = current->siblingPtr;
    }
    return 0;
}

// Helper function to add a new node (directory) to the parent's child/sibling list
void addNode(struct NODE* parent, struct NODE* newNode) {
    if (parent->childPtr == NULL) {
        // If parent has no child, add newNode as the first child
        parent->childPtr = newNode;
    } else {
        // Traverse sibling list and add newNode at the end
        struct NODE* current = parent->childPtr;
        while (current->siblingPtr != NULL) {
            current = current->siblingPtr;
        }
        current->siblingPtr = newNode;
    }
}
//make directory
void mkdir(char pathName[]){

  
    if (strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];
    char dirName[256];

    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    if (parentDir == NULL) {
        return; // Error printed by splitPath
    }

    if (checkIfExists(parentDir, baseName)) {
        printf("MKDIR ERROR: directory %s already exists\n", baseName);
        return;
    }

    struct NODE* newDir = (struct NODE*)malloc(sizeof(struct NODE));
    if (newDir == NULL) {
        printf("MKDIR ERROR: memory allocation failed\n");
        return;
    }

    strcpy(newDir->name, baseName);
    newDir->fileType = 'd';
    newDir->childPtr = NULL;
    newDir->siblingPtr = NULL;
    newDir->parentPtr = parentDir;

    addNode(parentDir, newDir);

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
