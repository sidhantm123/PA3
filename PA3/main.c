#include <stdio.h>
#include <stdlib.h>

// Structure to represent a Rectangle
typedef struct Rectangle {
    int label;
    int width;
    int height;
} Rectangle;

// Structure to represent a Node in the binary tree
typedef struct Node {
    char type; // 'V' or 'H' for cutline, or 0 for Rectangle
    Rectangle *rect; // Pointer to Rectangle for leaf nodes
    struct Node *left;
    struct Node *right;
} Node;

// Function prototypes
Node* create_node(char type);
Rectangle* create_rectangle(int label, int width, int height);
Node* build_tree(FILE *input_file);
void postorder_traversal(Node *node, FILE *output_file);
void Compute_dimension(Node *node, FILE *output_file);
void Compute_coordinates(Node *node, int x, int y, FILE *output_file); // New function prototype
void free_tree(Node *node);

int main(int argc, char *argv[]) {
    // Check if correct number of command-line arguments are provided
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> <output_file1> <output_file2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open input file
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Build the binary tree
    Node *root = build_tree(input_file);
    fclose(input_file);

    // Open output file for post-order traversal
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    // Perform post-order traversal and write to output file 1
    postorder_traversal(root, output_file);
    fclose(output_file);

    // Open output file for computing dimensions
    FILE *output_file1 = fopen(argv[3], "w");
    if (output_file1 == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", argv[3]);
        return EXIT_FAILURE;
    }

    // Compute the dimensions and write into output file 2
    Compute_dimension(root, output_file1);
    fclose(output_file1);

    // Open output file for writing the coordinates
    FILE *output_file2 = fopen(argv[4], "w");
    if (output_file2 == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", argv[4]);
        return EXIT_FAILURE;
    }

    // Calculate the coordinates and write to output file 3
    Compute_coordinates(root, 0, 0, output_file2);
    fclose(output_file2);
    free_tree(root);
    return EXIT_SUCCESS;
}

// Function to create a new node
Node* create_node(char type) {
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for node\n");
        exit(EXIT_FAILURE);
    }
    new_node->type = type;
    new_node->rect = NULL;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

// Function to create a new rectangle
Rectangle* create_rectangle(int label, int width, int height) {
    Rectangle *rect = (Rectangle*)malloc(sizeof(Rectangle));
    if (rect == NULL) {
        fprintf(stderr, "Memory allocation failed for rectangle\n");
        exit(EXIT_FAILURE);
    }
    rect->label = label;
    rect->width = width;
    rect->height = height;
    return rect;
}

// Function to build the binary tree from input file
Node* build_tree(FILE *input_file) {
    char line[20]; // Assuming maximum line length is 20 characters

    // Read the first line of the input file
    if(fgets(line, sizeof(line), input_file) == NULL){
        return NULL;
    }
    char type = line[0];

    // Create the root node
    Node *root = create_node(type);

    // If the root is a leaf node (rectangle), create a rectangle object and assign it to the root
    if (type != 'V' && type != 'H') {
        int label, width, height;
        sscanf(line, "%d(%d,%d)\n", &label, &width, &height);
        root->rect = create_rectangle(label, width, height);
    } else {
        // If the root is an internal node, recursively build the left and right subtrees
        root->left = build_tree(input_file);
        root->right = build_tree(input_file);
    }

    return root;
}

// Function to perform post-order traversal of the binary tree and write to output file
void postorder_traversal(Node *node, FILE *output_file) {
    if (node == NULL) return;

    // Traverse left subtree
    postorder_traversal(node->left, output_file);

    // Traverse right subtree
    postorder_traversal(node->right, output_file);

    // Visit the node
    if (node->type != 'V' && node->type != 'H') {
        // If node is a leaf node (rectangle), print rectangle details
        fprintf(output_file, "%d(%d,%d)\n", node->rect->label, node->rect->width, node->rect->height);
    } else {
        // If node is an internal node, print its type
        fprintf(output_file, "%c\n", node->type);
    }
}

// Function to perform post-order traversal of the binary tree and write to the output file
void Compute_dimension(Node *node, FILE *output_file) {
    if (node == NULL) return;

    // Traverse left subtree
    Compute_dimension(node->left, output_file);

    // Traverse right subtree
    Compute_dimension(node->right, output_file);

    // Visit the node
    if (node->type != 'V' && node->type != 'H') {
        // If node is a leaf node (rectangle), print rectangle details
        fprintf(output_file, "%d(%d,%d)\n", node->rect->label, node->rect->width, node->rect->height);
    } else {
        // If node is an internal node
        int width = 0, height = 0;

        // Calculate the dimensions of the smallest rectangular room enclosing all rectangles in the subtree
        if (node->left != NULL && node->left->rect != NULL) {
            width += node->left->rect->width;
            height = node->left->rect->height;
        }

        // Print the internal node with calculated dimensions
        if (node->type == 'V') {
            if (node->right != NULL && node->right->rect != NULL) 
            {
                height = (node->right->rect->height < height) ? height : node->right->rect->height;
                width += node->right->rect->width;
                node->rect = create_rectangle(0, width, height);
            }
            fprintf(output_file, "%c(%d,%d)\n", node->type, width, height);
        } else if (node->type == 'H') {
            if (node->right != NULL && node->right->rect != NULL) 
            {
                width = (node->right->rect->width < width) ? width : node->right->rect->width;
                height += node->right->rect->height;
                node->rect = create_rectangle(0, width, height);
            }
            fprintf(output_file, "%c(%d,%d)\n", node->type,  width, height); // Swap width and height for 'H' nodes
        }
    }
}

// Function to compute the coordinates of each rectangle in the binary tree
void Compute_coordinates(Node *node, int x, int y, FILE *output_file) {
    if (node == NULL) return;

    // Visit the node
    if (node->type != 'V' && node->type != 'H') {
        // If node is a leaf node (rectangle), compute and print its coordinates
        fprintf(output_file, "%d((%d,%d)(%d,%d))\n", node->rect->label,node->rect->width,node->rect->height, x, y);
    } else {
        // If node is an internal node, compute coordinates for its children recursively
        int left_x = x, left_y = y;
        int right_x = x, right_y = y;

        if (node->left != NULL && node->left->rect != NULL) {
            // Compute coordinates for left child
            if (node->type == 'V') {
                // For vertical cut, adjust x coordinate for left child
                left_x = x;
                left_y = y;
            } else if (node->type == 'H') {
                // For horizontal cut, adjust y coordinate for left child
                left_x = x;
                left_y = y + node->left->rect->height;
            }
            Compute_coordinates(node->left, left_x, left_y, output_file);
        }

        if (node->right != NULL && node->right->rect != NULL) {
            // Compute coordinates for right child
            if (node->type == 'V') {
                // For vertical cut, adjust x coordinate for right child
                right_x = x + node->left->rect->width;
                right_y = y;
            } else if (node->type == 'H') {
                // For horizontal cut, adjust y coordinate for right child
                right_x = x;
                right_y = y;
            }
            Compute_coordinates(node->right, right_x, right_y, output_file);
        }
    }
}

void free_tree(Node *node) {
    if (node == NULL) return;

    // Free left and right subtrees recursively
    free_tree(node->left);
    free_tree(node->right);

    // Free the current node
    if (node->rect != NULL) {
        free(node->rect);
    }
    free(node);
}
