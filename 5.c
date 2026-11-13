/* Structures - 2 : rectangle inventory, extended with add, display, search,
   update, delete, statistics, a sorted list and an area filter.
   Build: gcc -std=c11 -Wall -Wextra 5.c -o 5 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOL 1e-9  /* relative tolerance used for the square test */

/* structure for one rectangle, area and perimeter are derived */
typedef struct { int id; double w, h, area, perimeter; } Rect;

Rect *list = NULL;
int count = 0, capacity = 0;

/* function declarations */
int read_line(char *buf, int size);
double ask(const char *prompt);
int find_rect(int id);
void refresh(Rect *r);
void show(const Rect *r);
void add_rect(void);
void display(void);
void search(void);
void update(void);
void delete_rect(void);
void statistics(void);
int cmp_area(const void *a, const void *b);
void sorted(void);
void filter(void);

int main(void)
{
    int choice, n;
    n = (int)ask("Number of rectangles (1-1000): ");
    if (n < 1 || n > 1000) { printf("Invalid count.\n"); return 1; }
    list = malloc((size_t)n * sizeof(Rect));
    if (list == NULL) { printf("Out of memory.\n"); return 1; }
    capacity = n;
    while (1) {
        printf("\n1.Add 2.Display 3.Search 4.Update 5.Delete 6.Stats 7.Sorted 8.Filter 0.Exit\n");
        choice = (int)ask("Choice: ");
        switch (choice) {
        case 1: add_rect(); break;
        case 2: display(); break;
        case 3: search(); break;
        case 4: update(); break;
        case 5: delete_rect(); break;
        case 6: statistics(); break;
        case 7: sorted(); break;
        case 8: filter(); break;
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

int find_rect(int id)
{
    int i;
    for (i = 0; i < count; i++) {
        if (list[i].id == id) { return i; }
    }
    return -1;
}

void refresh(Rect *r)
{
    r->area = r->w * r->h;
    r->perimeter = 2 * (r->w + r->h);
}

void show(const Rect *r)
{
    double diff = fabs(r->w - r->h);
    double big = r->w > r->h ? r->w : r->h;
    if (big < 1.0) { big = 1.0; }
    printf("  ID %d  %.2f x %.2f  area %.2f  perimeter %.2f  %s\n",
           r->id, r->w, r->h, r->area, r->perimeter,
           diff <= TOL * big ? "square" : "not square");
}

void add_rect(void)
{
    Rect r, *tmp;
    r.id = (int)ask("ID: ");
    r.w = ask("Width: ");
    r.h = ask("Height: ");
    if (r.id < 1 || !isfinite(r.w) || !isfinite(r.h) || r.w <= 0 || r.h <= 0) {
        printf("Invalid ID or dimensions.\n");
        return;
    }
    if (find_rect(r.id) >= 0) { printf("ID %d exists.\n", r.id); return; }
    if (count == capacity) {
        tmp = realloc(list, (capacity + 1) * sizeof(Rect));
        if (tmp == NULL) { printf("Out of memory.\n"); return; }
        list = tmp;
        capacity++;
    }
    refresh(&r);
    list[count++] = r;
    printf("Rectangle %d added.\n", r.id);
}

void display(void)
{
    int i;
    if (count == 0) { printf("Inventory is empty.\n"); return; }
    for (i = 0; i < count; i++) { show(&list[i]); }
}

void search(void)
{
    int i = find_rect((int)ask("ID to search: "));
    if (i < 0) { printf("Not found.\n"); return; }
    show(&list[i]);
}

void update(void)
{
    int i = find_rect((int)ask("ID to update: "));
    double w, h;
    if (i < 0) { printf("Not found.\n"); return; }
    w = ask("New width: ");
    h = ask("New height: ");
    if (!isfinite(w) || !isfinite(h) || w <= 0 || h <= 0) {
        printf("Invalid dimensions.\n");
        return;
    }
    list[i].w = w;
    list[i].h = h;
    refresh(&list[i]);
    printf("Rectangle %d updated.\n", list[i].id);
}

void delete_rect(void)
{
    int i = find_rect((int)ask("ID to delete: "));
    int id, j;
    if (i < 0) { printf("Not found.\n"); return; }
    id = list[i].id;
    for (j = i; j < count - 1; j++) { list[j] = list[j + 1]; }
    count--;
    printf("Rectangle %d deleted.\n", id);
}

void statistics(void)
{
    int i, big = 0, small = 0;
    double total = 0.0;
    if (count == 0) { printf("Inventory is empty.\n"); return; }
    for (i = 0; i < count; i++) {
        total += list[i].area;
        if (list[i].area > list[big].area) { big = i; }
        if (list[i].area < list[small].area) { small = i; }
    }
    printf("Largest: "); show(&list[big]);
    printf("Smallest: "); show(&list[small]);
    printf("Total area %.2f, average area %.2f\n", total, total / count);
}

int cmp_area(const void *a, const void *b)
{
    const Rect *x = (const Rect *)a;
    const Rect *y = (const Rect *)b;
    if (x->area < y->area) { return -1; }
    if (x->area > y->area) { return 1; }
    return x->id - y->id;
}

void sorted(void)
{
    Rect *copy;
    int i;
    if (count == 0) { printf("Inventory is empty.\n"); return; }
    copy = malloc(count * sizeof(Rect));
    if (copy == NULL) { printf("Out of memory.\n"); return; }
    memcpy(copy, list, count * sizeof(Rect));
    qsort(copy, count, sizeof(Rect), cmp_area);
    for (i = 0; i < count; i++) { show(&copy[i]); }
    free(copy);
}

void filter(void)
{
    double limit = ask("Area greater than: ");
    int i, shown = 0;
    for (i = 0; i < count; i++) {
        if (list[i].area > limit) { show(&list[i]); shown++; }
    }
    if (shown == 0) { printf("Nothing above that area.\n"); }
}
