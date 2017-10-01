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

#define MAX  120
#define NMAX   8



char text[2];
char grid[MAX][MAX];

char normal_grid[NMAX][NMAX] =        { "11111111",
				       "10000011",
				       "10111101",
				       "10111011",
				       "10000111",
				       "10111101",
				       "10000011",
				       "11111111" } ;

int  lst  = 0;

int trainingmode = 0;


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
      if(grid[i][j] == '1') normal_grid[y][x] = '1';
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
  /*
  char  normal2222_grid[2][8][8] = { {
      "11111111",
      "11000011",
      "10111101",
      "10000001",
      "10111101",
      "10111101",
      "10111101",
      "11111111" },

				     { "11111111",
				       "10000011",
				       "10111101",
				       "10111011",
				       "10000111",
				       "10111101",
				       "10000011",
				       "11111111" } };
  char expectedvalue[2] = {'A','B'};
  */
  struct neural *neurone = malloc(sizeof(struct neural));
  neuralInit(neurone,NMAX*NMAX,num_chars,leaning_rate);
  init(neurone);
  lo(neurone);
  callback(neurone);
  rec(neurone); 
}




