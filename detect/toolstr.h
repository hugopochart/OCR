
# include <stdlib.h>
# include <SDL.h>

/*
struct Point
{
  int x, y;
};

struct Block
{
  struct Point origin;
  int width, height;
};


struct Symbol
{
  struct Point origin;
  int width, height;
};

*/
struct s_binary_image
{
   char *name;
  int width;
  int height;
  int **data;  /* Matrix of binary integer */
  int *hproj;  /* Horizontal projection vector */
};

typedef struct s_binary_image t_binary_image;



