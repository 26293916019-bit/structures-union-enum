/* Structures - 1 : student records, extended to a menu driven system with
   add, display, search, update, delete, GPA summary and a sorted list.
   Build: gcc -std=c11 -Wall -Wextra 4.c -o 4 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* structure for one student record */
typedef struct { int roll; char name[50]; int age; float gpa; } Student;

Student *list = NULL;
int count = 0, capacity = 0;

/* function declarations */
int read_line(char *buf, int size);
double ask(const char *prompt);
void ask_text(const char *prompt, char *buf, int size);
int find_student(int roll);
void show(const Student *s);
void add_student(void);
void display(void);
void search(void);
void update(void);
void delete_student(void);
void summary(void);
int cmp_gpa(const void *a, const void *b);
void sorted(void);

int main(void)
{
    int choice, cap;
    cap = (int)ask("Initial capacity: ");
    if (cap < 1 || (size_t)cap > SIZE_MAX / sizeof(Student)) {
        printf("Invalid capacity.\n");
        return 1;
    }
    list = malloc((size_t)cap * sizeof(Student));
    if (list == NULL) { printf("Out of memory.\n"); return 1; }
    capacity = cap;
    while (1) {
        printf("\n1.Add 2.Display 3.Search 4.Update 5.Delete 6.Summary 7.Sorted 0.Exit\n");
        choice = (int)ask("Choice: ");
        switch (choice) {
        case 1: add_student(); break;
        case 2: display(); break;
        case 3: search(); break;
        case 4: update(); break;
        case 5: delete_student(); break;
        case 6: summary(); break;
        case 7: sorted(); break;
        case 0: free(list); printf("Memory freed.\n"); return 0;
        default: printf("Wrong choice.\n"); break;
        }
    }
}

int read_line(char *buf, int size)
{
    int len, c;
    if (fgets(buf, size, stdin) == NULL) { return 0; }
    len = (int)strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') { buf[len - 1] = '\0'; return 1; }
    while ((c = getchar()) != '\n' && c != EOF) { }
    return 1;
}

double ask(const char *prompt)
{
    char line[64], *end;
    double value;
    while (1) {
        printf("%s", prompt);
        if (!read_line(line, sizeof line)) { printf("\n"); exit(EXIT_SUCCESS); }
        value = strtod(line, &end);
        while (*end == ' ' || *end == '\t') { end++; }
        if (end != line && *end == '\0') { return value; }
        printf("Please enter a number.\n");
    }
}

void ask_text(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (!read_line(buf, size)) { printf("\n"); exit(EXIT_SUCCESS); }
}

int find_student(int roll)
{
    int i;
    for (i = 0; i < count; i++) {
        if (list[i].roll == roll) { return i; }
    }
    return -1;
}

void show(const Student *s)
{
    printf("  %d  %-20s age %d  GPA %.2f\n", s->roll, s->name, s->age, s->gpa);
}

void add_student(void)
{
    Student s, *tmp;
    char line[80];
    s.roll = (int)ask("Roll: ");
    ask_text("Name: ", line, sizeof line);
    if (strlen(line) >= sizeof s.name) { printf("Name too long.\n"); return; }
    strcpy(s.name, line);
    s.age = (int)ask("Age: ");
    s.gpa = (float)ask("GPA: ");
    if (s.roll < 1 || s.name[0] == '\0' || s.age < 16 || s.age > 100 ||
        s.gpa < 0 || s.gpa > 4) {
        printf("Invalid details.\n");
        return;
    }
    if (find_student(s.roll) >= 0) { printf("Roll %d exists.\n", s.roll); return; }
    if (count == capacity) {
        tmp = realloc(list, (capacity + 1) * sizeof(Student));
        if (tmp == NULL) { printf("Out of memory.\n"); return; }
        list = tmp;
        capacity++;
    }
    list[count++] = s;
    printf("Student %d added.\n", s.roll);
}

void display(void)
{
    int i;
    if (count == 0) { printf("No records.\n"); return; }
    for (i = 0; i < count; i++) { show(&list[i]); }
}

void search(void)
{
    int i = find_student((int)ask("Roll to search: "));
    if (i < 0) { printf("Not found.\n"); return; }
    show(&list[i]);
}

void update(void)
{
    int i = find_student((int)ask("Roll to update: "));
    char line[80];
    int age;
    float gpa;
    if (i < 0) { printf("Not found.\n"); return; }
    ask_text("New name: ", line, sizeof line);
    if (strlen(line) >= sizeof list[i].name) { printf("Name too long.\n"); return; }
    age = (int)ask("New age: ");
    gpa = (float)ask("New GPA: ");
    if (line[0] == '\0' || age < 16 || age > 100 || gpa < 0 || gpa > 4) {
        printf("Invalid details.\n");
        return;
    }
    strcpy(list[i].name, line);
    list[i].age = age;
    list[i].gpa = gpa;
    printf("Roll %d updated.\n", list[i].roll);
}

void delete_student(void)
{
    int i = find_student((int)ask("Roll to delete: "));
    int roll, j;
    if (i < 0) { printf("Not found.\n"); return; }
    roll = list[i].roll;
    for (j = i; j < count - 1; j++) { list[j] = list[j + 1]; }
    count--;
    printf("Roll %d deleted.\n", roll);
}

void summary(void)
{
    int i, high = 0, low = 0;
    float total = 0.0f;
    if (count == 0) { printf("No records.\n"); return; }
    for (i = 0; i < count; i++) {
        total += list[i].gpa;
        if (list[i].gpa > list[high].gpa) { high = i; }
        if (list[i].gpa < list[low].gpa) { low = i; }
    }
    printf("Highest: "); show(&list[high]);
    printf("Lowest : "); show(&list[low]);
    printf("Average GPA: %.2f over %d record(s)\n", total / count, count);
}

int cmp_gpa(const void *a, const void *b)
{
    const Student *x = (const Student *)a;
    const Student *y = (const Student *)b;
    if (x->gpa > y->gpa) { return -1; }
    if (x->gpa < y->gpa) { return 1; }
    return x->roll - y->roll;
}

/* sorted view works on a copy, so insertion order is kept */
void sorted(void)
{
    Student *copy;
    int i;
    if (count == 0) { printf("No records.\n"); return; }
    copy = malloc(count * sizeof(Student));
    if (copy == NULL) { printf("Out of memory.\n"); return; }
    memcpy(copy, list, count * sizeof(Student));
    qsort(copy, count, sizeof(Student), cmp_gpa);
    for (i = 0; i < count; i++) { show(&copy[i]); }
    free(copy);
}
