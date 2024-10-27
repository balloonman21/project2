#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


extern struct NODE* root;
extern struct NODE* cwd;


void mkdir(char pathName[]){

  
char baseName[64], dirName[256];

    // If no path is provided, print an error
    if (strcmp(pathName, "/") == 0) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    // Call splitPath() to get the baseName and the parent directory node
    struct NODE* parent = splitPath(pathName, baseName, dirName);

    // If the parent directory does not exist, return early
    if (parent == NULL) {
        return;
    }

    // Check if baseName already exists in the parent directory
    struct NODE* temp = parent->childPtr;
    while (temp != NULL) {
        if (strcmp(temp->name, baseName) == 0 && temp->fileType == 'D') {
            printf("MKDIR ERROR: directory %s already exists\n", pathName);
            return;
        }
        temp = temp->siblingPtr;
    }

    // Allocate space for the new directory node
    struct NODE* newNode = (struct NODE*) malloc(sizeof(struct NODE));
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';  // 'D' for directory
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parent;

    // Attach the new node to the parent
    struct NODE* child = parent->childPtr;
    if (child == NULL) {
        parent->childPtr = newNode;  // First child
    } else {
        while (child->siblingPtr != NULL) {
            child = child->siblingPtr;
        }
        child->siblingPtr = newNode;  // Add as sibling
    }

    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

    struct NODE* current = NULL;
    
    // Case: pathName is empty or just root "/"
    if (strcmp(pathName, "/") == 0) {
        strcpy(baseName, "");
        strcpy(dirName, "/");
        return root;  // Return the root node
    }

    // Find the last '/' and split the pathName into dirName and baseName
    char* lastSlash = strrchr(pathName, '/');
    if (lastSlash == NULL) {
        // No '/' in pathName, implying the current directory
        strcpy(dirName, "");
        strcpy(baseName, pathName);
        return cwd;  // Return current working directory
    } else {
        // If there's a '/', split into dirName and baseName
        strncpy(dirName, pathName, lastSlash - pathName); // Copy dir part
        dirName[lastSlash - pathName] = '\0';             // Null-terminate
        strcpy(baseName, lastSlash + 1);                  // Base name after '/'
    }

    // If dirName is empty, stay in current directory
    if (strlen(dirName) == 0) {
        return cwd;
    }

    // Traverse from root or cwd based on absolute or relative path
    current = (pathName[0] == '/') ? root : cwd;

    char* token = strtok(dirName, "/");
    while (token != NULL) {
        struct NODE* temp = current->childPtr;
        int found = 0;

        // Search for directory in the current node's children
        while (temp != NULL) {
            if (strcmp(temp->name, token) == 0 && temp->fileType == 'D') {
                found = 1;
                current = temp;  // Move to the next level in the path
                break;
            }
            temp = temp->siblingPtr;
        }

        // If directory not found, print error and return NULL
        if (!found) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }

        token = strtok(NULL, "/");  // Move to the next token
    }

    return current;
}
