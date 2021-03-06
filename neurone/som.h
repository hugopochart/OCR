#include <stdlib.h>
#include <math.h>
#include <stdio.h>

struct node
{
 int num_inputs;
 float *weights;
 float *inputs;
 float *errors;
 float output;
};

struct layer
{        
 int num_nodes;
 struct node *chr;
};

struct neural
{
 int num_inputs;
 int num_outputs;
 int num_weights;
 float leaning_rate;
 float *inputs;
 float *outputs;
 float **weights;
 char *map; 
};

void neuralInit(struct neural *neural,int in, int out, float lrate);

void init(struct neural *neural);

void print_map(struct neural *neural);

char *get_map(struct neural *neural);

int get_num_weights(struct neural *neural);

float** get_weights(struct neural *neural);

int learn(struct neural *neural,float *in,char ch);

char find(struct neural *neural,float *in);

float train(struct neural *neural,int index);

void record(struct neural *neural);

int load(struct neural *neural);
     
float convert(float input);       

void draw_normal_grid();

void normalize();

void set_normal_grid (float *in);

void tr(struct neural *neural, char c);

char fi(struct neural *neural);

void callback(struct neural *neural);

void lo(struct neural *neural);

void rec(struct neural *neural);

