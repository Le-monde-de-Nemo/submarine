#ifndef _VEC2_H_
#define _VEC2_H_
struct vec2 {
    int x;
    int y;
};

/*
 *  Returns the vector (x, y)
 */
struct vec2 vec2__create(int x, int y);

/*
 *  Returns the vector (0, 0)
 */
struct vec2 vec2__zeros();

/*
 *  Returns the vector (1, 1)
 */
struct vec2 vec2__ones();

/*
 *  Add v to u
 *  Returns u + v
 */
struct vec2 vec2__add(struct vec2 u, struct vec2 v);

/*
 *  Substract v to u
 *  Returns u - v
 */
struct vec2 vec2__sub(struct vec2 u, struct vec2 v);

/*
 *  Multiply u by r
 *  Returns (r * x, r * y)
 */
struct vec2 vec2__rmul(int r, struct vec2 u);

/*
 *  Write at most n chars to dst to display vec
 *  Returns dst
 */
char* vec2__disp(struct vec2 u, char* dst, long n);
#endif
