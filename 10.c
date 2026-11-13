/* Structures, unions and enums : a typed container that holds one value of any
   type, with a tag, safe getters and a deep copy.  Build: gcc -std=c11 -Wall -Wextra 10.c -o 10 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* enum to track the type of data stored */
typedef enum { TYPE_EMPTY, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING } DataType;
/* union to store different types of data */
typedef union { int i; float f; char c; char *s; } Data;
/* struct to encapsulate the data and its type */
typedef struct { Data data; DataType type; } Container;

/* function declarations */
void init(Container *c);
void clear(Container *c);
int set_int(Container *c, int v);
int set_float(Container *c, float v);
int set_char(Container *c, char v);
int set_string(Container *c, const char *text);
int get_int(const Container *c, int *out);
int get_float(const Container *c, float *out);
int get_char(const Container *c, char *out);
const char *get_string(const Container *c);
const char *type_name(DataType t);
void print(const Container *c);
int copy(Container *dest, const Container *src);

int main(void)
{
    Container box[6], dup, empty;
    int i, number;
    float real;
    char letter;

    for (i = 0; i < 6; i++) { init(&box[i]); }
    set_int(&box[0], 10);
    set_float(&box[1], 3.14f);
    set_char(&box[2], 'A');
    set_string(&box[3], "Hello");
    set_int(&box[4], 0);
    set_string(&box[5], "Data");
    printf("Six containers:\n");
    for (i = 0; i < 6; i++) {
        printf("  [%d] %-7s = ", i, type_name(box[i].type));
        print(&box[i]);
        printf("\n");
    }

    init(&dup);
    copy(&dup, &box[5]);
    set_string(&box[5], "Changed original");
    printf("\nAfter changing the original:\n  original: ");
    print(&box[5]);
    printf("\n  copy    : ");
    print(&dup);
    clear(&dup);
    clear(&dup);
    printf("\nCopy cleared twice: ");
    print(&dup);

    printf("\nType changes on container 0:\n");
    set_string(&box[0], "first text");
    print(&box[0]);
    set_int(&box[0], 77);
    printf("\n"); print(&box[0]);
    set_string(&box[0], "second text");
    printf("\n"); print(&box[0]);
    set_float(&box[0], 2.5f);
    printf("\n"); print(&box[0]);

    printf("\nEdge cases:\n");
    printf("  zero is a valid int : %d, value %d\n", get_int(&box[4], &number), number);
    printf("  wrong type getter   : %d\n", get_int(&box[1], &number));
    init(&empty);
    printf("  getter on empty box : %d\n", get_int(&empty, &number));
    printf("  char getter on text : %d\n", get_char(&box[3], &letter));
    printf("  float getter on text: %d\n", get_float(&box[3], &real));
    printf("  empty string        : %d ", set_string(&box[2], ""));
    print(&box[2]);
    printf("\n  self copy           : %d\n", copy(&box[5], &box[5]));
    printf("  string getter       : %s\n", get_string(&box[5]));

    for (i = 0; i < 6; i++) { clear(&box[i]); }
    printf("\nAll containers cleared.\n");
    return 0;
}

void init(Container *c)
{
    if (c == NULL) { return; }
    c->data.s = NULL;
    c->type = TYPE_EMPTY;
}

void clear(Container *c)
{
    if (c == NULL) { return; }
    /* free any allocated memory */
    if (c->type == TYPE_STRING) { free(c->data.s); }
    c->data.s = NULL;
    c->type = TYPE_EMPTY;
}

int set_int(Container *c, int v)
{
    if (c == NULL) { return 0; }
    clear(c);
    c->data.i = v;
    c->type = TYPE_INT;
    return 1;
}

int set_float(Container *c, float v)
{
    if (c == NULL) { return 0; }
    clear(c);
    c->data.f = v;
    c->type = TYPE_FLOAT;
    return 1;
}

int set_char(Container *c, char v)
{
    if (c == NULL) { return 0; }
    clear(c);
    c->data.c = v;
    c->type = TYPE_CHAR;
    return 1;
}

int set_string(Container *c, const char *text)
{
    if (c == NULL) { return 0; }
    char *copy_text;
    size_t len = strlen(text) + 1;
    /* free previous string if any, but only after the new copy exists */
    copy_text = malloc(len);
    if (copy_text == NULL) { return 0; }
    memcpy(copy_text, text, len);
    clear(c);
    c->data.s = copy_text;
    c->type = TYPE_STRING;
    return 1;
}

int get_int(const Container *c, int *out)
{
    if (c == NULL) { return 0; }
    if (c->type == TYPE_EMPTY) { return -1; }
    if (c->type != TYPE_INT) { return 0; }
    *out = c->data.i;
    return 1;
}

int get_float(const Container *c, float *out)
{
    if (c == NULL) { return 0; }
    if (c->type == TYPE_EMPTY) { return -1; }
    if (c->type != TYPE_FLOAT) { return 0; }
    *out = c->data.f;
    return 1;
}

int get_char(const Container *c, char *out)
{
    if (c == NULL) { return 0; }
    if (c->type == TYPE_EMPTY) { return -1; }
    if (c->type != TYPE_CHAR) { return 0; }
    *out = c->data.c;
    return 1;
}

const char *get_string(const Container *c)
{
    if (c == NULL) { return NULL; }
    if (c->type != TYPE_STRING) { return NULL; }
    return c->data.s;
}

const char *type_name(DataType t)
{
    switch (t) {
    case TYPE_INT: return "int";
    case TYPE_FLOAT: return "float";
    case TYPE_CHAR: return "char";
    case TYPE_STRING: return "string";
    default: return "empty";
    }
}

void print(const Container *c)
{
    if (c == NULL) { return; }
    switch (c->type) {
    case TYPE_INT: printf("%d", c->data.i); break;
    case TYPE_FLOAT: printf("%g", (double)c->data.f); break;
    case TYPE_CHAR: printf("'%c'", c->data.c); break;
    case TYPE_STRING: printf("\"%s\"", c->data.s); break;
    default: printf("empty"); break;
    }
}

int copy(Container *dest, const Container *src)
{
    if (dest == NULL || src == NULL) { return 0; }
    if (dest == src) { return 1; }
    if (src->type == TYPE_STRING) { return set_string(dest, src->data.s); }
    clear(dest);
    dest->data = src->data;
    dest->type = src->type;
    return 1;
}
