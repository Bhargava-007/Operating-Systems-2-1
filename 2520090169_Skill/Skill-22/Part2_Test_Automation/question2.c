#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int passed = 0, failed = 0;

void test(char *name, int result) {
    if (result) { printf("[PASS] %s\n", name); passed++; }
    else        { printf("[FAIL] %s\n", name); failed++; }
}

// modules to test
int add(int a, int b)       { return a + b; }
int is_empty(char *s)       { return s == NULL || s[0] == '\0'; }
char *trim(char *s)         { while (*s == ' ') s++; return s; }
int valid_cmd(char *s)      { return s && strlen(s) > 0 && s[0] != ' '; }

int main() {
    printf("=== Test Automation ===\n\n");

    printf("-- Math Tests --\n");
    test("add(2,3)==5",    add(2,3) == 5);
    test("add(0,0)==0",    add(0,0) == 0);
    test("add(-1,1)==0",   add(-1,1) == 0);

    printf("\n-- String Tests --\n");
    test("is_empty('')",   is_empty(""));
    test("is_empty(NULL)", is_empty(NULL));
    test("!is_empty(hi)",  !is_empty("hi"));
    test("trim spaces",    strcmp(trim("  hello"), "hello") == 0);

    printf("\n-- Command Tests --\n");
    test("valid: ls",      valid_cmd("ls"));
    test("invalid: empty", !valid_cmd(""));
    test("invalid: NULL",  !valid_cmd(NULL));

    printf("\n--- Report ---\n");
    printf("Passed: %d  Failed: %d  Total: %d\n", passed, failed, passed+failed);
    return failed > 0 ? 1 : 0;
}
