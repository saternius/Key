
#include "pebble.h"
#include <math.h>
#include <time.h>
#define PI 3.14159265
#define ACCEL_STEP_MS 50
#define FRICTION = .95;
typedef enum {TOP, MIDDLE, BOTTOM} floors;

//GRAPHIC PATHS
static const GPathInfo WHITE_BACKGROUND = {
  .num_points = 4,
  .points = (GPoint []) {{0, 0}, {180, 0}, {180, 180}, {0, 180}}
};

static const GPathInfo SQUARE_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, 0}, {10, 0}, {10, 10}, {0, 10}}
};

static const GPathInfo BAR_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, 0}, {180, 0}, {180, 50}, {0, 50}}
};


GPathInfo STAR_INFO_0 = {
  .num_points = 10,
  .points = (GPoint []) {
    {0,5},{5,5},{7,0},{9,5},{14,5},{10,8},{12,14},{7,10},{3,13},{4,8}
  }
};

GPathInfo STAR_INFO_1 = {
  .num_points = 10,
  .points = (GPoint []) {
    {1,3},{6,5},{9,0},{9,6},{14,7},{10,9},{9,14},{6,10},{2,12},{4,7}
  }
};

GPathInfo STAR_INFO_2 = {
  .num_points = 10,
  .points = (GPoint []) {
    {2,2},{6,4},{11,1},{10,6},{14,9},{9,9},{8,14},{5,9},{0,10},{4,6}
  }
};

GPathInfo STAR_INFO_3 = {
  .num_points = 10,
  .points = (GPoint []) {
   {4,0},{7,4},{12,2},{10,7},{13,10},{9,10},{6,14},{5,9},{0,8},{4,5}
  }
};

static GPathInfo SMALL_BLOB_1 = {
  .num_points = 4,
  .points = (GPoint []) {
    {0,0},{6,0},{6,6},{0,6}
  }
};

static GPathInfo SMALL_BLOB_2 = {
  .num_points = 4,
  .points = (GPoint []) {
    {0,5},{5,0},{5,5},{0,5}
  }
};

static GPathInfo SMALL_BLOB_3 = {
  .num_points = 4,
  .points = (GPoint []) {
   {0,0},{4,0},{4,4},{0,4}
  }
};




static Window *window;
static TextLayer *menu_text;
static TextLayer *menu_sub_text;

static int GAME_TIMER = 0;

Layer *window_layer;
bool menu = true;

static AppTimer *timer;
int y_grav = 0;
bool pos = true;
double time_elapsed = 0;

//game variables
int box_x = -1;
int box_y = 60;
int box_rotation;
int box_dir = 3;
bool box_black = true;
static int star_frame = 0;
floors box_floor = 0;

char death_disp[12];


static char* nar[45];
static int text_index = 0;

static GPath *s_my_path_ptr = NULL;
static const GPathInfo BOLT_PATH_INFO = {
  .num_points = 6,
  .points = (GPoint []) {{21, 0}, {14, 26}, {28, 26}, {7, 60}, {14, 34}, {0, 34}}
};



static GPath *bg = NULL;
static GPath *square = NULL;
static GPath *bar1 = NULL;


static GPath *stars[10];
static int stars_x[10];
static int stars_y[10];
static bool stars_black[10];
static bool star_visible[10];



GPathInfo SMALL_BLOB[3];
GPathInfo STAR_INFO[4];
GPath* smallBlobs[3]; 

struct particlePlayer{
    bool black;
    int deathTimer;
    int blobIndex[7];
    double p_x[7];
    double p_y[7];
    double xVel[7];
    double yVel[7];
    GPath* part[7];
};
typedef struct particlePlayer ParticlePlayer;
ParticlePlayer spark;

void initialize_spark(bool b){
      spark.black = b;
      spark.deathTimer = 50;
      for(int i = 0; i<7; i++){
        spark.blobIndex[i] = i%3;
        spark.part[i] = smallBlobs[spark.blobIndex[i]];
        spark.p_x[i] = (box_x + (rand() % 10 - 5));
        spark.p_y[i] = (box_y + (rand() % 10 -5));
        spark.xVel[i] = (8.0*rand()/RAND_MAX - 4.0);
        spark.yVel[i] = (-12.5*rand()/RAND_MAX);
        
      }
}

void destroy_spark(){
  //deallocate all your shit from spark
}

// .update_proc of my_layer:
void window_layer_update_callback(Layer *my_layer, GContext* ctx) {
  
  // Draw BG
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, bg);
  // Draw Bar
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, bar1);
  //Draw Deaths
  snprintf(death_disp, sizeof death_disp, "%s%d", "Deaths: ", GAME_TIMER);
  text_layer_set_text(menu_sub_text,death_disp);
  
  //draw floor 2
  gpath_draw_filled(ctx, bar1);
  
  //draw box
  if(box_black){
    graphics_context_set_fill_color(ctx, GColorBlack);
  }else{
    graphics_context_set_fill_color(ctx, GColorWhite);
  }
   gpath_draw_filled(ctx, square);
  
  //draw particles
  if(spark.deathTimer>0){
    spark.deathTimer--;
    if(spark.black){
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_fill_color(ctx, GColorWhite);
    }
    for(int i =0; i<7; i++){
      spark.xVel[i]*=.95;
      spark.yVel[i]++;
      spark.p_x[i]+=spark.xVel[i];
      spark.p_y[i]+=spark.yVel[i];
      gpath_draw_filled(ctx, spark.part[i]);
      gpath_move_to(spark.part[i], GPoint((int)spark.p_x[i], (int)spark.p_y[i]));
    }
  }
   
  
  //draw stars
  //have a for loop in the future
  for(int i=0; i<5; i++){
    if(stars[i] && star_visible[i]){
      if(stars_black[i]){
        graphics_context_set_fill_color(ctx, GColorBlack);
      }else{
        graphics_context_set_fill_color(ctx, GColorWhite);
      }
      gpath_draw_filled(ctx, stars[i]);
    }
  }
 
}
void setup_my_path(void) {
  s_my_path_ptr = gpath_create(&BOLT_PATH_INFO);
  // Rotate 15 degrees:
  gpath_rotate_to(s_my_path_ptr, TRIG_MAX_ANGLE / 360 * 15);
  // Translate by (5, 5):
  gpath_move_to(s_my_path_ptr, GPoint(18, 18));
}
void draw_stage(){
  stars_x[0] = 120;
  stars_y[0] = 57;
  stars_black[0] = true;
  star_visible[0] = true;
  
  stars_x[1] = 7;
  stars_y[1] = 105;
  stars_black[1] = false;
  star_visible[1] = true;
  
  stars_x[2] = 120;
  stars_y[2] = 155;
  stars_black[2] = true;
  star_visible[2] = true;
  
  
  
   bg = gpath_create(&WHITE_BACKGROUND);
   bar1 = gpath_create(&BAR_INFO);
   gpath_move_to(bar1, GPoint(-2, 70));
   square = gpath_create(&SQUARE_INFO);
   gpath_move_to(square, GPoint(box_x, box_y));
  
   //stars should have there own meathod in the future.
  for(int i=0; i<5;i++){
    if(stars[i]){
      gpath_destroy(stars[i]);
    }
     stars[i] = gpath_create(&STAR_INFO[0]);
     gpath_move_to(stars[i], GPoint(stars_x[i], stars_y[i]));
  }
}



void nextText(){
      text_index++;
      text_layer_set_text(menu_text, nar[text_index]);
      if(box_floor == TOP){
        text_layer_set_text_color(menu_text, GColorBlack);//change the text to black
        layer_set_frame(text_layer_get_layer(menu_text),(GRect) { .origin = { 0, 19 }, .size = { 180, 17 } });//set top
      }else if(box_floor == MIDDLE){
         text_layer_set_text_color(menu_text, GColorWhite);//change the text to white
         layer_set_frame(text_layer_get_layer(menu_text),(GRect) { .origin = { 0, 70 }, .size = { 180, 17 } });//set middle
      }else{
         text_layer_set_text_color(menu_text, GColorBlack);//change the text to black
         layer_set_frame(text_layer_get_layer(menu_text),(GRect) { .origin = { 0, 120 }, .size = { 180, 17 } });//set bottom
      }
}


static void timer_callback(void *data) {
  //enterframe code
  GAME_TIMER++;
  if(GAME_TIMER%3 == 0 ){
    star_frame++;
    star_frame = star_frame%4;
    for(int i=0; i<5; i++){
      if(stars[i]){
        gpath_destroy(stars[i]);
        stars[i] = gpath_create(&STAR_INFO[star_frame]);
        gpath_move_to(stars[i], GPoint(stars_x[i], stars_y[i]));
      }
    }
  }
  
  for(int i = 0; i<10; i++){
    //lets hit_Test
    int box_centerX = box_x+5;
    int box_centerY = box_y+5;
    int star_width = 18;
    int star_height = 18;
    if(star_visible[i] && box_centerX>stars_x[i] && box_centerX<stars_x[i]+star_width && box_centerY>stars_y[i] && box_centerY<stars_y[i]+star_height){
      APP_LOG(APP_LOG_LEVEL_DEBUG,"IT HIT!!!");
      star_visible[i] = false;
      initialize_spark(box_black);
    }
  }
  
  box_x+= box_dir;
  if(box_x>155 && box_floor == TOP){
    box_floor = MIDDLE;
    box_dir*=-1;
    box_y = 110;
    box_black = false;
    nextText();
  }
  if(box_x<-10 && box_floor == MIDDLE){
    box_floor = BOTTOM;
    box_dir*=-1;
    box_y = 157;
    box_black = true;
    nextText();
  }
  
  gpath_move_to(square, GPoint(box_x, box_y));
  
  //trigger a redraw and setup the loop.
  layer_mark_dirty(window_layer);
  timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

void middle_click_handler(ClickRecognizerRef recognizer, void *context) {
   if(menu){
     
     //lets change the display of the textlayers
      text_layer_destroy(menu_text); //no need for the big title
      menu_text = text_layer_create(layer_get_bounds(window_layer));//lets make it a new text_layer of modest size
      text_layer_set_text_alignment(menu_text, GTextAlignmentLeft);
      text_layer_set_text(menu_text, nar[0]);
      text_layer_set_background_color(menu_text, GColorClear);//change text bgColor to clear
     
      layer_set_frame(text_layer_get_layer(menu_text),(GRect) { .origin = { 0, 19 }, .size = { 180, 17 } });
      layer_add_child(window_layer, text_layer_get_layer(menu_text));//lets add it to the window now
     
      snprintf(death_disp, sizeof death_disp, "%s%d", "Deaths: ", GAME_TIMER);
      text_layer_set_text(menu_sub_text,death_disp);//make the subtitle the death counter
      text_layer_set_text_color(menu_sub_text, GColorWhite);//change the text to white
      text_layer_set_background_color(menu_sub_text, GColorBlack);//change text bgColor to black
      layer_set_frame(text_layer_get_layer(menu_sub_text),(GRect) { .origin = { 0, 0 }, .size = { 200, 20 } });//move the subtitle
      menu = false;//no longer the menu
      timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);//initialize the gameloop
     
       draw_stage();
       setup_my_path();
       layer_set_update_proc(window_layer, window_layer_update_callback);
       
     
   }
}
void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, middle_click_handler);
}

static void init() {
  srand(time(NULL));
  
  APP_LOG(APP_LOG_LEVEL_DEBUG,"Initialized..");
  //initializes everything and makes the menu screen.
  window = window_create();
  window_set_fullscreen(window, true);
  window_stack_push(window, true /* Animated */);
  window_set_click_config_provider(window, config_provider);
  window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  
  GFont menu_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FAP_50));
  //you should free this memory at some time.
  menu_text = text_layer_create(bounds);
  menu_sub_text = text_layer_create(bounds);

  text_layer_set_text(menu_text, "KEY");
  text_layer_set_text(menu_sub_text,"Press to Start");
  text_layer_set_font(menu_text, menu_font);
  text_layer_set_text_alignment(menu_text, GTextAlignmentCenter);
  text_layer_set_text_alignment(menu_sub_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(menu_text));
  layer_add_child(window_layer, text_layer_get_layer(menu_sub_text));
  
  layer_set_frame(text_layer_get_layer(menu_text),(GRect) { .origin = { -15, 39 }, .size = { 180, 180 } });
  layer_set_frame(text_layer_get_layer(menu_sub_text),(GRect) { .origin = { -15, 100 }, .size = { 180, 180 } });
  //lets initialize some things now shall we
  STAR_INFO[0] = STAR_INFO_0;
  STAR_INFO[1] = STAR_INFO_1;
  STAR_INFO[2] = STAR_INFO_2;
  STAR_INFO[3] = STAR_INFO_3;
  SMALL_BLOB[0] = SMALL_BLOB_1;
  SMALL_BLOB[1] = SMALL_BLOB_2;
  SMALL_BLOB[2] = SMALL_BLOB_3;
  smallBlobs[0] = gpath_create(&SMALL_BLOB[0]);
  smallBlobs[1] = gpath_create(&SMALL_BLOB[1]);
  smallBlobs[2] = gpath_create(&SMALL_BLOB[2]);
  nar[0] = "HIT THE STAR";
  nar[1] = "MOVIN RIGHT TO LEFT";
  nar[2] = "MIDDLE BTN TO JUMP"; 
  
}

static void deinit() {
  window_destroy(window);
  text_layer_destroy(menu_text);
  text_layer_destroy(menu_sub_text);
  destroy_spark();
  //get to deallocing the GPaths.
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}