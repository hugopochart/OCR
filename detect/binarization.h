#include "SDL/SDL.h"


//t_binary_image *bitmap_to_binaryimage(SDL_Surface *image, char *filename);

Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y);

void wait_for_keypressed(void);

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

t_binary_image *bitmap_to_binaryimage(SDL_Surface *image);

void init_sdl(void);

SDL_Surface* display_image(SDL_Surface *img);

void transform(SDL_Surface *img);

void traceHorizontal (SDL_Surface *img,  int xbegin, int xend, int y);

void traceVertical(SDL_Surface *img,  int ybegin, int yend, int x);

void traceBlock (SDL_Surface *img,int res[4]);

int heightvf (t_binary_image *pic, int res[4]);

int nBlock(t_binary_image *pic, int yres, int res[4]);

void blocDetect(t_binary_image *pic, int res[4]);

void underblock( t_binary_image *pic , int res[] , int el, int ressb[]);

int nbLines_rec( t_binary_image *pic, int *ressb, int i);

int nbLines(t_binary_image *pic, int *ressb, int nBlock);

void Lines_rec (t_binary_image *pic, int xb, int yb, int xf, int yf,int *ressl, int nL, int j);

void Lines (t_binary_image *pic,  int nBlock,int *ressb,int *ressl,  int nL);

int nbChar (t_binary_image *pic, int *ressl, int nbline);

void detectChar (t_binary_image *pic, int *ressl, int nbline, int *resC);

void free_pic(t_binary_image *pic);

