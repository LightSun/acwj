#include "utils.h"
#include "content.h"

void fatal(struct _Content* c,char *s) {
  fprintf(stderr, "%s on line %d\n", s, c->context.line); exit(1);
}

void fatals(struct _Content* c,char *s1, char *s2) {
  fprintf(stderr, "%s:%s on line %d\n", s1, s2, c->context.line); exit(1);
}

void fatald(struct _Content* c,char *s, int d) {
  fprintf(stderr, "%s:%d on line %d\n", s, d, c->context.line); exit(1);
}

void fatalc(struct _Content* c,char *s, int ch) {
  fprintf(stderr, "%s:%c on line %d\n", s, ch, c->context.line); exit(1);
}
