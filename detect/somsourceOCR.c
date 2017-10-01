/*
 * Author :Aleat2
 * Project:Ocr using neural networks
 * Date   :17/11/2016
 */

# include <assert.h>
# include <err.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include "som.h"
# include "toolstr.h"
# include "binarization.h"

#define MAX  40
#define NMAX   8



char text[2];
char grid[MAX][MAX];

char normal_grid[NMAX][NMAX] ;

int  lst  = 0;

int trainingmode = 1;


int  min_x = MAX;
int  min_y = MAX;
int  max_x = 0;
int  max_y = 0;
int  train1 = 0;

float min_draw_x = -8.0;
float min_draw_y = -8.0;
float max_draw_x =  8.0;
float max_draw_y =  8.0;

float width  = 16.0;
float height = 16.0; 

float leaning_rate = 0.1;
float num_chars    = 50;
float max_error    = 0.0000001;
float iterations   = 1000000;




void neuralInit(struct neural *neural, int in, int out, float lrate)
{
  neural->num_inputs = in;
  neural->num_outputs = out;
  neural->num_weights = in * out;
  neural->leaning_rate = lrate;
  neural->weights = (float **)malloc(sizeof(float *) * neural->num_outputs);
  neural->inputs  = (float  *)malloc(sizeof(float  ) * neural->num_inputs);
  neural->outputs = (float  *)malloc(sizeof(float  ) * neural->num_outputs);
  for(int i = 0; i < neural->num_outputs; i++)
    {
      neural->weights[i] = (float *)malloc(sizeof(float) * neural->num_inputs);
    }
  neural->map = (char *)malloc(neural->num_outputs);
  for(int i = 0; i < neural->num_outputs; i++)
    {
      neural->map[i] = '_';
    }
}
void init(struct neural *neural)
{
  for(int i = 0; i < neural->num_outputs; i++)
    {
      for(int j = 0; j < neural->num_inputs; j++)
	{
	  neural->weights[i][j] = (float)rand() / (float)RAND_MAX - 0.5;
	}
    }
}

void print_map(struct neural *neural)
{
  for(int i = 0; i < neural->num_outputs; i++)
    {
      printf("%c ",neural->map[i]);
    }
}

char* get_map(struct neural *neural)
{
  return neural->map;
}
int get_num_weights(struct neural *neural)
{
  return neural->num_weights;
}

float** get_weights(struct neural *neural)
{
  return neural->weights;
}

int learn(struct neural *neural, float *in, char ch)
{
  for(int j = 0; j < neural->num_inputs; j++)
    {
      neural->inputs[j] = ((in[j] > 0) ? 1 : -1);
    }
  float max = -10000;
  int index = 0;  /* MODIF PEUT ETRE A ANNULER */
  for(int i = 0; i < neural->num_outputs; i++)
    {
      neural->outputs[i] = 0;
      for(int j = 0; j < neural->num_inputs; j++){
	neural->outputs[i] += neural->weights[i][j] * neural->inputs[j]  / neural->num_inputs; 
      }
      neural->outputs[i] /= neural->num_inputs;
      if(neural->outputs[i] > max){
	index = i;
	max = neural->outputs[i];
      }
      if(neural->outputs[i] < 0){
	neural->outputs[i] = 0;
      }else if(neural->outputs[i] > 1){
	neural->outputs[i] = 1;
      }
    }
  while(neural->map[index] != '_') index = (index + 1) % neural->num_outputs; 	
  printf("%d ",index);
  neural->map[index] = ch;
  return index;
}

char find(struct neural *neural, float *in)
{
 for(int j=0;j<neural->num_inputs;j++)
  neural->inputs[j] =((in[j] > 0) ? 1 : -1);
 float max = -10000;
 int index=0;
 for(int i=0;i<neural->num_outputs;i++)
 {
  neural->outputs[i] = 0;
  for(int j=0;j<neural->num_inputs;j++)
   neural->outputs[i]+=(neural->weights[i][j]*neural->inputs[j])/(neural->num_inputs);
  if(neural->outputs[i]> max)
  {
    index=i;
    max=neural->outputs[i]; 
  }
  if(neural->outputs[i]<0)
   neural->outputs[i]=0;
  if(neural->outputs[i]>1)
   neural->outputs[i]=1;
 }
 // printf("%d\n",index);  /* surement enlever le + 1 */
 // neural->map[index] = ch; 
 return neural->map[index];
}

float train(struct neural *neural, int index)
{
  float be = 0;
  for(int i = 0; i < neural->num_inputs; i++)
    {
      float e = neural->inputs[i] - neural->weights[index][i];
      if(e > be)
	{
	  be = e;
	}
      neural->weights[index][i] += neural->leaning_rate * e;
    }
  return be;
}
void record(struct neural *neural)
{
  FILE *fp = fopen("weights.data", "w");
  for(int i = 0; i < neural->num_outputs; i++)
    {
      for(int j = 0; j < neural->num_inputs; j++)
	{	
	  fprintf(fp, "%f \n", neural->weights[i][j]);
	}
    }
  for(int i = 0; i < neural->num_outputs; i++)
    {
      fprintf(fp, "%c \n", neural->map[i]);
    }
  fclose(fp);
}
int load(struct neural *neural)
{
  FILE *fp = fopen("weights.data", "r");
  if(fp == NULL)
    {
      return 0;
    }
  for(int i = 0; i < neural->num_outputs; i++)
    {
      for(int j = 0; j < neural->num_inputs; j++)
	{	
	  float y = fscanf(fp, "%f \n", &neural->weights[i][j]); /* PEUT ETRE A MODIFIER */
	  y ++; /**/
	}
    }
  for(int i = 0; i < neural->num_outputs; i++)
    {
     char w = fscanf(fp, "%c \n", &neural->map[i]); /* PEUT ETRE A MODIFIER */
     w = 'a';                                       /* PEUT ETRE A MODIFIER */
     if (w == 'z')                                  /* PEUT ETRE A MODIFIER */
       {
	 w = 'O';                                   /* PEUT ETRE A MODIFIER */
       }
    }
  fclose(fp);
  return 1;
}
float convert(float input)
{
  return ( 1.0 / ( 1.0 + (float)exp(-input)));
}

/******************************************************DETECTION*****************************************************************/


void draw_normal_grid()
{
  for(int i = 0; i < NMAX; i++){
    for(int j = 0; j < NMAX; j++){
      printf(" %c",normal_grid[i][j]);
    }
  }
}

void clear_normal_grid(){
  for(int i = 0; i < NMAX; i++){
    for(int j = 0; j < NMAX; j++){
      normal_grid[i][j] = '0';
    }
  }
}

void set_draw_range(){
  min_x = MAX;
  min_y = MAX;
  max_x = 0;
  max_y = 0;
  for(int i = 0; i < MAX; i++){
    for(int j = 0; j < MAX; j++){
      if(grid[i][j] == '1'){ /**/
	if(i <= min_y){
	  min_y = i;
	}
	if(i >= max_y){
	  max_y = i;
	}
	if(j <= min_x){
	  min_x = j;
	}
	if(j >= max_x){
	  max_x = j;
	}
      }
    }
  }
  if(max_x - min_x < NMAX){
    max_x += 4;
    min_x -= 4;
  }
  if(max_y - min_y < NMAX){
    max_y += 4;
    min_y -= 4;
  }
}

void normalize(){
  clear_normal_grid();
  set_draw_range();
	
  int xb = (max_x - min_x) / 8; 
  int yb = (max_y - min_y) / 8; 

  for(int i = min_y; i < max_y; i++){
    for(int j = min_x; j < max_x; j++){
      int y = (i - min_y) / yb;
      int x = (j - min_x) / xb;
      if(x > NMAX - 1) x = NMAX - 1;
      if(y > NMAX - 1) y = NMAX - 1;
      if(normal_grid[i][j] == '1') normal_grid[y][x] = '1';
    }
  }
  draw_normal_grid();
}

void set_normal_grid (float *in)
{
  int n = 0;
  for(int i = 0; i < NMAX; i++)
    {
      for(int j = 0; j < NMAX; j++)
	{
	  if(normal_grid[i][j] == '1')
	    in[n] = 1.0;
	  else
	    in[n] = 0.0;
	  n++;
	}
    }
  
}


/********train1**********/

void tr(struct neural *neural, char c){
  float in[NMAX * NMAX];
  set_normal_grid(in);
  int index = learn(neural,in, c);
  int b = 0;
  int i;
  float error;
  for(i = 0; i < iterations && (b == 0); i++)
    {	
      error = train(neural,index);
      if(error < max_error) 
	{
	  b = 1;
	}
    }
}


/********find1**********/

char fi(struct neural *neural)
{
  // draw_normal_grid();
  float in[NMAX * NMAX];
  set_normal_grid(in);
  char ch[2];
  ch[0] = find(neural,in);
  ch[1] = '\0';
  printf("%c", find(neural,in));
  // cout << s -> find(in) << endl;
  // edittext -> set_text(ch);
}
void callback(struct neural *neural){
  
  //normalize();
  if(trainingmode){
      char b;
      b = scanf("%c",&b);
      tr(neural,b);
      char ch[2];
      ch[0] = b;
      ch[1] = '\0';    
    }else{
      fi(neural);             
    }
  }


void lo(struct neural *neural){
	if(!load(neural)){
	  printf("error");
		return;
	}
	printf("loaded\n");
	char *map = get_map(neural);;
	for(int i = 0; i < num_chars; i++){
		if(map[i] != '_'){
			char ch[2];
			ch[0] = map[i];
			ch[1] = '\0';
		}
	}
}

void rec(struct neural *neural){
	record(neural);
	//edittext -> set_text("saved");
}

void translate (char grid1[][NMAX][NMAX],char grid[NMAX][NMAX],int n)
{
  // rtn = n;
  for (int i = 0; i< NMAX ; i++)
    {
      for (int j = 0; j < NMAX ; j ++)
	{
	  grid[i][j] = grid1[n][NMAX][NMAX];
	}
    }
}


/**********************************************************    MAIN    *****************************************************************/


int main (void)
{

  
  SDL_Surface *image;
  image = NULL;
  
  t_binary_image *pic;
  pic = NULL;
  image = SDL_LoadBMP("Lorem-Droid.bmp");
  //image = SDL_LoadBMP("incruster-image-texte-02.bmp");
  // image = SDL_LoadBMP("happy.bmp");
  //image = SDL_LoadBMP("testblock.bmp");
  transform(image);
  pic = bitmap_to_binaryimage(image);


  
  
  int res[] = {0,0,0,0};
   
  
  blocDetect(pic,  res);
  int yres = heightvf (pic, res);
  int numberBlock = nBlock(pic, yres, res);
  int *ressb= malloc(numberBlock * 4 * sizeof(int)); 
  underblock( pic , res , yres, ressb);
  
  //  for (int i = 0; i<(numberBlock * 4); i+=4)
  //  {
  //  int tab[4] = {ressb[i], ressb[i+1], ressb[i+2], ressb[i+3]};
  //  for (int i = 0; i<4; i++)
  //  traceBlock(image, tab);
  //   }
  
  
  int nL =  nbLines(pic, ressb, numberBlock);
  int *ressl= malloc(nL * 4 * sizeof(int));
  Lines (pic,  numberBlock, ressb, ressl, nL);
  
    
  //  for (int i = 0; i<(nL * 4); i+=4)
  //  {
  // int tab[4] = {ressl[i], ressl[i+1], ressl[i+2], ressl[i+3]};
  //for (int i = 0; i<4; i++)
  //traceBlock(image, tab);
  //}
  


  int nC =  nbChar (pic, ressl, nL);
  int *resC= malloc(nC * 4 * sizeof(int));
  detectChar (pic, ressl, nL, resC);

  int ze = 0+4;
  // int nlargeur = resC[6]-resC[4];
  // int nlongueur = resC[7] - resC[5];
  char expectletter[MAX][MAX];
  int i = resC[ze+1];
  int j = resC[ze];
  int i1 = 0;
  int j1 = 0;
  for(i = resC[ze+1]; i< resC[ze+3]; i++)
    {
      j1 = 0;
      for(j = resC[ze]; j < resC[ze+2];j++)
	{
	  
	  if ( pic->data[j][i] == 1 && j < resC[ze + 2] && i<resC[ze + 3])
	    {
	      grid[j1][i1] = '1';
	    }
	  else
	    {
	      grid[j1][i1] = '0';
	    }
	  printf("%c",grid[j1][i1]);
	  j1++;
	}
      i1++;
      printf("\n");
    }
  normalize();
  
  printf("\n");
  
  int i001 = 0;
  int y001 = 0;
  for(int i00 = 0 ; i00 < NMAX;i00++)
    {
      for(int y00 = 0; y00 < NMAX ; y00++)
	{
	  printf("%c", normal_grid[y00][i00]);
	}
      printf("\n");
    }
  
  for (int i = 0; i<(nC * 4); i+=4)
    {
      int tab[4] = {resC[i], resC[i+1], resC[i+2], resC[i+3]};
      for (int i = 0; i<4; i++)
	traceBlock(image, tab);
    }
  
  

 
  struct neural *neurone = malloc(sizeof(struct neural));
  neuralInit(neurone,NMAX*NMAX,num_chars,leaning_rate);
  init(neurone);
  lo(neurone);
  callback(neurone);
  rec(neurone); 
  
  // init_sdl();
  // display_image(image);
  free(ressb);
  free(ressl);
  free(resC);
  // free_pic(pic);
 
}





