//
// Copyright (c) 2011-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/tetris
//

#define apply(type, func, ...) {                          \
        void* end_ptr = (int[]){0};                       \
        type** pointer = (type*[]){__VA_ARGS__, end_ptr}; \
        for (int i = 0; pointer[i] != end_ptr; ++i)       \
             func(pointer[i]);                            \
        }
#define delete_all(...) apply(void, free, __VA_ARGS__);

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <termios.h>

typedef char* string;

const int box0[2] = {4, 30};
const int box1[4] = {4, 30, 4, 32};
const int box2[4] = {4, 30, 5, 32};
const int box3[6] = {4, 28, 4, 30, 4, 32};
const int box4[6] = {4, 28, 4, 30, 5, 30};
const int box5[6] = {4, 28, 5, 30, 6, 32};
const int box6[6] = {4, 30, 5, 28, 5, 32};
const int box7[8] = {4, 28, 5, 30, 6, 32, 7, 34};
const int box8[8] = {4, 30, 5, 28, 5, 30, 5, 32};
const int box9[8] = {4, 30, 5, 28, 5, 32, 6, 30};
const int box10[8] = {4, 28, 4, 30, 4, 32, 4, 34};
const int box11[8] = {4, 28, 5, 28, 5, 30, 5, 32};
const int box12[8] = {4, 28, 4, 30, 5, 30, 5, 32};
const int box13[8] = {4, 28, 4, 30, 5, 28, 5, 30};
const int box14[8] = {4, 28, 4, 34, 5, 30, 5, 32};
const int box15[10] = {4, 26, 4, 28, 4, 30, 4, 32, 4, 34};
const int box16[10] = {4, 30, 5, 28, 5, 30, 5, 32, 6, 30};
const int box17[10] = {4, 28, 4, 32, 5, 30, 6, 28, 6, 32};
const int box18[10] = {4, 28, 4, 32, 5, 28, 5, 30, 5, 32};
const int box19[10] = {4, 28, 4, 30, 5, 30, 6, 30, 6, 32};
const int box20[10] = {4, 28, 5, 28, 6, 28, 6, 30, 6, 32};
const int box21[10] = {4, 28, 4, 30, 5, 30, 5, 32, 6, 32};
const int box22[10] = {4, 26, 4, 34, 5, 28, 5, 30, 5, 32};
const int box23[10] = {4, 26, 4, 34, 5, 28, 5, 32, 6, 30};
const int box24[10] = {4, 26, 5, 28, 6, 30, 7, 32, 8, 34};
const int box25[10] = {4, 28, 4, 32, 5, 26, 5, 30, 5, 34};
const int box26[16] = {4, 28, 4, 34, 5, 30, 5, 32, 6, 30, 6, 32, 7, 28, 7, 34};
const int box27[18] = {4, 30, 5, 28, 5, 32, 6, 26, 6, 30, 6, 34, 7, 28, 7, 32, 8, 30};
const int box28[26] = {4, 30, 5, 28, 5, 30, 5, 32, 6, 26, 6, 28, 6, 30, 6, 32, 6, 34, 7, 28, 7, 30, 7, 32, 8, 30};
const int box29[26] = {4, 30, 5, 30, 6, 28, 6, 30, 6, 32, 7, 26, 7, 30, 7, 34, 8, 30, 9, 30, 10, 30, 11, 30, 12, 30};
const int box30[34] = {4, 26, 4, 28, 4, 30, 4, 34, 5, 30, 5, 34, 6, 26, 6, 28, 6, 30, 6, 32, 6, 34, 7, 26, 7, 30, 8, 26, 8, 30, 8, 32, 8, 34};
const int box31[34] = {4, 30, 5, 28, 6, 26, 6, 34, 7, 28, 7, 32, 7, 36, 8, 22, 8, 30, 8, 38, 9, 24, 9, 28, 9, 32, 10, 26, 10, 34, 11, 32, 12, 30};
const int length[32] = {2, 4, 4, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 16, 18, 26, 26, 34, 34};
const int* BOX[32] = {box0, box1, box2, box3, box4, box5, box6, box7, box8, box9, box10, box11, box12, box13, box14, box15,
                      box16, box17, box18, box19, box20, box21, box22, box23, box24, box25, box26, box27, box28, box29, box30, box31};
int score = 0;
int width = 25;
int height = 30;
int prelevel = 6;
int runlevel = 31;
int speedlevel = 0;
const int toph = 3;
const int modw = 4;
static int sig = 0;
size_t color_len = 593;
size_t color_size = 12;
size_t str_size = 1000;
int  wthm, dist, lower;
const string unit = "[]";
static struct termios oldt;
const string gmover = "\e[?25h\e[36;26HGame Over!\e[0m\n";

typedef struct 
{
        void (*draw[3])();
} board;

typedef struct
{
        int first;
        int second;
} pair;
      
typedef struct vector
{
        void (*clear)(struct vector*);
        void (*delete)(struct vector*);
        size_t (*size)(const struct vector*); 
        void (*push_back)(struct vector*, int);
        void (*resize)(struct vector*, size_t);
        int (*at)(const struct vector*, size_t);
        void (*reserve)(struct vector*, size_t);
        size_t (*capacity)(const struct vector*);
        void (*set)(struct vector*, size_t, int);
        void (*assign)(struct vector*, size_t, int);
        void (*copy)(struct vector*, const struct vector*);

        int* vec;
        size_t v_size, v_capacity;

} vector;

struct vector box, locus;

typedef struct get_time
{
        void* (*current)(void*);
        void (*set_time)(struct get_time*, int*, int*, int, size_t); 

        string* time;

} get_time;

typedef struct max_distance
{
        void (*delete)(struct max_distance*);
        int (*max_height)(struct max_distance*, int**);
        struct max_distance* (*clear)(struct max_distance*);
        struct max_distance* (*max_vertical_coordinate)(struct max_distance*);

        vector max;

} max_distance;

typedef struct transpose
{
        pair (*mid_point)(vector*);
        void (*addbox)(vector*, int, int);
        void (*delete)(struct transpose*);
        void (*unique)(struct transpose*, vector*);
        void (*optimize)(struct transpose*, vector*, pair);
        struct vector* (*multiple)(struct transpose*, vector*, int, int);
        void (*abstract)(struct transpose*, vector*, int*, int*, int, int);
        void (*coordinate_transformation)(struct transpose*, double, int, int);

        vector new_box, new_cdt, mid;

} transpose;

typedef struct piece
{
        void* (*persig)(void *);
        int (*getsig)(struct piece*);
        void (*delete)(struct piece*);
        void (*drawbox)(struct piece*);
        void (*get_erase)(struct piece*);
        void (*clear_row)(struct piece*);
        int (*drop_bottom)(struct piece*);
        void (*init_color)(struct piece*);
        void (*ghost_cross)(struct piece*);
        void (*random_piece)(struct piece*);
        vector* (*get_piece)(struct piece*);
        void (*init)(struct piece*, int, int);
        void (*move)(struct piece*, int, int);
        void (*check)(struct piece*, int, int);
        void (*get_invoke)(struct piece*, int);
        void (*show_piece)(struct piece*, int);
        void (*shift_piece)(struct piece*, int);
        void (*update)(struct piece*, int, int);
        void (*perplus)(struct piece*, int, int);
        void (*top_point)(struct piece*, vector*);
        void (*rotate)(struct piece*, double, int);
        int (*move_piece)(struct piece*, int, int);
        void (*coordinate)(struct piece*, vector*);
        void (*transform)(struct piece*, double, int);
        void (*draw_piece)(struct piece*, int, int, int);
        string (*get_replace)(struct piece*, string str);
        void (*bomb)(struct piece*, int x, int y, int size);
        void (*coord_comp)(struct piece*, int*, int, int*, int);
        void (*get_preview)(struct piece*, vector*, string, int, string);
        void (*pipe_piece)(struct piece*, vector*, vector*, string, int, int, string, string);
        void (*loop)(struct piece*, void (*lhs)(struct piece*, int, int), void (*rhs)(struct piece*, int), bool);

        board bd;
        int** box_map; 
        transpose trans;
        string** box_color;
        max_distance dists;
        int line, count, full;
        vector *next_preview_piece, preview_box;
        string *old_preview_block, *next_preview_color, *color;
        string cur_shadow, cur_color, old_shadow, cur_preview_block;

} piece;

int get_sig();
int set_sig(int);
void* wait(void*);
void runleave(int);
void board_border();
void board_matrix();
void board_notify();
void init_piece(piece*);
int piece_getsig(piece*);
void init_time(get_time*);
void piece_delete(piece*);
void* piece_persig(void*);
void vector_init(vector*);
int get_argv(int, string*);
void piece_drawbox(piece*);
void vector_clear(vector*);
void vector_delete(vector*);
void piece_clear_row(piece*);
void piece_get_erase(piece*);
int piece_drop_bottom(piece*);
void* get_time_current(void*);
void piece_init_color(piece*);
void piece_ghost_cross(piece*);
vector* piece_get_piece(piece*);
void piece_random_piece(piece*);
void transpose_init(transpose*);
void disable_waiting_for_enter();
void restore_terminal_settings();
size_t vector_size(const vector*);
void piece_init(piece*, int, int);
void piece_move(piece*, int, int);
void transpose_delete(transpose*);
void piece_check(piece*, int, int);
void piece_get_invoke(piece*, int);
void piece_show_piece(piece*, int);
void piece_shift_piece(piece*, int);
void piece_update(piece*, int, int);
void vector_push_back(vector*, int);
void vector_resize(vector*, size_t);
void piece_perplus(piece*, int, int);
void vector_reserve(vector*, size_t);
size_t vector_capacity(const vector*);
void max_distance_init(max_distance*);
void piece_top_point(piece*, vector*);
void vector_set(vector*, size_t, int);
int vector_at(const vector*, size_t n);
void piece_bomb(piece*, int, int, int);
void piece_coordinate(piece*, vector*);
void piece_rotate(piece*, double, int);
void max_distance_delete(max_distance*);
string piece_get_replace(piece*, string);
void vector_assign(vector*, size_t, int);
void vector_copy(vector*, const vector*);
void piece_transform(piece*, double, int);
void transpose_unique(transpose*, vector*);
int piece_move_piece(piece*, int dx, int dy);
void piece_draw_piece(piece*, int, int, int);
max_distance* max_distance_clear(max_distance*);
int max_distance_max_height(max_distance*, int**);
pair transpose_mid_point(vector* mid);
void transpose_optimize(transpose*, vector*, pair);
void piece_coord_comp(piece*, int*, int, int*, int);
void transpose_addbox(vector*, int, int);
vector* transpose_multiple(transpose*, vector*, int, int);
void get_time_set_time(get_time*, int*, int*, int, size_t);
void piece_get_preview(piece*, vector*, string, int, string);
max_distance* max_distance_max_vertical_coordinate(max_distance*);
void transpose_abstract(transpose*, vector*, int*, int*, int, int);
void transpose_coordinate_transformation(transpose*, double, int, int);
void piece_loop(piece*, void (*lhs)(piece*, int, int), void (*rhs)(piece*, int), bool);

int main(int argc, char* argv[])
{
    if ( get_argv(argc, argv) ) 
         exit(0);

    srand(time(NULL));

    vector_init(&box);
    vector_init(&locus);

    piece pg;
    get_time time;
    init_piece(&pg); 
    init_time(&time);

    pthread_t t0, t1;
    pthread_create(&t0, NULL, get_time_current, &time);
    pthread_create(&t1, NULL, piece_persig, &pg);

    pthread_detach(t0);
    pthread_detach(t1);

    pg.getsig(&pg);

    pg.delete(&pg);
    box.delete(&box);
    locus.delete(&locus);
    return 0;
}

int get_argv(int argc, string* argv)
{
    wthm = 2*width+modw;
    dist = modw+wthm+3; 
    lower = height+toph;
    if ( argc > 2 )
    {
         runlevel = atoi(argv[1]);
         prelevel = atoi(argv[2]);
         if ( argc > 3 )
         {
              speedlevel = atoi(argv[3]);
              if ( argc > 4 )
              {
                   width = atoi(argv[4]);
                   if ( argc > 5 ) 
                        height = atoi(argv[5]);
              }
         }
         wthm = 2*width+modw;
         dist = modw+wthm+3; 
         lower = height+toph;
         return 0;
    }
    else if ( argc == 2 )
    {
       string str = argv[1]; 
       if ( !strcmp(str, "-h") || !strcmp(str, "--help") ) 
       {
            printf("Usage: %s [ <runlevel> [ <previewlevel> [ <speedlevel> [ <width> [height] ] ] ] ]\n", argv[0]);
            printf("Range: [ 0 <= runlevel <= 31 ] [ previewlevel >= 1 ] [ speedlevel <= 30 ] [ width >= 17 ] [ height >= 10 ]\n");
            return 1;
       }
       else if ( !strcmp(str, "-v") || !strcmp(str, "--version") )
       {
            printf("Version 1.0.3\n");
            return 1;
       }
       else
       {
            runlevel = atoi(argv[1]);
            return 0;
       }
    }
    return 0;
}

int get_sig()
{   
    return __sync_fetch_and_add(&sig, 0);
}

int set_sig(int value)
{
    return __sync_lock_test_and_set(&sig, value);
} 

void restore_terminal_settings()
{
     tcsetattr(0, TCSANOW, &oldt);
}

void disable_waiting_for_enter()
{
     struct termios newt;
     tcgetattr(0, &oldt);
     newt = oldt;
     newt.c_lflag &= ~(ICANON | ECHO);
     tcsetattr(0, TCSANOW, &newt);
     atexit(restore_terminal_settings);
}

void* wait(void* n)
{
      usleep(1000* *(int*)n);
      return NULL;
}

void runleave(int n)
{ 
     if ( n == 0 )
          printf("%s\n", gmover);
     else
     {
        if ( n == 1 )
             set_sig(22);
        printf("\e[?25h\e[36;4H\n");
     }
}

void vector_clear(vector* this)
{    
     this->v_size = 0;
}
     
int vector_at(const vector* this, size_t n)
{
    return this->vec[n];
}

void vector_set(vector* this, size_t i, int k)
{
     this->vec[i] = k;
}

void vector_copy(vector* this, const vector* rhs)
{
     this->v_size = rhs->v_size;
     this->v_capacity = rhs->v_capacity;
     this->vec = realloc(this->vec, this->v_capacity*sizeof(int));
     memcpy(this->vec, rhs->vec, this->v_size*sizeof(int));
}

void vector_reserve(vector* this, size_t n)
{
     this->v_capacity = n;
     int* new_vec = malloc(n*sizeof(int));
     size_t s = n > this->v_size ? this->v_size : n;
     memcpy(new_vec, this->vec, s*sizeof(int));
     delete_all(this->vec);
     this->vec = new_vec;
}

void vector_resize(vector* this, size_t n)
{
     this->reserve(this, n);
     this->v_size = n;
}

void vector_assign(vector* this, size_t len, int k)
{
     if ( this->v_capacity < len ) 
          this->reserve(this, 2*len+1);
     for (size_t i = 0; i!=len; ++i)
          this->set(this, i, k); 
     this->v_size = len;
}

size_t vector_size(const vector* this)
{
       return this->v_size;
}

size_t vector_capacity(const vector* this)
{
       return this->v_capacity;
}

void vector_push_back(vector* this, int k)
{
      if ( this->v_size >= this->v_capacity ) 
           this->reserve(this, 2*this->v_capacity+1);
      this->vec[this->v_size++] = k;
}

void vector_delete(vector* this)
{
     delete_all(this->vec);
     this->vec = 0;
     this->v_size = 0;
     this->v_capacity = 0;
}

void vector_init(vector* this)
{
     this->vec = 0;
     this->v_size = 0;
     this->v_capacity = 0;
     this->at = vector_at;
     this->set = vector_set;
     this->copy = vector_copy;
     this->size = vector_size;
     this->clear = vector_clear;
     this->resize = vector_resize;
     this->delete = vector_delete;
     this->assign = vector_assign;
     this->capacity = vector_capacity;
     this->reserve = vector_reserve;
     this->push_back = vector_push_back;
}

max_distance* max_distance_clear(max_distance* this)
{
     this->max.clear(&this->max);
     return this;
}

max_distance* max_distance_max_vertical_coordinate(max_distance* this)
{
     size_t size = 0;
     size_t len = box.size(&box)/2;
     pair* col = malloc(len*sizeof(pair));
     for (size_t i = 0; i != 2*len; i += 2)
     {   
          size_t j;
          int q = box.at(&box, i);
          int p = box.at(&box, i+1);
          for (j = 0; j != size; ++j) 
          {
               if (col[j].first == p)
                   break;
          }
          int* m = &col[j].first;
          int* n = &col[j].second;
          if ( j == size )
          {
               *m = p;
               *n = q;
               ++size;
          }    
          else 
          { 
               if ( *n < q ) 
                    *n = q;
          }
     }
     for (size_t i = 0; i != size; ++i)
     {
          this->max.push_back(&this->max, col[i].second);
          this->max.push_back(&this->max, col[i].first);
     }
     delete_all(col);
     return this;
}

int max_distance_max_height(max_distance* this, int** box_map)
{   
    int i = 0;
    for (size_t j = 0; i != height; j += 2)
    { 
         int k = this->max.at(&this->max,j);
         int l = this->max.at(&this->max,j+1);
         if ( box_map[k+i-modw][l/2-toph] == 1 ) return i-1;
         if ( k+i == lower ) return i;
         if ( j+2 == this->max.size(&this->max) )
         { 
              j = -2; 
              ++i;
         }
    }
    return 0;
}

void max_distance_delete(max_distance* this)
{
     this->max.delete(&this->max);
}  

void max_distance_init(max_distance* this)
{
     vector_init(&this->max);
     this->clear = max_distance_clear;
     this->delete = max_distance_delete;
     this->max_height = max_distance_max_height;
     this->max_vertical_coordinate = max_distance_max_vertical_coordinate;
}
      
void transpose_unique(transpose* this, vector* vec)
{
     size_t len = 0, j, k;
     this->mid.clear(&this->mid);
     for (size_t i = 0; i != vec->size(vec); i += 2)
     {
          int p = vec->at(vec, i);
          int q = vec->at(vec, i+1);
          for (j = 0; j != len; ++j)
          {
               k = 2*j;
               if ( this->mid.at(&this->mid, k) == p && this->mid.at(&this->mid, k+1) == q )
                    break;
          }
          if ( j == len )
          {
               ++len;
               this->mid.push_back(&this->mid, p);
               this->mid.push_back(&this->mid, q);
          } 
     }
     vec->copy(vec, &this->mid);
}

void transpose_addbox(vector* new_box, int k, int j)
{
     for (size_t i = 0; i != new_box->size(new_box); i += 2)
     {
          int v = new_box->at(new_box,i+k)+j;
          new_box->set(new_box ,i+k, v);
     }
}

pair transpose_mid_point(vector* mid)
{
     size_t size = mid->size(mid);
     size_t len = size/2-(size%4)/2;
     pair tuple = { mid->at(mid, len), mid->at(mid, len+1) };
     return tuple;
}

vector* transpose_multiple(transpose* this, vector* cur_box, int b, int d)
{
     size_t len = cur_box->size(cur_box);
     this->new_box.assign(&this->new_box, len, 0);
     this->new_box.copy(&this->new_box, cur_box);
    
     for (size_t i = 0; i != len-2; i += 2)
     {
          int l = cur_box->at(cur_box, i+3);
          int m = cur_box->at(cur_box, i+1);
          int k = this->new_box.at(&this->new_box, i+1);
          this->new_box.set(&this->new_box, i+3, k+(l-m)*b/d); 
     }
     return &this->new_box;
}

void transpose_coordinate_transformation(transpose* this, double dx, int m, int n)
{
     size_t len = this->new_box.size(&this->new_box);
     this->new_cdt.assign(&this->new_cdt, len, 0);
     for (int i = 0; i != len; i += 2)                                                          // row=(x-m)*zoomx*cos(a)-(y-n)*zoomy*sin(a)+m
     {                                                                                          // col=(x-m)*zoomx*sin(a)+(y-n)*zoomy*cos(a)+n
          this->new_cdt.set(&this->new_cdt, i, this->new_box.at(&this->new_box, i+1)+m-n);      // a=-pi/2 zoomx=+1 zoomy=+1 dx=0 dy=0
          this->new_cdt.set(&this->new_cdt, i+1, (this->new_box.at(&this->new_box, i)-m)*dx+n); // a=-pi/2 zoomx=-1 zoomy=+1 dx=0 dy=0 
     }                                                                                          // a=+pi/2 zoomx=+1 zoomy=-1 dx=0 dy=0
     if ( dx == 0.5 ) 
          this->unique(this, &this->new_cdt);
}

void transpose_abstract(transpose* this, vector* mid, int* i, int* j, int b, int d)
{
     pair tuple = this->mid_point(this->multiple(this, mid, b, d));
     *i = tuple.first;
     *j = tuple.second;
}

void transpose_optimize(transpose* this, vector* new_box, pair dual)
{
     int coord[2] = { dual.first, dual.second };
     for (size_t i = 0; i != 2; ++i)
     {
          int val = coord[i];
          if ( val )
               this->addbox(new_box, i, val);
     }
}

void transpose_delete(transpose* this)
{
     this->mid.delete(&this->mid);
     this->new_box.delete(&this->new_box);
     this->new_cdt.delete(&this->new_cdt);
}

void transpose_init(transpose* this)
{
     this->unique = transpose_unique;
     this->addbox = transpose_addbox;
     this->delete = transpose_delete;
     this->optimize = transpose_optimize;
     this->multiple = transpose_multiple;
     this->abstract = transpose_abstract;
     this->mid_point = transpose_mid_point;
     this->coordinate_transformation = transpose_coordinate_transformation;
     
     vector_init(&this->mid);
     vector_init(&this->new_box); 
     vector_init(&this->new_cdt);
}

void get_time_set_time(get_time* this, int* lhs, int* rhs, int value, size_t i)
{
     if ( *lhs == value )
     {
          ++*rhs;
          *lhs = 0;
     }
     this->time[i][0] = *lhs/10+'0';
     this->time[i][1] = *lhs%10+'0';
}

void* get_time_current(void* ptr)
{
      get_time* this = (get_time*)ptr;
      int j = width-9;
      this->time = malloc(4*sizeof(string));
      for (size_t i = 0; i != 4; ++i)
      {
          this->time[i] = malloc(3);
          this->time[i][2] = 0;
      }
      string line = malloc(j+1);
      string color = "\e[1;33m";
      for (size_t k = 0; k != j; ++k)
           line[k] = '-';
      line[j] = '\0';
      printf("\e[2;6H%s%s[Time \e[2;%dH%s]%s\e[0m\n", color, line, 23+j, color, line);
      delete_all(line);

      int delay = 1000;
      int day = 0, hour = 0, minute = 0, second = 0;

      while (true)
      {
             if (  get_sig() == 22 ) 
             { 
                   runleave(0); 
                   break;
             }
             pthread_t tid;
             pthread_create(&tid, NULL, wait, &delay);
             this->set_time(this, &second, &minute, 60, 0);
             this->set_time(this, &minute, &hour, 60, 1);
             this->set_time(this, &hour, &day, 24, 2);
             this->time[3][0] = day/10+'0';
             this->time[3][1] = day%10+'0';
             printf("\e[2;%dH%s%s:%s:%s:%s\e[0m\n", 12+j, color, this->time[3], this->time[2], this->time[1], this->time[0]);
             pthread_join(tid, NULL);
             ++second;
      }

      for (size_t i = 0; i != 4; ++i)
           delete_all(this->time[i]);
      delete_all(this->time);
      return NULL;
}

void init_time(get_time* this)
{
     this->current = get_time_current;
     this->set_time = get_time_set_time;
}

void piece_check(piece* this, int i, int j)
{ 
     if ( ! this->box_map[i-modw][j/2-toph] ) 
          this->full = 0; 
}

void piece_update(piece* this, int i, int j)
{ 
     if ( !this->box_map[i-modw][j/2-toph] )
          printf("\e[%d;%dH  ", i, j);
     else
          printf("\e[%d;%dH\e[%s%s\e[0m", i, j, this->box_color[i-modw][j/2-toph], unit);
}

int piece_drop_bottom(piece* this)
{
    return this->dists.clear(&this->dists)->max_vertical_coordinate(&this->dists)->max_height(&this->dists, this->box_map);
}

vector* piece_get_piece(piece* this)
{
    box.clear(&box);
    int len = rand()%runlevel;
    for (const int* p = BOX[len]; p != BOX[len]+length[len]; ++p)
         box.push_back(&box, *p);
    return &box;
}

void piece_init(piece* this, int i, int j)
{ 
     this->box_map[i-modw][j/2-toph] = 0;
     strcpy(this->box_color[i-modw][j/2-toph], ""); 
}

void piece_init_color(piece* this)
{
     size_t index = 0;
     this->color = malloc(color_len*sizeof(string));
     for (size_t i = 0; i != color_len; ++i)
         this->color[i] = malloc(12*sizeof(char));
     for (int i = 0; i != 9; ++i)
     {
          for (int j = 0;  j != 9; ++j)
               snprintf(this->color[index++], color_size, "1;3%d;4%dm", i, j);
     }
   
     for (int i = 0, j = 38; i != 2; ++i, j += 10)
     {
          for (int k = 0; k != 256; ++k)
               snprintf(this->color[index++], color_size, "%d;5;%d;1m", j, k);
     }
}

void piece_loop(piece* this, void (*lhs)(piece*, int, int), void (*rhs)(piece*, int), bool call)
{
     for (int i = 4, j = 6, l = wthm; i <= lower; j += 2)
     {
          this->full = 1; 
          lhs(this, i, j);
          if ( !this->full || j == l )
          {
               if ( this->full && call ) 
                    rhs(this, i);
               ++i;
               j = 4; 
          }
     }
}

string piece_get_replace(piece* this, string src)
{ 
       string pos, begin = src;
       while ( (pos = strstr(src, unit)) ) 
       {
                size_t i = pos-src;
                src[i] = ' ';
                src[i+1] = ' ';
                src = pos+2;
       }
       return begin;
}

void piece_shift_piece(piece* this, int i)
{
     this->line++;
     for (int j = i; j > toph+1; --j)
     {
          for (size_t n = 0; n != width; ++n)
          {
               strcpy(this->box_color[j-modw][n], this->box_color[j-5][n]);
               this->box_map[j-modw][n] = this->box_map[j-5][n];
          } 
     }
     for (size_t n = 0; n != width; ++n)
     {
          this->box_map[0][n] = 0;
          strcpy(this->box_color[0][n], "");
     }
}

void piece_get_preview(piece* this, vector* box0, string old_block, int n, string new_block_color)
{
     char* buffer = malloc(str_size);
     size_t box_size = box0->size(box0);
     for (size_t i = 0; i != box_size; i += 2)
     {
          int col = box0->at(box0,i+1)-(n-dist);
          snprintf(buffer, str_size, "\e[%d;%dH%s", box0->at(box0, i)-1, col, unit);
          strcat(this->cur_preview_block, buffer);
     }
     delete_all(buffer);
     printf("%s\e[%s%s\e[0m\n", this->get_replace(this, old_block), new_block_color, this->cur_preview_block);
}

void piece_get_erase(piece* this)
{  
     printf("%s\e[0m\n", this->get_replace(this, this->old_shadow)); 
}

void piece_pipe_piece(piece* this, vector* next_preview_box0, vector* next_preview_box1, 
string old_preview_box, int n, int p, string new_preview_color0, string new_preview_color1)
{
     next_preview_box0->clear(next_preview_box0);
     strcpy(this->cur_preview_block, "");
     if ( p ) 
     {
          next_preview_box0->copy(next_preview_box0, this->get_piece(this));
          strcpy(this->cur_color,this->color[rand()%color_len]);
          strcpy(new_preview_color0, this->cur_color);
          this->get_preview(this, &box, old_preview_box, n, this->cur_color);
     }
     else 
     {
          next_preview_box0->copy(next_preview_box0, next_preview_box1);
          strcpy(new_preview_color0, new_preview_color1);
          this->get_preview(this, next_preview_box0, old_preview_box, n, new_preview_color0);
     }
     strcpy(old_preview_box, this->cur_preview_block);
}

void piece_get_invoke(piece* this, int n)
{
     for (size_t i = 0; i != prelevel-1; ++i) 
     {
          this->pipe_piece(this, &this->next_preview_piece[i], &this->next_preview_piece[i+1], 
          this->old_preview_block[i], 12*(2-i), n, this->next_preview_color[i], this->next_preview_color[i+1]);
     }
}
            
void piece_show_piece(piece* this, int n)
{
     int end = prelevel-1;
     strcpy(this->cur_color, this->next_preview_color[0]);
     this->preview_box.copy(&this->preview_box,  &this->next_preview_piece[0]);
     this->get_invoke(this, n);
     strcpy(this->cur_preview_block, "");
     this->next_preview_piece[end].copy(&this->next_preview_piece[end], this->get_piece(this));
     strcpy(this->next_preview_color[end], this->color[rand()%color_len]);
     this->get_preview(this, &box, this->old_preview_block[end], 12*(2-end), this->next_preview_color[end]);
     strcpy(this->old_preview_block[end], this->cur_preview_block);
     box.copy(&box, &this->preview_box);
}

void piece_coordinate(piece* this, vector* lbox)
{
     strcpy(this->cur_shadow, "");
     locus.copy(&locus, lbox);
     char* buffer = malloc(str_size);
     for (size_t i = 0; i != locus.size(&locus); i += 2)
     {
          snprintf(buffer, str_size, "\e[%d;%dH%s", locus.at(&locus, i), locus.at(&locus, i+1), unit);
          strcat(this->cur_shadow, buffer);
     } 
     delete_all(buffer);
}

void piece_draw_piece(piece* this, int n, int dx, int dy)
{
     if ( n )
     {
          strcpy(this->cur_color, this->color[rand()%color_len]);
          this->coordinate(this, this->get_piece(this));
     }
     else
     {
          strcpy(this->cur_color, this->next_preview_color[0]);
          this->coordinate(this, &this->next_preview_piece[0]);
     }
     this->drawbox(this);
     if ( !this->move_piece(this, dx, dy) ) 
     { 
          set_sig(22); 
          runleave(0); 
     }
}

void piece_random_piece(piece* this)
{
     this->count++;
     size_t k = height-1;
     for (size_t i = 0, j = 6; i != this->count; j += 2)
     {
          if ( j == wthm ) { j = 4; ++i; }
          if ( rand()%2 ) 
          { 
               this->box_map[k-i][j/2-toph] = 1; 
               strcpy(this->box_color[k-i][j/2-toph], this->color[rand()%color_len]);
          }
     }
     if ( this->count == k ) 
          this->count = 0;
}

void piece_coord_comp(piece* this, int* x, int y, int* u, int v)
{
     if ( *x <= y )
     { 
          *x = y;
          if ( *u < v ) 
               *u = v;
     }
}

void piece_top_point(piece* this, vector* cur_box)
{
     int x = 0, y = 0; 
     int u = cur_box->at(cur_box,0);
     int v = cur_box->at(cur_box,1);
     for (size_t i = 0; i != cur_box->size(cur_box); i += 2)
     {
          int p = cur_box->at(cur_box, i);
          int q = cur_box->at(cur_box, i+1);
          this->coord_comp(this, &x, p, &y, q);
          this->coord_comp(this, &p, u, &q, v);
          cur_box->set(cur_box, i, p);
          cur_box->set(cur_box, i+1, q);
     }
     if ( x-u == 3 && y-v == 6 )
     {
          cur_box->clear(cur_box);
          cur_box->push_back(cur_box, x-3);
          cur_box->push_back(cur_box, y-6);
          cur_box->push_back(cur_box, x-3);
          cur_box->push_back(cur_box,   y);
          cur_box->push_back(cur_box,   x);
          cur_box->push_back(cur_box, y-6);
          cur_box->push_back(cur_box,   x);
          cur_box->push_back(cur_box,   y);
     }
}

void piece_bomb(piece* this, int x, int y, int size)
{
     const int len = 18;
     string empty = malloc(str_size);
     string buffer = malloc(str_size);
     strcpy(empty, "");

     int radius[len];
     size_t r_size = 0;
     radius[r_size++] = x-1;
     radius[r_size++] = y-2;
     radius[r_size++] = x-1;
     radius[r_size++] = y;
     radius[r_size++] = x-1;
     radius[r_size++] = y+2;
     radius[r_size++] = x;
     radius[r_size++] = y-2;
     radius[r_size++] = x;
     radius[r_size++] = y;
     radius[r_size++] = x;
     radius[r_size++] = y+2;
     radius[r_size++] = x+1;
     radius[r_size++] = y-2;
     radius[r_size++] = x+1;
     radius[r_size++] = y;
     radius[r_size++] = x+1;
     radius[r_size++] = y+2;

     for (size_t j = 0; j != len; j += 2)
     {
          int p = radius[j];
          int q = radius[j+1];
          bool boolq = (q <= wthm && q > modw);
          bool boolp = (p > toph && p <= lower);
          if ( boolp && boolq )
          {
               if ( ! this->box_map[p-modw][q/2-toph] && p+q != x+y && size != 8 ) 
                    continue;
               snprintf(buffer, str_size, "\e[%d;%dH\040\040", p, q);
               strcat(empty, buffer);
               this->init(this, p, q); 
          }
     }
     int delay = 30;
     wait(&delay);
     printf("%s\n", empty);
     delete_all(empty, buffer);
} 

void piece_clear_row(piece* this)
{
     vector cur_box;
     vector_init(&cur_box);
     cur_box.copy(&cur_box, &locus);
     size_t len = cur_box.size(&cur_box);
     if ( len == 16 ) 
          this->top_point(this, &cur_box);
     for (size_t i = 0; i != cur_box.size(&cur_box); i += 2)
     {
          int x = cur_box.at(&cur_box, i);
          int y = cur_box.at(&cur_box, i+1);
          if ( len == 16 )
          {
               this->bomb(this, x, y, cur_box.size(&cur_box));
          }
          else
          {
               this->box_map[x-modw][y/2-toph] = 1;
               strcpy(this->box_color[x-modw][y/2-toph], this->cur_color);
          }
     }
     cur_box.delete(&cur_box);
     this->line = 0;
     this->loop(this, piece_check, piece_shift_piece, true);
     if ( ! this->line ) return;
     int num = this->line*200-100;
     score += num;
     printf("\e[1;34m\e[%d;%dH%d\e[0m\n", toph+10, dist+56, score);
     if ( score%5000 < num && speedlevel < 30 )
     {
          this->random_piece(this);
          ++speedlevel;
          printf("\e[1;34m\e[%d;%dH%d\e[0m\n", toph+10, dist+26, speedlevel);
     }
     this->loop(this, piece_update, piece_shift_piece, false);
}        

void* piece_persig(void* ptr)
{
      int j = 0;
      int delay = 20;
      piece* this = (piece*)ptr;
      while ( ++j )
      {
              if ( j != 1 ) wait(&delay);
              int sigswap = set_sig(0);
              switch (sigswap)
              {
                      case 22 : runleave(0);                      return NULL;
                      case 23 : this->transform(this,  -1,   1);  break;
                      case 24 : this->transform(this,   1,   1);  break;
                      case 25 : this->transform(this,  -2,   1);  break;
                      case 26 : this->transform(this,   0.5, 1);  break;
                      case 27 : this->transform(this,   0,  -2);  break;
                      case 28 : this->transform(this,   0,   2);  break;
                      case 29 : this->transform(this,   1,   0);  break;
                      case 30 : this->transform(this,  -1,   0);  break;
                      case 31 : this->transform(this, this->drop_bottom(this),0); break;
              }
              if ( j == 31-speedlevel ) 
              {    
                   j = 0; 
                   this->transform(this, 1, 0); 
              }
      }
      return NULL;
}

int piece_getsig(piece* this)
{
    printf("\e[?25l");
    char key;
    char pool = '\e'; 
    char arry[3] = "";
    disable_waiting_for_enter();
    while ( true )
    {
            key = getchar();
            set_sig(0);
            arry[0] = arry[1]; 
            arry[1] = arry[2];
            arry[2] = key;
            if   ( key == ' ' || key == '\n' ) set_sig(31);
            else if ( key == pool && arry[1] == pool ) { runleave(1); return 0; }
            else if ( arry[0] == pool && arry[1] == '[' )
            {
                      switch (key)
                      {
                              case 'A':  set_sig(23); break;
                              case 'B':  set_sig(29); break;
                              case 'D':  set_sig(27); break;
                              case 'C':  set_sig(28); break;
                      }
                 
            }
            else
            {
                      switch (key)
                      {
                              case 'W': case 'w': set_sig(23); break;
                              case 'T': case 't': set_sig(24); break;
                              case 'M': case 'm': set_sig(25); break;
                              case 'N': case 'n': set_sig(26); break;
                              case 'S': case 's': set_sig(29); break;
                              case 'A': case 'a': set_sig(27); break;
                              case 'D': case 'd': set_sig(28); break;
                              case 'U': case 'u': set_sig(30); break;
                              case 'P': case 'p': set_sig(19); break;
                              case 'R': case 'r': set_sig(18); break;
                              case 'Q': case 'q': runleave(1); return 0;
                      }
            }
            if ( get_sig() == 22 ) 
            { 
                 runleave(1); 
                 return 0;
            }
    }
    return 0;
}

void piece_drawbox(piece* this)
{ 
     strcpy(this->old_shadow, this->cur_shadow); 
     printf("\e[%s%s\e[0m\n", this->cur_color, this->cur_shadow);
}

int piece_move_piece(piece* this, int dx, int dy)
{
    size_t len = locus.size(&locus);
    for (size_t i = 0; i != len; i += 2)
    { 
         int x = locus.at(&locus, i)+dx;
         int y = locus.at(&locus, i+1)+dy;
         bool boolx = (x <= toph || x > lower);
         bool booly = (y >= wthm+2 || y <= modw+1);
         if ( boolx || booly ) return 0;
         if ( this->box_map[x-modw][y/2-toph] )
         {
             if ( len == 2 )
             {
                  for (int j = lower; j > x; --j)
                  {
                       if ( ! this->box_map[j-modw][y/2-toph] ) return 1;
                  }
             }
             return 0;
         }
    }
    return 1;  
}

void piece_ghost_cross(piece* this)
{
     int i = locus.at(&locus, 0);
     int j = locus.at(&locus, 1);
     if ( this->box_map[i-modw][j/2-toph] ) 
          printf("\e[%d;%dH\e[%s%s\e[0m\n", i, j, this->box_color[i-modw][j/2-toph], unit);
}

void piece_perplus(piece* this, int dx, int dy)
{
     if ( locus.size(&locus) == 2 ) 
          this->ghost_cross(this);
     pair p = { dx, dy };
     this->trans.optimize(&this->trans, &box, p);
     this->coordinate(this, &box);
}

void piece_move(piece* this, int dx, int dy)
{
     if (this->move_piece(this, dx, dy))
     {
         this->get_erase(this);
         this->perplus(this, dx, dy);
         this->drawbox(this);
     }
     else
     {
         if ( dx == 1 )
         {
              this->clear_row(this);  
              this->draw_piece(this, 0, dx, dy); 
              this->show_piece(this, 0);
         }
     } 
}

void piece_rotate(piece* this, double dx, int dy)
{     
     int m, n, p, q;
     pair mid = this->trans.mid_point(&box);
     this->trans.abstract(&this->trans, &box, &m, &n, 1, 2);
     this->trans.coordinate_transformation(&this->trans, dx, m, n); 
     this->trans.abstract(&this->trans, &this->trans.new_cdt, &p, &q, 2, 1);
     pair deta = { mid.first-p, mid.second-q};
     this->trans.optimize(&this->trans, &this->trans.new_box, deta);
     locus.copy(&locus, &this->trans.new_box);
     if (this->move_piece(this, 0, 0))
     {
         this->get_erase(this); 
         this->coordinate(this, &this->trans.new_box);
         this->drawbox(this); 
         box.copy(&box, &locus);
     }
     else
     {
         locus.copy(&locus, &box);
     }
}

void piece_transform(piece* this, double dx, int dy)
{ 
     if (dy != 1)
         this->move(this, dx, dy);
     else
         this->rotate(this, dx, dy); 
}

void piece_delete(piece* this)
{
     for (int i = 0; i != height; ++i)
     {
          for (size_t j = 0; j != width; ++j)
               delete_all(this->box_color[i][j]);
          delete_all(this->box_map[i], this->box_color[i]);
     } 
     delete_all(this->box_map, this->box_color);

     for (size_t i = 0; i != prelevel; ++i)
     {
          delete_all(this->old_preview_block[i], this->next_preview_color[i]);
     }

     delete_all(this->old_preview_block, this->next_preview_color);
     delete_all(this->cur_color, this->cur_shadow, this->old_shadow, this->cur_preview_block);

     for (size_t i = 0; i != color_len; ++i)
          delete_all(this->color[i]);
     for (size_t i = 0; i != prelevel; ++i)
          this->next_preview_piece[i].delete(&this->next_preview_piece[i]);
     delete_all(this->color, this->next_preview_piece);
     this->preview_box.delete(&this->preview_box);
}

void init_piece(piece* this)
{
     this->getsig = piece_getsig;
     this->persig = piece_persig;
     this->drawbox = piece_drawbox;
     this->get_erase = piece_get_erase;
     this->clear_row = piece_clear_row; 
     this->drop_bottom = piece_drop_bottom;
     this->init_color = piece_init_color;
     this->ghost_cross = piece_ghost_cross;
     this->init = piece_init;
     this->move = piece_move;
     this->random_piece = piece_random_piece;
     this->check = piece_check;   
     this->get_invoke = piece_get_invoke;
     this->show_piece = piece_show_piece;
     this->shift_piece = piece_shift_piece;
     this->update = piece_update;
     this->perplus = piece_perplus;
     this->get_piece = piece_get_piece;
     this->rotate = piece_rotate;
     this->move_piece = piece_move_piece;
     this->coordinate = piece_coordinate;
     this->draw_piece = piece_draw_piece;
     this->transform = piece_transform;
     this->get_replace = piece_get_replace;
     this->bomb = piece_bomb;
     this->top_point = piece_top_point;
     this->coord_comp = piece_coord_comp;
     this->get_preview = piece_get_preview;
     this->loop = piece_loop;
     this->pipe_piece = piece_pipe_piece;
     this->delete = piece_delete;

     prelevel = (prelevel < 1)?6:prelevel;            
     speedlevel = (speedlevel > 30)?0:speedlevel;
     runlevel = (runlevel < 0 || runlevel > 31)?32:runlevel+1;            
     width = width<17?25:width;
     height = height<10?30:height;
     lower = height+toph;
     wthm = 2*width+modw;
     dist = modw+wthm+3;

     this->line = 0;
     this->count = 0;
     this->full = 1;

     transpose_init(&this->trans);
     max_distance_init(&this->dists);
     vector_init(&this->preview_box);
    
     this->box_map = malloc(height*sizeof(int*));
     this->box_color = malloc(height*sizeof(string*));

     for (size_t i = 0; i != height; ++i)
     {
          this->box_map[i] = malloc(width*sizeof(int));
          this->box_color[i] = malloc(width*sizeof(string));
          for (size_t j = 0; j != width; ++j)
          {
               this->box_map[i][j] = 0;
               this->box_color[i][j] = malloc(color_size);
          }
     }

     this->old_preview_block = malloc(prelevel*sizeof(string));
     this->next_preview_color = malloc(prelevel*sizeof(string));
     this->cur_color = malloc(str_size);
     this->cur_shadow = malloc(str_size);
     this->old_shadow = malloc(str_size);
     this->cur_preview_block = malloc(str_size);
     for (size_t i = 0; i != prelevel; ++i)
     {
          this->old_preview_block[i] = malloc(str_size);
          this->next_preview_color[i] = malloc(str_size);
     }

     this->next_preview_piece = malloc(prelevel*sizeof(vector));
     for (size_t i = 0; i != prelevel; ++i)
          vector_init(&this->next_preview_piece[i]);
     printf("%c%c%c%c%c%c",27,'[','H',27,'[','J' );
     this->init_color(this);
     this->bd.draw[0] = board_border;
     this->bd.draw[1] = board_matrix;
     this->bd.draw[2] = board_notify;
     for (size_t i = 0; i != 3; ++i)
          this->bd.draw[i]();
     this->show_piece(this, 1);
     this->draw_piece(this, 1, 0, 1);
}

void board_border()
{
     int num = rand()%145+6;
     string boucol = "\e[38;5";
     for (int i = 6; i <= wthm; i += 2)
          printf("%s;%d;1m\e[%d;%dH==%s;%d;1m\e[%d;%dH==\e[0m", boucol, num+i, toph, i, boucol, num+i+25, lower+1, i);
     for (int i = toph; i <= lower+1; ++i)
          printf("%s;%d;1m\e[%d;%dH||%s;%d;1m\e[%d;%dH||\e[0m", boucol, num+i, i, modw, boucol, num+i+30, i, wthm+2);
}

void board_notify()
{
     printf("\e[1;31m\e[%d;%dHRunLevel\e[1;31m\e[%d;%dHPreviewLevel\e[0m\n", toph+9, dist, toph+9, dist+11);
     printf("\e[1;31m\e[%d;%dHSpeedLevel\e[1;31m\e[%d;%dHScore\e[0m\n", toph+9, dist+26, toph+9, dist+56);
     printf("\e[1;31m\e[%d;%dHWidth\e[1;31m\e[%d;%dHHeight\e[0m\n", toph+9, dist+39, toph+9, dist+47);
     printf("\e[1;34m\e[%d;%dH%d\e[1;34m\e[%d;%dH%d\e[0m\n", toph+10, dist, runlevel-1, toph+10, dist+11, prelevel);
     printf("\e[1;34m\e[%d;%dH%d\e[1;34m\e[%d;%dH%d\e[0m\n", toph+10, dist+26, speedlevel, toph+10, dist+56, score);
     printf("\e[1;34m\e[%d;%dH%d\e[1;34m\e[%d;%dH%d\e[0m\n", toph+10, dist+39, width, toph+10, dist+47, height);
     printf("\e[38;5;34;1m\e[%d;%dHM|m      ===   double         N|n          ===   half\n", toph+12, dist);
     printf("\e[%d;%dHQ|q|ESC  ===   exit           U|u          ===   one step up\n" ,toph+13, dist);
     printf("\e[%d;%dHP|p      ===   pause          S|s|down     ===   one step down\n" ,toph+14, dist);
     printf("\e[%d;%dHR|r      ===   resume         A|a|left     ===   one step left\n" ,toph+15, dist);
     printf("\e[%d;%dHW|w|up   ===   rotate         D|d|right    ===   one step right\n", toph+16, dist);
     printf("\e[%d;%dHT|t      ===   transpose      Space|enter  ===   drop all down\n", toph+17, dist);
     printf("\e[38;5;106;1m");
     printf("\e[%d;%dHVersion 1.0.3\n", toph+19, dist);
     printf("\e[%d;%dHhttps://github.com/deepgrace", toph+20, dist);
     printf("\e[%d;%dHGrace <complex.invoke@gmail.com>", toph+21, dist);
}

void board_matrix()
{
     string one = " ";
     string end = "\e[0m";
     string block = "[][]";
     string two = "  ";
     string cube = "   ";
     string five = "     ";
     string ten = "          ";         
     string equ = "\e[38;5;191;1m";
     string colbon = "\e[38;5;47;1m";
     string mcol = "\e[38;5;30;1m";
     string str5 = "                    ";

     size_t len_colbon = strlen(colbon);
     size_t len_mcol = strlen(mcol);
     size_t len_end = strlen(end);
     size_t len_block = strlen(block);
     size_t block_end = len_block+len_end;

     size_t len_str4 = len_mcol+len_end+3;
     string str4 = malloc(len_str4);
     snprintf(str4, len_str4, "%s%s%s", mcol, "[]", end);
     
     size_t len_str0 = len_colbon+len_end+3;
     string str0 = malloc(len_str0);
     snprintf(str0, len_str0, "%s%s%s", colbon, "[]", end);

     size_t len_str1 = len_colbon+block_end+1;
     string str1 = malloc(len_str1);
     snprintf(str1, len_str1, "%s%s%s", colbon, block, end);

     size_t len_str6 = len_mcol+block_end+3;
     string str6 = malloc(len_str6);
     snprintf(str6, len_str6, "%s%s%s%s", mcol, "[]", block, end);

     size_t len_str2 = len_colbon+block_end+3;
     string str2 = malloc(len_str2);
     snprintf(str2, len_str2, "%s%s%s%s", colbon, "[]", block, end);


     size_t len_str3 = len_colbon+2*len_block+len_end+1;
     string str3 = malloc(len_str3);
     snprintf(str3, len_str3, "%s%s%s%s", colbon, block, block, end);
    
     size_t len_str = len_str0+2*len_str3+8;
     string str = malloc(len_str);
     snprintf(str, len_str, "%s%s%s%s%s%s", cube, str0, two, str3, two, str3);


     size_t len_str7 = 2*len_str1+len_str+len_str4+6;
     string str7 = malloc(len_str7);
     snprintf(str7, len_str7, "%s%s%s%s%s%s%s", str1, one, str1, str, str4, two, two);

     size_t len_equ = strlen(equ);
     size_t len_str8 = 4*len_str0+2*len_equ+25;
     string str8 = malloc(len_str8);
     snprintf(str8, len_str8, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s", str0, one, equ, 
       "row", one, str0, cube, str0, two, str0, one, equ, "(x-m)*zoomx", two);

     size_t len_str9 = 3*len_str0+2*len_equ+25;
     string str9 = malloc(len_str9);
     snprintf(str9, len_str9, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s", one, equ, 
        "=", one, str0, two, str0, ten, cube, one, str0, cube, equ, cube);

     size_t len_str10 = 4*len_str0+len_equ+28;
     string str10 = malloc(len_str10);
     snprintf(str10, len_str10, "%s%s%s%s%s%s%s%s%s%s%s%s%s", ten, five, 
             two, str0, two, str0, one, equ, "+", one, str0, five, str0);

     size_t len_str11 = 5*len_str0+3*len_equ+32;
     string str11 = malloc(len_str11);
     snprintf(str11, len_str11, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", cube, one, str0, two, 
       equ, "cos(a)", one, equ, "sin(a)", two, str0, two, str0, cube, str0, two, equ, "m", two, str0);

     size_t len_str12 = 4*len_str0+2*len_equ+31;
     string str12 = malloc(len_str12);
     snprintf(str12, len_str12, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", one, equ, "col", 
         one, str0, cube, str0, two, str0, one, equ, "(y-n)*zoomy", two, str0, five, one);
     
     size_t len_str13 = 4*len_str0+3*len_equ+28;
     string str13 = malloc(len_str13);
     snprintf(str13, len_str13, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", one, equ, "-sin(a)", 
            one, equ, "cos(a)", two, str0, two, str0, cube, str0, two, equ, "n", two, str0);

     size_t len_str14 = 2*len_str1+len_str+8;
     string str14 = malloc(len_str14);
     snprintf(str14, len_str14, "%s%s%s%s%s%s", str1, one, str1, str, five, one); 
    
     size_t len_str15 = 3*len_str1+len_str0+7;
     string str15 = malloc(len_str15);
     snprintf(str15, len_str15, "%s%s%s%s%s%s%s", str1, two, str0, cube, str1, one, str1); 
    
     printf("\e[%d;%dH%s          %s%s%s%s%s%s%s\n", toph+23, dist, colbon, two, str2, one, str5, str5, str2, str4);
     printf("\e[%d;%dH%s%s%s%s%s%s%s%s%s\n", toph+30, dist, ten, two, str0, two, two, five, str5, str5, str0);
     printf("\e[%d;%dH%s%s%s%s%s%s%s%s%s%s%s\n",toph+25, dist, str7, str1, ten, cube, str1, two, str0, cube, str1, one, str1);
     printf("\e[%d;%dH%s%s%s%s\e[%d;%dH%s%s%s%s\n", toph+26, dist, str8, str0, str4, str11, toph+28, dist, str0, str12, str0, str13);
     printf("\e[%d;%dH%s%s%s%s%s%s%s%s%s%s%s%s\n", toph+24, dist, two, ten, str0, ten, cube, two, cube, str6, str5, five, str0, str4);
     printf("\e[%d;%dH%s%s%s%s%s\e[%d;%dH%s%s%s%s%s\n", toph+22, dist, cube, str5, str5, str5, str6, toph+29, dist, str14, str1, ten, cube, str15);
     printf("\e[%d;%dH%s%s%s%s%s%s\e[%d;%dH%s%s%s%s%s %s\n",toph+27, dist, str0, 
           five, str0, str9, str0, str10, toph+31, dist, ten, two, str2, str5, str5, str2);
     delete_all(str, str0, str1, str2, str3, str4, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15);
}
