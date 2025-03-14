#include "vec2.h"

/*
 *  Returns the vector (x, y)
 */
struct vec2 vec2__create(int x, int y)
{
  struct vec2 vec = {
    .x = x,
    .y = y
  };

  return vec;
}

/*
 *  Returns the vector (0, 0)
 */
struct vec2 vec2__zeros()
{
  struct vec2 zeros = {0, 0};

  return zeros;
}

/*
 *  Returns the vector (1, 1)
 */
struct vec2 vec2__ones()
{
  struct vec2 zeros = {1, 1};

  return zeros;
}

/*
 *  Add v to u
 *  Returns u + v
 */
struct vec2 vec2__add(struct vec2 u, struct vec2 v)
{
  struct vec2 added = {
    .x = u.x + v.x,
    .y = u.y + v.y
  };

  return added;
}

/*
 *  Substract v to u
 *  Returns u - v
 */
struct vec2 vec2__sub(struct vec2 u, struct vec2 v)
{
  struct vec2 sub = {
    .x = u.x - v.x,
    .y = u.y - v.y
  };

  return sub;
}

/*
 *  Multiply u by r
 *  Returns (r * x, r * y)
 */
struct vec2 vec2__rmul(int r, struct vec2 u)
{
  struct vec2 rmuld = {
    .x = r * u.x,
    .y = r * u.y
  };

  return rmuld;
}
