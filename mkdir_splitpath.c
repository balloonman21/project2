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

    // Call splitPath to get the parent directory and split dirName and baseName
    struct NODE* parentDir = splitPath(pathName, baseName, dirName);
    if (parentDir == NULL) {
        // If splitPath returns NULL, directory path doesn't exist
        return;
    }

    // Check if a node with baseName already exists in the parentDir
    struct NODE* temp = parentDir->childPtr;
    while (temp != NULL) {
        if (strcmp(temp->name, baseName) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", baseName);
            return;
        }
        temp = temp->siblingPtr;
    }

    // Allocate space for the new directory node
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    if (newNode == NULL) {
        printf("MKDIR ERROR: memory allocation failed\n");
        return;
    }

    // Set the fields for the new node
    strcpy(newNode->name, baseName);
    newNode->fileType = 'd'; // Mark it as a directory
    newNode->childPtr = NULL; // No children initially
    newNode->siblingPtr = NULL; // No siblings initially
    newNode->parentPtr = parentDir; // Set its parent to the parentDir

    // Attach the new node to the parent directory's child list
    struct NODE* child = parentDir->childPtr;
    if (child == NULL) {
        // Parent has no child yet, set newNode as the first child
        parentDir->childPtr = newNode;
    } else {
        // Traverse to the end of the sibling list and attach newNode there
        while (child->siblingPtr != NULL) {
            child = child->siblingPtr;
        }
        child->siblingPtr = newNode; // Attach as the last sibling
    }

    // Successfully created directory
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

// Initialize the dirName and baseName to empty strings
    strcpy(baseName, "");
    strcpy(dirName, "");

    // Edge case: if pathName is just "/"
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        strcpy(baseName, "");
        return root;
    }

    // Find the last occurrence of '/' in pathName
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash == NULL) {
        // Case where pathName contains only the file/dir name (no '/')
        strcpy(baseName, pathName);
        strcpy(dirName, ""); // Current directory
        return cwd; // Assume current working directory (cwd)
    }

    // Otherwise, split the path into dirName and baseName
    strncpy(dirName, pathName, lastSlash - pathName);
    dirName[lastSlash - pathName] = '\0'; // Null terminate dirName
    strcpy(baseName, lastSlash + 1); // The part after the last '/'

    // Traverse the tree from root or cwd depending on the path type (absolute/relative)
    struct NODE* currentDir = (pathName[0] == '/') ? root : cwd;

    // Tokenize the dirName by "/"
    char* token = strtok(dirName, "/");
    while (token != NULL) {
        struct NODE* found = NULL;
        struct NODE* temp = currentDir->childPtr;

        // Search for the directory in the current level
        while (temp != NULL) {
            if (strcmp(temp->name, token) == 0 && temp->fileType == 'd') {
                found = temp;
                break;
            }
            temp = temp->siblingPtr;
        }

        // If not found, print error and return NULL
        if (found == NULL) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }

        // Move to the next directory level
        currentDir = found;
        token = strtok(NULL, "/");
    }

    return currentDir; // Return the parent directory where the target resides
}
