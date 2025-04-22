#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define MAX_RESP_LENGTH 200

// Trie Node Structure
struct TrieNode {
    struct TrieNode* children[26];  // Assume lowercase English letters only
    int isEndOfWord;  // Flag to indicate if a word ends here
    char response[MAX_RESP_LENGTH]; // Bot's response to the word
};

// Linked List Node for storing conversation history
struct LinkedListNode {
    char userQuery[MAX_WORD_LENGTH];
    char botResponse[MAX_RESP_LENGTH];
    struct LinkedListNode* next;
};

// Queue Node for handling user inputs
struct QueueNode {
    char userInput[MAX_WORD_LENGTH];
    struct QueueNode* next;
};

// Trie Functions
struct TrieNode* createTrieNode() {
    struct TrieNode* node = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    node->isEndOfWord = 0;
    for (int i = 0; i < 26; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void insertWord(struct TrieNode* root, const char* word, const char* response) {
    struct TrieNode* current = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = tolower(word[i]) - 'a';
        if (!current->children[index]) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }
    current->isEndOfWord = 1;
    strncpy(current->response, response, MAX_RESP_LENGTH);
}

int searchWord(struct TrieNode* root, const char* word, char* response) {
    struct TrieNode* current = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = tolower(word[i]) - 'a';
        if (!current->children[index]) {
            return 0; // Word not found
        }
        current = current->children[index];
    }
    if (current != NULL && current->isEndOfWord) {
        strncpy(response, current->response, MAX_RESP_LENGTH);
        return 1; // Word found
    }
    return 0; // Word not found
}

// Linked List Functions
struct LinkedListNode* createLinkedListNode(const char* userQuery, const char* botResponse) {
    struct LinkedListNode* newNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
    strncpy(newNode->userQuery, userQuery, MAX_WORD_LENGTH);
    strncpy(newNode->botResponse, botResponse, MAX_RESP_LENGTH);
    newNode->next = NULL;
    return newNode;
}

void appendToHistory(struct LinkedListNode** head, const char* userQuery, const char* botResponse) {
    struct LinkedListNode* newNode = createLinkedListNode(userQuery, botResponse);
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct LinkedListNode* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

void printHistory(struct LinkedListNode* head) {
    struct LinkedListNode* temp = head;
    while (temp != NULL) {
        printf("User: %s\n", temp->userQuery);
        printf("Bot: %s\n", temp->botResponse);
        temp = temp->next;
    }
}

// Queue Functions
struct QueueNode* createQueueNode(const char* userInput) {
    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    strncpy(newNode->userInput, userInput, MAX_WORD_LENGTH);
    newNode->next = NULL;
    return newNode;
}

void enqueue(struct QueueNode** front, struct QueueNode** rear, const char* userInput) {
    struct QueueNode* newNode = createQueueNode(userInput);
    if (*rear == NULL) {
        *front = *rear = newNode;
        return;
    }
    (*rear)->next = newNode;
    *rear = newNode;
}

int dequeue(struct QueueNode** front, char* userInput) {
    if (*front == NULL) {
        return 0; // Queue is empty
    }
    struct QueueNode* temp = *front;
    *front = (*front)->next;
    strncpy(userInput, temp->userInput, MAX_WORD_LENGTH);
    free(temp);
    return 1;
}

// Main Bot Logic
void chatbot(struct TrieNode* root) {
    struct LinkedListNode* historyHead = NULL;
    struct QueueNode* front = NULL, *rear = NULL;
    char userInput[MAX_WORD_LENGTH];
    char botResponse[MAX_RESP_LENGTH];

    while (1) {
        printf("User: ");
        fgets(userInput, MAX_WORD_LENGTH, stdin);
        userInput[strcspn(userInput, "\n")] = '\0';  // Remove newline character

        if (strcmp(userInput, "exit") == 0) {
            printf("Exiting chatbot.\n");
            break;
        }

        // Add user input to queue
        enqueue(&front, &rear, userInput);

        // Process the user input
        if (dequeue(&front, userInput)) {
            if (searchWord(root, userInput, botResponse)) {
                printf("Bot: %s\n", botResponse);
                appendToHistory(&historyHead, userInput, botResponse);
            } else {
                printf("Bot: Sorry, I don't understand that.\n");
                appendToHistory(&historyHead, userInput, "Sorry, I don't understand that.");
            }
        }

        // Print conversation history
        printf("\nConversation History:\n");
        printHistory(historyHead);
        printf("\n");
    }
}

int main() {
    // Initialize Trie
    struct TrieNode* root = createTrieNode();

    // Predefined words and responses
    insertWord(root, "hello", "Hi there! How can I help you?");
    insertWord(root, "bye", "Goodbye! Have a nice day.");
    insertWord(root, "how are you", "I'm a bot, I'm always good!");
    insertWord(root, "thanks", "You're welcome!");
    
    // Start the chatbot
    chatbot(root);

    return 0;
}
