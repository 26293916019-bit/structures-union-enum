/* Enumerations : command driven task list.  The enum decides which operation
   runs.  Build: gcc -std=c11 -Wall -Wextra 7.c -o 7 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* enum for command types */
typedef enum { CMD_ADD, CMD_DELETE, CMD_VIEW, CMD_UPDATE, CMD_COMPLETE,
               CMD_FIND, CMD_HELP, CMD_EXIT, CMD_UNKNOWN } CommandType;
/* enum for the status of a task */
typedef enum { PENDING, DONE } Status;
typedef struct { int id; char desc[100]; Status status; } Task;

Task *list = NULL;
int count = 0, capacity = 0;

int read_line(char *buf, int size);
int ask(const char *prompt);
void ask_text(const char *prompt, char *buf, int size);
/* converts command text to a CommandType */
CommandType get_command(const char *text);
int find_task(int id);
void show(const Task *t);
void add_task(void);
void delete_task(void);
void update_task(void);
void complete_task(void);
void find_one(void);
void view(void);

/* main program */
int main(void)
{
    char line[64];
    int running = 1, choice;
    printf("Type HELP for the command list.\n");
    while (running) {
        printf("\ntask> ");
        choice = read_line(line, sizeof line);
        if (choice == 0) { printf("\nEnd of input.\n"); break; }
        if (choice < 0) { printf("Command too long.\n"); continue; }
        switch (get_command(line)) {
        case CMD_ADD: add_task(); break;
        case CMD_DELETE: delete_task(); break;
        case CMD_VIEW: view(); break;
        case CMD_UPDATE: update_task(); break;
        case CMD_COMPLETE: complete_task(); break;
        case CMD_FIND: find_one(); break;
        case CMD_HELP:
            printf("ADD      add a task (ID and description)\n");
            printf("DELETE   remove a task by ID\n");
            printf("VIEW     list all tasks\n");
            printf("UPDATE   change a description\n");
            printf("COMPLETE mark a task DONE\n");
            printf("FIND     show one task by ID\n");
            printf("HELP     show this list\n");
            printf("EXIT     leave the program\n");
            break;
        case CMD_EXIT: running = 0; break;
        default: if (line[0] != '\0') { printf("Unknown command.\n"); } break;
        }
    }
    free(list);
    printf("Memory freed.\n");
    return 0;
}

int read_line(char *buf, int size)
{
    int len, c;
    if (fgets(buf, size, stdin) == NULL) { return 0; }
    len = (int)strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') { buf[len - 1] = '\0'; return 1; }
    while ((c = getchar()) != '\n' && c != EOF) { }
    return -1;
}

int ask(const char *prompt)
{
    char line[64], *end;
    long value;
    while (1) {
        printf("%s", prompt);
        if (!read_line(line, sizeof line)) { printf("\n"); exit(EXIT_SUCCESS); }
        value = strtol(line, &end, 10);
        while (*end == ' ' || *end == '\t') { end++; }
        if (end != line && *end == '\0') { return (int)value; }
        printf("Please enter a whole number.\n");
    }
}

void ask_text(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (!read_line(buf, size)) { printf("\n"); exit(EXIT_SUCCESS); }
}

CommandType get_command(const char *text)
{
    char up[32];
    int i;
    if (strlen(text) >= sizeof up) { return CMD_UNKNOWN; }
    strcpy(up, text);
    for (i = 0; up[i] != '\0'; i++) { up[i] = (char)toupper((unsigned char)up[i]); }
    if (!strcmp(up, "ADD")) { return CMD_ADD; }
    if (!strcmp(up, "DELETE")) { return CMD_DELETE; }
    if (!strcmp(up, "VIEW")) { return CMD_VIEW; }
    if (!strcmp(up, "UPDATE")) { return CMD_UPDATE; }
    if (!strcmp(up, "COMPLETE")) { return CMD_COMPLETE; }
    if (!strcmp(up, "FIND")) { return CMD_FIND; }
    if (!strcmp(up, "HELP")) { return CMD_HELP; }
    if (!strcmp(up, "EXIT")) { return CMD_EXIT; }
    return CMD_UNKNOWN;
}

int find_task(int id)
{
    int i;
    for (i = 0; i < count; i++) {
        if (list[i].id == id) { return i; }
    }
    return -1;
}

void show(const Task *t)
{
    printf("  %d  %-8s %s\n", t->id, t->status == DONE ? "DONE" : "PENDING",
           t->desc);
}

void add_task(void)
{
    Task t, *tmp;
    t.id = ask("Task ID: ");
    ask_text("Description: ", t.desc, sizeof t.desc);
    if (t.id < 1 || t.desc[0] == '\0') { printf("Invalid ID or description.\n"); return; }
    if (find_task(t.id) >= 0) { printf("Task %d exists.\n", t.id); return; }
    if (count == capacity) {
        tmp = realloc(list, (capacity + 1) * sizeof(Task));
        if (tmp == NULL) { printf("Out of memory.\n"); return; }
        list = tmp;
        capacity++;
    }
    t.status = PENDING;
    list[count++] = t;
    printf("Task %d added.\n", t.id);
}

void delete_task(void)
{
    int i = find_task(ask("Task ID: "));
    int id, j;
    if (i < 0) { printf("No such task.\n"); return; }
    id = list[i].id;
    for (j = i; j < count - 1; j++) { list[j] = list[j + 1]; }
    count--;
    printf("Task %d deleted.\n", id);
}

void update_task(void)
{
    int i = find_task(ask("Task ID: "));
    if (i < 0) { printf("No such task.\n"); return; }
    ask_text("New description: ", list[i].desc, sizeof list[i].desc);
    printf("Task %d updated.\n", list[i].id);
}

void complete_task(void)
{
    int i = find_task(ask("Task ID: "));
    if (i < 0) { printf("No such task.\n"); return; }
    list[i].status = DONE;
    printf("Task %d marked DONE.\n", list[i].id);
}

void find_one(void)
{
    int i = find_task(ask("Task ID: "));
    if (i < 0) { printf("No such task.\n"); return; }
    show(&list[i]);
}

void view(void)
{
    int i;
    if (count == 0) { printf("Task list is empty.\n"); return; }
    for (i = 0; i < count; i++) { show(&list[i]); }
}
