
#ifndef EXPECT_MATCH_H_
#define EXPECT_MATCH_H_

#include <stdint.h>
#include <stdbool.h>

struct expect_match;
typedef struct expect_match expect_match_t;

expect_match_t * expect_matcher_create(uint8_t *buf, uint8_t size);
void expect_matcher_destroy(expect_match_t *matcher);

void expect_matcher_update(expect_match_t *matcher, uint8_t c);
bool is_expect_matcher_bingo(expect_match_t *matcher);

#endif /* EXPECT_MATCH_H_ */
