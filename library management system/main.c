#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_USERS 100
#define MAX_ROWS 500
#define MAX_FIELD_SIZE 256
typedef struct
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} User;
typedef struct
{
    char book_name[MAX_FIELD_SIZE];
    char author[MAX_FIELD_SIZE];
    int num_copies;
} BookInfo;
int choice;
BookInfo books[MAX_ROWS];
int num_books = 0;
int read_csv(const char *filename, BookInfo books[MAX_ROWS], int *num_books)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Could not open file %s\n", filename);
        return -1;
    }

    *num_books = 0;
    char line[MAX_FIELD_SIZE];
    while (fgets(line, sizeof(line), file) && *num_books < MAX_ROWS)
    {
        char *token = strtok(line, ",");
        if (token != NULL)
        {
            strcpy(books[*num_books].book_name, token);
            token = strtok(NULL, ",");
            if (token != NULL)
            {
                strcpy(books[*num_books].author, token);
                token = strtok(NULL, ",");
                if (token != NULL)
                {
                    books[*num_books].num_copies = atoi(token);
                    (*num_books)++;
                }
            }
        }
    }

    fclose(file);
    return 0;
}
void print_books(BookInfo books[MAX_ROWS], int num_books)
{
    printf("Book Information:\n");
    int i;
    for (i = 0; i < num_books; i++)
    {
        if (books[i].num_copies > 0)
        {
            printf("%d. Book Name: %s, Author: %s, Number of Copies: %d\n", i + 1,
                   books[i].book_name, books[i].author, books[i].num_copies);
        }
    }
}

void add_book(BookInfo books[MAX_ROWS], int *num_books)
{
    getchar();
    if (*num_books >= MAX_ROWS)
    {
        printf("Cannot add more books. Maximum limit reached.\n");
        return;
    }

    printf("Enter book name: ");
    fgets(books[*num_books].book_name, sizeof(books[*num_books].book_name), stdin);
    if (books[*num_books].book_name[strlen(books[*num_books].book_name) - 1] == '\n')
        books[*num_books].book_name[strlen(books[*num_books].book_name) - 1] = '\0';

    printf("Enter author name: ");
    fgets(books[*num_books].author, sizeof(books[*num_books].author), stdin);
    if (books[*num_books].author[strlen(books[*num_books].author) - 1] == '\n')
        books[*num_books].author[strlen(books[*num_books].author) - 1] = '\0';

    printf("Enter number of copies: ");
    scanf("%d", &books[*num_books].num_copies);
    getchar();

    (*num_books)++;
}
void delete_book(BookInfo books[MAX_ROWS], int *num_books, int index)
{
    if (index < 0 || index >= *num_books+1)
    {
        printf("Invalid index.\n");
        return;
    }
    index-=1;
    int i;
    for ( i = index; i < *num_books - 1; i++)
    {
        strcpy(books[i].book_name, books[i + 1].book_name);
        strcpy(books[i].author, books[i + 1].author);
        books[i].num_copies = books[i + 1].num_copies;
    }

    (*num_books)--;
}
void write_csv(const char *filename, BookInfo books[MAX_ROWS], int num_books)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Could not open file %s\n", filename);
        return;
    }
    int i;
    for ( i = 0; i < num_books; i++)
    {
        fprintf(file, "%s,%s,%d\n", books[i].book_name, books[i].author, books[i].num_copies);
    }

    fclose(file);
}
void create_user(const char *username, const char *password)
{
    FILE *file = fopen("users.csv", "a");
    if (!file)
    {
        printf("Error opening file.\n");
        exit(1);
    }

    fprintf(file, "%s,%s\n", username, password);
    fclose(file);
}
int check_user(const char *username, const char *password)
{

    FILE *file = fopen("users.csv", "r");
    if (!file)
    {
        printf("Error opening file.\n");
        exit(1);
    }

    char line[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2];
    while (fgets(line, sizeof(line), file))
    {
        char *token;
        char user[MAX_USERNAME_LENGTH];
        char pass[MAX_PASSWORD_LENGTH];

        token = strtok(line, ",");
        strcpy(user, token);
        token = strtok(NULL, ",");
        strcpy(pass, token);
        pass[strlen(pass) - 1] = '\0';

        if (strcmp(username, user) == 0 && strcmp(password, pass) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}
int is_username_taken(const char *username)
{
    FILE *file = fopen("users.csv", "r");
    if (!file)
    {
        printf("Error opening file.\n");
        exit(1);
    }

    char line[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2];
    while (fgets(line, sizeof(line), file))
    {
        char *token;
        char user[MAX_USERNAME_LENGTH];

        token = strtok(line, ",");
        strcpy(user, token);

        if (strcmp(username, user) == 0)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}
void signup()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter username: ");
    scanf("%s", username);

    if (is_username_taken(username))
    {
        printf("Username already taken. Please choose another.\n");
        return;
    }

    printf("Enter password: ");
    scanf("%s", password);

    create_user(username, password);
    printf("User created successfully!\n");
}
void search_books_by_author(const char *username, const char *author, BookInfo books[], int num_books) {
    // Create an array to store found books
    BookInfo found_books[MAX_ROWS];
    int found_count = 0;
    int i,j;
    // Convert the author name to lowercase for case-insensitive search
    char author_lowercase[MAX_FIELD_SIZE];
    strcpy(author_lowercase, author);
    for (i = 0; author_lowercase[i]; i++) {
        author_lowercase[i] = tolower(author_lowercase[i]);
    }

    // Search for books with the specified author (or part of it)
    for (i = 0; i < num_books; i++) {
        char book_author_lowercase[MAX_FIELD_SIZE];
        strcpy(book_author_lowercase, books[i].author);
        for (j = 0; book_author_lowercase[j]; j++) {
            book_author_lowercase[j] = tolower(book_author_lowercase[j]);
        }

        // Check if the author name (or part of it) is found in the book's author
        if (strstr(book_author_lowercase, author_lowercase) != NULL) {
            // Add the book to the found_books array
            strcpy(found_books[found_count].book_name, books[i].book_name);
            strcpy(found_books[found_count].author, books[i].author);
            found_books[found_count].num_copies = books[i].num_copies;
            found_count++;
        }
    }

    // Print the found books
    if (found_count > 0) {
        printf("Books found by author '%s':\n", author);
        for (i = 0; i < found_count; i++) {
            printf("%d. Book Name: %s, Author: %s, Number of Copies: %d\n", i + 1,
                   found_books[i].book_name, found_books[i].author, found_books[i].num_copies);
        }

        // Ask if the user wants to lend one of the found books
        printf("Would you like to lend one of them? (1 for yes, 0 for no): ");
        int answer;
        scanf("%d", &answer);
        if (answer == 1) {
                system("cls");
                lend_book(username, found_books,found_count);
        }
    }
    else {
        printf("No books found by author '%s'.\n", author);
    }
}
void menu(const char *username)
{
    int choice;

    while (1)
    {
        printf("\n1. Check lent books\n");
        printf("2. Return book\n");
        printf("3. Lend book\n");
        printf("4. Check all books\n");
        printf("5. Donate a book\n");
        printf("6. Search book\n");
        printf("7. Log out\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            system("cls");
            check_lent_books(username);
            Sleep(1000);
            break;
        case 2:
            system("cls");
            return_book(username,books, num_books);
            Sleep(1000);
            break;
        case 3:
            system("cls");
            lend_book(username,books, num_books);
            Sleep(1000);
            system("cls");
            break;
        case 4:
            system("cls");
            print_books(books, num_books);
            Sleep(1000);
            break;
        case 5:
            system("cls");
            add_book(books, &num_books);
            Sleep(1000);
            break;
        case 6:
            system("cls");
            printf("Enter author name or part of it to search for books: ");
            char author_to_search[MAX_FIELD_SIZE];
            scanf("%s", author_to_search);
            search_books_by_author(username, author_to_search, books, num_books);
            Sleep(1000);
            system("cls");
            break;
        case 7:
            write_csv("data.csv", books, num_books);
            system("cls");
            return;
        default:
            printf("Invalid choice. Please enter again.\n");
        }
    }
}

void login()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    if (check_user(username, password))
    {
        printf("Login successful!\n");
        Sleep(2000);
        system("cls");
        menu(username);
    }
    else
    {
        printf("Invalid username or password!\n");
    }
}

void check_lent_books(const char *username, BookInfo books[], int num_books)
{
    // Open the lentbooks.csv file for reading
    FILE *file = fopen("lentbooks.csv", "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    // Variable to store the book lent by the user
    char bookName[MAX_FIELD_SIZE];
    char author[MAX_FIELD_SIZE];

    // Read the file line by line
    char line[256];
    int userFound = 0;
    while (fgets(line, sizeof(line), file))
    {
        char user[MAX_FIELD_SIZE];
        // Tokenize the line to extract user, book name, and author
        sscanf(line, "%[^,],%[^,],%[^\n]", user, bookName, author);

        // Remove newline character from author
        author[strcspn(author, "\n")] = '\0';

        // Check if the current line corresponds to the specified user
        if (strcmp(user, username) == 0)
        {
            userFound = 1; // User found, set flag
            // Print book information
            printf("Book Name: %s, Author: %s\n", bookName, author);
        }
    }

    // Close the file
    fclose(file);

    if (!userFound)
    {
        printf("User hasn't lent any books.\n");
    }
}

int compare_strings(const char *str1, const char *str2)
{
    while (*str1 && *str2)
    {
        // Skip leading whitespace
        while (isspace(*str1)) str1++;
        while (isspace(*str2)) str2++;

        // Convert characters to lowercase and compare
        if (tolower(*str1) != tolower(*str2))
        {
            return 0; // Strings are different
        }

        str1++;
        str2++;
    }

    // Check if both strings are at the end
    if (*str1 == '\0' && *str2 == '\0')
    {
        return 1; // Strings are the same
    }

    // Check if only one string is at the end
    if (*str1 == '\0' && isspace(*str2))
    {
        while (isspace(*str2)) str2++;
        if (*str2 == '\0') return 1; // Strings are the same
    }

    if (*str2 == '\0' && isspace(*str1))
    {
        while (isspace(*str1)) str1++;
        if (*str1 == '\0') return 1; // Strings are the same
    }

    return 0; // Strings are different
}
void return_book(const char *username, BookInfo books[], int num_books) {
    // Open the CSV file for reading
    FILE *file = fopen("lentbooks.csv", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    // Temporary array to store lent books
    BookInfo lent_books[5];
    int num_lent_books = 0;
    int i,j;
    // Read each line of the file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Parse the line
        char user[MAX_FIELD_SIZE];
        char book_name[MAX_FIELD_SIZE];
        char author[MAX_FIELD_SIZE];
        sscanf(line, "%[^,],%[^,],%[^\n]", user, book_name, author);

        // Check if the book belongs to the given user
        if (strcmp(user, username) == 0) {
            strcpy(lent_books[num_lent_books].book_name, book_name);
            strcpy(lent_books[num_lent_books].author, author);
            num_lent_books++;
        }
    }
    fclose(file);

    // Display lent books to the user
    printf("Books lent to %s:\n", username);
    for (i = 0; i < num_lent_books; i++) {
        printf("%d. %s by %s\n", i + 1, lent_books[i].book_name, lent_books[i].author);
    }

    // Ask user which books to return
    printf("Enter the number of the book to return (0 to return all): ");
    int choice;
    scanf("%d", &choice);

    // Update CSV file to remove returned books
    file = fopen("lentbooks.csv", "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    if (choice == 0) {
        // Return all books, so don't write them back to file
        for (i = 0; i < num_lent_books; i++) {
            for (j = 0; j < num_books; j++) {
                if (strcmp(books[j].book_name, lent_books[i].book_name) == 0 &&
                    strcmp(books[j].author, lent_books[i].author) == 0) {
                    books[j].num_copies++;
                }
            }
        }
        num_lent_books = 0; // Set the number of lent books to 0 as all are returned
    } else if (choice > 0 && choice <= num_lent_books) {
        // Write back lent books except the chosen one
        for (i = 0; i < num_lent_books; i++) {
            if (i != choice - 1) {
                fprintf(file, "%s,%s,%s\n", username, lent_books[i].book_name, lent_books[i].author);
                for (j = 0; j < num_books; j++) {
                    if (strcmp(books[j].book_name, lent_books[i].book_name) == 0 &&
                        strcmp(books[j].author, lent_books[i].author) == 0) {
                        books[j].num_copies++;
                    }
                }
            }
        }
    } else {
        printf("Invalid choice.\n");
    }
    fclose(file);
}

void lend_book(const char *username, BookInfo books[], int num_books)
{
    print_books(books, num_books);
    int index;
    printf("Enter the index of the book you want to lend: ");
    scanf("%d", &index);
    getchar(); // Consume newline character

    if (index < 1 || index > num_books)
    {
        printf("Invalid index.\n");
        return;
    }

    // Open the file in append mode
    FILE *file = fopen("lentbooks.csv", "a");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    fprintf(file, "%s,%s,%s\n",username, books[index - 1].book_name, books[index - 1].author);
    fclose(file);
    books[index - 1].num_copies--;
    printf("Book lent successfully.\n");
}
int main()
{
    read_csv("data.csv", books, &num_books);
    while (1)
    {
        printf("\n1. Sign Up\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            signup();
            break;
        case 2:
            login();
            break;
        case 3:
            return 0;
        default:
            printf("Invalid choice. Please enter again.\n");
        }
    }

    return 0;
}

