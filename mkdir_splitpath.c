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

    // Start with handling the root directory case
    if (strcmp(pathName, "/") == 0) {
        strcpy(dirName, "/");
        strcpy(baseName, "");
        return root;  // Return root as it's the only directory
    }

    // Initialize dirName and baseName
    strcpy(dirName, "");
    strcpy(baseName, "");

    // Variables to keep track of traversal
    struct NODE* current = NULL;

    // Determine whether path is absolute or relative
    if (pathName[0] == '/') {
        current = root;  // Absolute path starts at root
    } else {
        current = cwd;   // Relative path starts at current working directory (cwd)
    }

    // Extract the baseName (the last part of the path after the last '/')
    char tempPath[256];
    strcpy(tempPath, pathName);

    char* lastSlash = strrchr(tempPath, '/');
    if (lastSlash != NULL) {
        // Split the path into dirName and baseName
        strncpy(dirName, tempPath, lastSlash - tempPath);
        dirName[lastSlash - tempPath] = '\0';
        strcpy(baseName, lastSlash + 1);
    } else {
        // No '/' in the path, meaning it's just a file or directory name in the cwd
        strcpy(baseName, tempPath);
        strcpy(dirName, "");  // Empty dirName for current directory
    }

    // Traverse the directory tree based on dirName
    if (strlen(dirName) > 0) {
        char* token = strtok(dirName, "/");
        while (token != NULL) {
            struct NODE* child = current->childPtr;
            int found = 0;

            // Search for the directory in the current node's children
            while (child != NULL) {
                if (strcmp(child->name, token) == 0 && child->fileType == 'd') {
                    // Found a matching directory
                    current = child;
                    found = 1;
                    break;
                }
                child = child->siblingPtr;
            }

            // If the directory doesn't exist, print an error and return NULL
            if (!found) {
                printf("ERROR: directory %s does not exist\n", token);
                return NULL;
            }

            // Continue to the next part of the path
            token = strtok(NULL, "/");
        }
    }

    // Return the pointer to the parent directory where the new directory/file should be created
    return current;
}
