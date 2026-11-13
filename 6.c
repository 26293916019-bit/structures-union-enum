/* Unions : union vs structure memory layout, and a tagged record that prints
   only the active member.  Build: gcc -std=c11 -Wall -Wextra 6.c -o 6
   Sizes are printed with %lu so the output is the same on old and new systems. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/* union and structure with the same three members */
typedef union { int i; float f; char str[20]; } UnionData;
typedef struct { int i; float f; char str[20]; } StructData;
/* enum to track which member of the union is currently valid */
typedef enum { VALUE_INT, VALUE_FLOAT, VALUE_TEXT } ValueType;
typedef struct { UnionData value; ValueType tag; } Record;
/* function declarations */
void print_record(const Record *r);
int set_int(Record *r, int v);
int set_float(Record *r, float v);
int set_text(Record *r, const char *text);
int main(void)
{
    StructData s;
    UnionData u;
    Record rec[5];
    int i;
    printf("sizeof(StructData) = %lu, sizeof(UnionData) = %lu\n",
           (unsigned long)sizeof s, (unsigned long)sizeof u);
    printf("alignof(StructData) = %lu, alignof(UnionData) = %lu\n",
           (unsigned long)_Alignof(StructData), (unsigned long)_Alignof(UnionData));
    printf("struct offsets: i=%lu f=%lu str=%lu\n",
           (unsigned long)offsetof(StructData, i),
           (unsigned long)offsetof(StructData, f),
           (unsigned long)offsetof(StructData, str));
    /* memory addresses: the structure */
    printf("struct addresses: %p %p %p\n", (void *)&s.i, (void *)&s.f,
           (void *)&s.str);
    /* memory addresses: the union */
    printf("union addresses : %p %p %p\n", (void *)&u.i, (void *)&u.f,
           (void *)&u.str);
    printf("100 structs = %lu bytes, 100 unions = %lu bytes\n",
           (unsigned long)(100 * sizeof(StructData)),
           (unsigned long)(100 * sizeof(UnionData)));
    /* the same members in a union, assigned and overwritten one at a time */
    u.i = 10;
    printf("\nu.i = %d\n", u.i);
    u.f = 220.5f;
    printf("u.f = %.1f  (the int is overwritten)\n", (double)u.f);
    strcpy(u.str, "Union Programming");
    printf("u.str = %s\n", u.str);
    set_int(&rec[0], 10);
    set_float(&rec[1], 220.5f);
    if (!set_text(&rec[2], "Union Programming")) { printf("text too long\n"); }
    set_int(&rec[3], 42);
    if (!set_text(&rec[4], "1234567890123456789")) { printf("text too long\n"); }
    printf("\nFive tagged records:\n");
    for (i = 0; i < 5; i++) { print_record(&rec[i]); }
    printf("\nChanging record 1:\n");
    set_text(&rec[0], "now text");
    print_record(&rec[0]);
    set_float(&rec[0], 3.5f);
    print_record(&rec[0]);
    printf("\nA 20 character string is rejected:\n");
    if (!set_text(&rec[4], "12345678901234567890")) { printf("text too long\n"); }
    print_record(&rec[4]);
    printf("\nAn invalid tag:\n");
    rec[3].tag = (ValueType)99;
    print_record(&rec[3]);
    printf("\nsizeof(Record) = %lu\n", (unsigned long)sizeof(Record));

    printf("\nObservation:\n");
    printf("  union members share one address, so only one is valid at a time\n");
    printf("  and the tag records which.  struct fields have their own offsets\n");
    printf("  (0, 4, 8) because int and float align on 4-byte boundaries, so a\n");
    printf("  struct is larger but all its fields stay valid together.\n");
    return 0;
}

void print_record(const Record *r)
{
    switch (r->tag) {
    case VALUE_INT: printf("int    : %d\n", r->value.i); break;
    case VALUE_FLOAT: printf("float  : %.1f\n", (double)r->value.f); break;
    case VALUE_TEXT: printf("text   : %s\n", r->value.str); break;
    default: printf("no active value\n"); break;
    }
}

int set_int(Record *r, int v)
{
    r->value.i = v;
    r->tag = VALUE_INT;
    return 1;
}

int set_float(Record *r, float v)
{
    r->value.f = v;
    r->tag = VALUE_FLOAT;
    return 1;
}

int set_text(Record *r, const char *text)
{
    if (strlen(text) >= sizeof r->value.str) { return 0; }
    strcpy(r->value.str, text);
    r->tag = VALUE_TEXT;
    return 1;
}
