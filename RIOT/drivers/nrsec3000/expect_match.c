#include "expect_match.h"

#include <stdio.h>

/*TODO improve this simple expect_match*/
struct expect_match {
   bool flag;
   uint8_t *buf;
   uint8_t size;
   uint8_t state;
};

struct expect_match g_expect_match;
expect_match_t * expect_matcher_create(uint8_t *buf, uint8_t size)
{
   expect_match_t *matcher = &g_expect_match;
   matcher->buf = buf;
   matcher->size = size;

   return matcher;
}

void expect_matcher_destroy(expect_match_t *matcher)
{
   matcher->buf = NULL;
   matcher->size = 0;
   matcher->state = 0;
}

void expect_matcher_update(expect_match_t *matcher, uint8_t c)
{
   if (c == matcher->buf[0]) {
      matcher->state = 1;
      return;
   }
   if (matcher->size < 2) {
      return;
   }
   if (c != matcher->buf[1]) {
      matcher->state = 0;
      return;
   }
   if (c == matcher->buf[1] && matcher->state == 1) {
      matcher->state = 2;
      return;
   }
}
bool is_expect_matcher_bingo(expect_match_t *matcher)
{
   bool match = (matcher->state == matcher->size);
   return match;
}
