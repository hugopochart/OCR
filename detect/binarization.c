#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <math.h>
#include <err.h>
#include "toolstr.h"

static inline
Uint8* pixelref(SDL_Surface *surf, unsigned x, unsigned y) {
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

void wait_for_keypressed(void) {
  SDL_Event             event;
  // Infinite loop, waiting for event
  for (;;) {
    // Take an event
    SDL_PollEvent( &event );
    // Switch on event type
    switch (event.type) {
      // Someone pressed a key -> leave the function
    case SDL_KEYDOWN: return;
    default: break;
    }
    // Loop until we got the expected event
  }
}

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4:
    return *(Uint32 *)p;
  }
  return 0;
}

void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    *p = pixel;
    break;
  case 2:
    *(Uint16 *)p = pixel;
    break;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
}


/*Transform image to Bitmap */
t_binary_image *bitmap_to_binaryimage(SDL_Surface *image)
{
  int i;
  int j;
  Uint32 black;
  t_binary_image *pic;

  black = SDL_MapRGB(image->format,0x00,0x00,0x00);
  pic = NULL;
  pic = malloc(sizeof(t_binary_image));
  pic->name = "OCR";
  pic->width = image->w;
  pic->height = image->h;
  pic->data = malloc(pic->height * pic->width * sizeof(int));
  for (i = 0; i< pic->width;i++ )
    {
      pic->data[i] = malloc(pic->width * pic->height * sizeof(int));
    }
  SDL_LockSurface(image);
  for(i = 0; i < image->h; i++)
    {
      for(j = 0; j < image->w;j++)
	{
	  if (getpixel(image,j,i) == black)
	    {
	      pic->data[j][i] = 1;
	    }
	  else
	    {
	      pic->data[j][i] = 0;
	    }
	}
    }
  pic->hproj = NULL;
  SDL_UnlockSurface(image);
  return(pic);
}
void init_sdl(void) {
  // Init only the video part
  if( SDL_Init(SDL_INIT_VIDEO)==-1 ) {
    // If it fails, die with an error message
    errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
  }
  // We don't really need a function for that ...
}

SDL_Surface* display_image(SDL_Surface *img) {
  SDL_Surface          *screen;
  // Set the window to the same size as the image
  screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
  if ( screen == NULL ) {
    // error management
    errx(1, "Couldn't set %dx%d video mode: %s\n",
         img->w, img->h, SDL_GetError());
  }
 
  /* Blit onto the screen surface */
  if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
    warnx("BlitSurface error: %s\n", SDL_GetError());
 
  // Update the screen
  SDL_UpdateRect(screen, 0, 0, img->w, img->h);
 
  // wait for a key
  wait_for_keypressed();
 
  // return the screen for further uses
  return screen;
}
/*Transform image in Black and White */
void transform(SDL_Surface *img)
{
  for (int i = 0; i < img->w; ++i)
    for (int j = 0; j < img->h; ++j)
      {
	Uint32 pix = getpixel(img, i, j);
	Uint8 r = 0;
	Uint8 g = 0;
	Uint8 b = 0;
	SDL_GetRGB(pix, img->format, &r, &g, &b);
	float avg = r * 0.3 + g * 0.59 + b * 0.11 / 3;
	if(avg > 100){ /**********************************************  vary level of grey **************************************************************************************/
	  avg = 255;
	}
	else{
	  avg = 0;
	}
	pix = SDL_MapRGB(img->format, avg, avg, avg);
	putpixel(img, i, j, pix);
      }
}
void traceHorizontal (SDL_Surface *img,  int xbegin, int xend, int y)
{
  int i;
  Uint32 BluePix = SDL_MapRGB(img->format, 0,0,255);
  for (i = xbegin; i<= xend; i++)
    {
      putpixel( img, i, y, BluePix);
    }
}

void traceVertical(SDL_Surface *img,  int ybegin, int yend, int x)
{
  int i;
  Uint32 BluePix = SDL_MapRGB(img->format, 0,0,255);
  for (i = ybegin; i<= yend; i++)
    {
      putpixel( img, x, i, BluePix);
    }
}

void traceBlock (SDL_Surface *img,int res[4])
{
  traceHorizontal(img,res[0],res[2],res[1]);
  traceHorizontal(img,res[0],res[2],res[3]);
  traceVertical(img,res[1],res[3],res[0]);
  traceVertical(img,res[1],res[3],res[2]);
  
}

int heightvf (t_binary_image *pic, int res[4]){
  int yres = res[3] - res[1];
  int yp = 0;
  int e = 0;
  int y;
  for (y =  res[1] ; y < res[3]; y++){
    int xp = 0;
    for (int x = res[0]; x < res[2]; x++){
      if (pic->data[x][y] == 1){
	xp += 1;
      }
    }
    if(xp == 0){
      yp += 1;
      e = 1;
    }
    else {
      if ( yp < yres && e){
	yres = yp;
      }
      yp = 0;
      e = 0;
    }
  }
  return yres;
}
	  
	
      

int nBlock(t_binary_image *pic, int yres, int res[4])
{
  int nbBlock = 1;
  int yp = 0;
  int y;
  for (y = res[1]; y < res[3]; y++){
    int xp = 0;
    for (int x = res[0]; x < res[2]; x++){
      if (pic->data[x][y] == 1){
	xp += 1;
      }
    }
    if(xp == 0){
      yp += 1;
	
    }
    else {
      if ( yp > yres+8){ /* vary +8  */
	nbBlock++;
      }
      yp = 0;
	
    }
  }
  return nbBlock;
}

void blocDetect(t_binary_image *pic, int res[4]){ 
  res[0] = pic->width;
  res[1] = pic->height;
  for(int y = 0; y< pic->height ; y++){
    
    int xpd = 0; 
    int xpf = 0; 
    int ypd = 0;  
    int ypf = 0; 
    int x = 0; 
    int e = 1 ;
    while( x< pic->width && e){ 
                         
      if (pic->data[x][y] == 1){ 
	xpd = x; 
	ypd = y; 
	e = 0; 
      } 
      x++ ;
    } 
    x = pic->width-1; 
    e = 1; 
    while (x >= 0 && e){ 
      if (pic->data[x][y] == 1){ 
	xpf = x; 
	ypf = y; 
	e = 0;
	
      }
      x--;
    } 
    if(ypd != 0){ 
      if(ypd < res[1]){ 
	res[1] = ypd; 
      } 
    } 
    if(ypf!= 0){ 
      if(ypf > res[3]){ 
	res[3] = ypf; 
      } 
    } 
    if(xpd!= 0){ 
      if(xpd < res[0]){ 
	res[0] = xpd; 
      } 
    } 
    if(xpf!= 0){ 
      if(xpf > res[2]){ 
	res[2] = xpf; 
      } 
    } 
  } 
}

void underblock( t_binary_image *pic , int res[] , int el, int ressb[]){
  ressb[0]= res[0];
  ressb[1] = res[1];
  int e = 1;
  int i = 0;
  int nblinewhite = 0;
  for (int y = res[1]; y < res[3]; y++){
    int xp = 0;
    for(int x = res[0]; x < res[2]; x++){
      if(pic->data[x][y] == 1){
	xp += 1;
      }
    }

    if(xp > 0) {
      nblinewhite = 0;
    }
    else{
      nblinewhite += 1;
    }

    if((nblinewhite > el+6) &&( e == 1)){
      ressb[i*4+3] = y - el -6;
      ressb[i*4+2] = res[2];
      e = 0;
      i++;
    }       
    else if(e == 0){
      if(xp > 0){
	ressb[i*4] = res[0];
	ressb[i*4 +1] = y;
	e = 1;
      }
    }
  }
  ressb[i*4+2]=res[2];
  ressb[i*4+3]=res[3];
}

int nbLines_rec( t_binary_image *pic, int *ressb, int i)
{
  int nbLine = 1;
  int e = 0;
  for (int y=ressb[i+1]; y<ressb[i+3]; y++)
    {
      int line = 0;
      for (int x=ressb[i+0]; x<ressb[i+2]; x++)
	{
	  if (pic->data[x][y]==1)
	    line++;
	}
      if (line!=0 && e==0)
	{
	  e=1;
	}
      if (line == 0 && e==1)
	{	      
	  nbLine++;
	  e=0;
	}
    }
  return nbLine;
}

int nbLines(t_binary_image *pic, int *ressb, int nBlock)
{
  int n = 0;
  for (int i = 0; i <  nBlock*4; i+=4)
    n+=nbLines_rec(pic, ressb, i);
  return n;
}

void Lines_rec (t_binary_image *pic, int xb, int yb, int xf, int yf,\
 int *ressl, int nL, int j)
{
  if (nL==1)
    {
      ressl[j] = xb;
      ressl[j+1] = yb;
      ressl[j+2] = xf;
      ressl[j+3] = yf;
      j+=4;
    }
  else
    {
      ressl[j] = xb;
      ressl[j+1] = yb;
      ressl[j+2] = xf;
      int e=0;
      for (int y = yb; y<yf; y++)
	{
	  int line =0;
	  for (int x = xb; x<xf; x++)
	    {
	      if (pic->data[x][y]==1)
		line++;
	    }
	  if (line!=0 && e==0)
	    {
	      e=1;
	      ressl[j] = xb;
	      ressl[j+1] = y;
	      ressl[j+2] = xf;
	    }
	  if (line == 0 && e==1)
	    {	      
	      ressl[j+3] = y-1;
	      e=0;
	      j+=4;
	    }	  
	}
      if (e)
	{
	  ressl[j+3] = yf;
	  e=0;
	  j+=4;
	}
    }
}

void Lines (t_binary_image *pic,  int nBlock,int *ressb,int *ressl,  int nL)
{  
  int q = nL;
  q++;
  int j=0;
  for (int i = 0; i < nBlock*4; i+=4)
    {
      int nL_SB = nbLines_rec( pic, ressb, i);
      Lines_rec(pic, ressb[i], ressb[i+1], ressb[i+2],ressb[i+3], ressl,\
 nL_SB, j);
      j+=nL_SB*4;
    }
}

int nbChar (t_binary_image *pic, int *ressl, int nbline){
  int res = 0;
  for(int i = 0; i < nbline; i++){
    int e = 1;
    int yp = 0;
    for(int y = ressl[i*4+1]; y < ressl[i*4+3]; y++){
      if( pic->data[0][y] == 1){
	yp += 1;
      }
    }
    if(yp > 0){
      e =0;
    }


    for(int x = ressl[i*4]; x<ressl[i*4+2];x++){
      int yp = 0;
      for(int y = ressl[i*4+1]; y < ressl[i*4+3]; y++){
	if( pic->data[x][y] == 1){
	  yp += 1;
	}
      }
      if (yp == 0 && e==0){
	e = 1;
      }
      if(yp > 0 && e==1){
	res++;
	e = 0;
      }
    }
  }
  return res;
}
void detectChar (t_binary_image *pic, int *ressl, int nbline, int *resC){
  int j = 0;
  for(int i = 0; i < nbline; i++){
    int e = 1;
    int yp=0;
    for(int y = ressl[i*4+1]; y < ressl[i*4+3]; y++){
      if( pic->data[0][y] == 1){
	yp += 1;
      }
    }
    if(yp > 0){
      e =0;
    }
    int x = ressl[i*4];
    for(; x<ressl[i*4+2];x++){
      int yp = 0;
      int y = ressl[i*4+1];
      for( ; y < ressl[i*4+3]; y++){
	if( pic->data[x][y] == 1){
	  yp += 1;
	}
      }
      if (yp == 0 && e==0){
	resC[j]=x ;
	j++;
	resC[j]= y +2 ;
	j++;
	e = 1;
      }
      if(yp > 0 && e==1){
	resC[j]=x;
	j++;
	resC[j] = ressl[i*4+1];
	j++;
	e = 0;
      }
    }
    if(!e){
       
      resC[j] = x;
      j++;
      resC[j] = ressl[i*4+3];
      j++;
       
    }
  }
}

void free_pic(t_binary_image *pic)
{
  int i;

  if (pic != NULL)
    {
      free(pic->hproj);
      pic->hproj = NULL;
      for (i=0; i < pic->height; i++)
	{
	  free(pic->data[i]);
	  pic->data[i] = NULL;
	}
      free(pic->data);
      pic->data= NULL;
      free(pic);
      pic = NULL;
    }
}
