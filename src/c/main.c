// Copyright 2026 Luke Steuber. MIT License.
#include <pebble.h>
#include "tank.h"
#include "tide.h"
#include <string.h>
static Tide s_tide;
static Window *s_window;static Layer *s_layer;
static AppTimer *s_timer;static bool s_focused;static int s_burst;
static int s_w,s_h,s_lw,s_lh,s_last_second=-1;static uint32_t s_frames;
#ifdef PBL_ROUND
#define ROUND true
#else
#define ROUND false
#endif
#ifdef PBL_COLOR
#define C(hex) GColorFromHEX(hex)
#else
#define C(hex) GColorWhite
#endif
static int X(int x){return x*s_w/s_lw;}static int Y(int y){return y*s_h/s_lh;}
static void box(GContext *ctx,int x,int y,int w,int h,GColor c){
  graphics_context_set_fill_color(ctx,c);graphics_fill_rect(ctx,GRect(X(x),Y(y),X(x+w)-X(x),Y(y+h)-Y(y)),0,GCornerNone);
}
static void stroke(GContext *ctx,int x1,int y1,int x2,int y2,GColor c){
  graphics_context_set_stroke_color(ctx,c);graphics_context_set_stroke_width(ctx,1);
  graphics_draw_line(ctx,GPoint(X(x1),Y(y1)),GPoint(X(x2),Y(y2)));
}
static void sprite(GContext *ctx,TankFish f,bool pm,uint32_t frame){
  const char *rows[3][9]={
    {"     aa      ","    aabba    ","aa abbbbbba  ","aaaabbccbbda "," aabbbbbbbba ","aaaabbbbbba  ","aa  bbbba    ","     cc      ","             "},
    {"      a      ","     aab     ","aa  abbba    ","aabbbbbbbda  "," aaabbcbbbaa ","aabbbbbbbba  ","aa  bbbba    ","     bb      ","      c      "},
    {"             ","    aaa      ","aa abbbbaa   ","aabbbbbbbda  "," aacccccbbba ","aabbbbbbbba  ","aa  bbbbaa   ","     cc      ","             "}
  };
#ifdef PBL_COLOR
  GColor body=pm?(f.kind==1?C(0x55aaff):C(0x00ffff)):(f.kind==1?C(0xff5555):C(0xffaa00));
  GColor light=pm?C(0xaaffff):C(0xffffaa),dark=pm?C(0x0055aa):C(0xaa5500);
#else
  GColor body=GColorWhite,light=GColorWhite,dark=GColorBlack;(void)pm;
#endif
  for(int y=0;y<9;y++)for(int x=0;x<13;x++){
    char c=rows[f.kind][y][x];if(c==' ')continue;
    int sx=f.x+(x-6)*f.direction;if(x<2&&((frame/2)%2))sx+=f.direction;
    box(ctx,sx,f.y+y-4,1,1,c=='d'?GColorBlack:c=='a'?light:c=='c'?dark:body);
  }
}
static void kelp(GContext *ctx,int x,int base,int height,uint32_t frame,int seed){
  int sway=(int)((frame/5+seed)%6);if(sway>3)sway=6-sway;sway--;
  for(int y=0;y<height;y++){
    int dx=(y/7%2?1:-1)+sway*y/height;box(ctx,x+dx,base-y,2,1,C(0x00aa55));
    if(y%7==3){int side=(y/7+seed)%2?1:-1;box(ctx,x+dx+(side>0?1:-3),base-y-1,4,2,C(0x55ff55));}
  }
}
static void draw(Layer *layer,GContext *ctx){
  time_t epoch;uint16_t ms;time_ms(&epoch,&ms);struct tm t=*localtime(&epoch);
  TankTime parts=tank_time(t.tm_hour,t.tm_min,t.tm_sec);
  uint32_t phase=(uint32_t)(epoch%86400)*8+(s_burst?ms/125:0);
  int w=s_lw,h=s_lh,bed=h-(ROUND?29:17);
  int level=tide_level(&s_tide,(int64_t)epoch),surface=tide_surface(level,ROUND);
  graphics_context_set_antialiased(ctx,false);box(ctx,0,0,w,h,GColorBlack);
#ifdef PBL_COLOR
  box(ctx,0,0,w,h,parts.daylight?C(0x005555):C(0x000055));
  box(ctx,0,48,w,h-48,C(0x005555));box(ctx,0,112,w,h-112,C(0x000055));
  for(int i=0;i<7;i++){int x=10+i*23;for(int y=20;y<105;y+=3)if((y/3+i)%3==0)box(ctx,x+y/8,y,1,2,C(0x0055aa));}
#else
  for(int i=0;i<36;i++){uint32_t z=tank_hash(i+87);box(ctx,10+z%(w-20),30+(z>>8)%90,1,1,GColorWhite);}
#endif
  // Dry glass is empty; only a valid forecast draws a measurable surface.
  box(ctx,5,8,w-10,surface-8,GColorBlack);
  if(level>=0){stroke(ctx,9,surface,w-10,surface,C(0xaaffff));
    for(int x=12;x<w-12;x+=13)box(ctx,x,surface+2,7,1,C(0x55aaaa));}
  int castle=ROUND?w*65/100:w*72/100;
#ifdef PBL_COLOR
  GColor rock=C(0x555555),edge=C(0xaaaaaa);
#else
  GColor rock=GColorBlack,edge=GColorWhite;
#endif
  box(ctx,castle-15,bed-25,31,25,rock);
  box(ctx,castle-17,bed-29,10,4,edge);box(ctx,castle+8,bed-29,10,4,edge);
  box(ctx,castle-3,bed-35,7,11,rock);box(ctx,castle-5,bed-38,4,5,edge);box(ctx,castle+3,bed-38,4,5,edge);
  stroke(ctx,castle-15,bed-25,castle-15,bed,edge);stroke(ctx,castle+15,bed-25,castle+15,bed,edge);
  box(ctx,castle-4,bed-13,9,13,GColorBlack);box(ctx,castle-2,bed-16,5,3,GColorBlack);
  for(int row=0;row<3;row++)for(int j=0;j<4;j++)box(ctx,castle-13+j*8+(row%2)*2,bed-24+row*7,4,1,edge);
  for(int i=0;i<parts.tens;i++)kelp(ctx,(ROUND?35:19)+i*11,bed,30+(i%3)*5,phase,i);
  kelp(ctx,ROUND?w-39:w-19,bed,19,phase,8);kelp(ctx,ROUND?w-32:w-12,bed,14,phase,12);
  for(int i=0;i<parts.bubbles;i++){
    int x=castle-13+(i%6)*5+(int)((phase/8+i)%3)-1;
    int y=bed-40-((phase/3+i*17)%(ROUND?69:77));if(y<surface+5)continue;
    graphics_context_set_stroke_color(ctx,C(0x55aaaa));graphics_draw_rect(ctx,GRect(X(x),Y(y),X(2+(i%3==0)),Y(2+(i%3==0))));
    if(i%3==0)box(ctx,x,y,1,1,C(0xaaffff));
  }
  for(int i=0;i<parts.hour;i++){
    TankFish fish=tank_fish(i,parts.hour,w,h,ROUND,phase);
    if(level>=0)fish.y=surface+10+(fish.y-(ROUND?45:40))*(bed-16-surface-10)/70;
    sprite(ctx,fish,parts.pm,phase);
  }
#ifdef PBL_COLOR
  box(ctx,4,bed+2,w-8,h-bed-2,C(0x555500));
  for(int i=0;i<95;i++){
    uint32_t z=tank_hash(i*17+43);int x=6+z%(w-12),y=bed+2+(z>>9)%(h-bed-4);
    box(ctx,x,y,2+(z%2),1+(i%2),i%3==0?C(0xaa5500):i%3==1?C(0x555555):C(0xaaaa55));
  }
#else
  box(ctx,4,bed+2,w-8,h-bed-2,GColorBlack);
  for(int i=0;i<42;i++){uint32_t z=tank_hash(i*17+43);box(ctx,6+z%(w-12),bed+2+(z>>9)%(h-bed-4),1,1,GColorWhite);}
#endif
  for(int i=0;i<parts.ones;i++){
    int x=(ROUND?44:19)+i*7;
    box(ctx,x-1,bed+1,7,8,GColorBlack);box(ctx,x,bed+4,5,4,C(0xffaa00));
    box(ctx,x+1,bed+2,3,2,C(0xffff55));box(ctx,x+1,bed+3,1,1,GColorWhite);
  }
  if(ROUND){GPoint center=GPoint(s_w/2,s_h/2);
    graphics_context_set_stroke_width(ctx,X(4));graphics_context_set_stroke_color(ctx,C(0x555555));graphics_draw_circle(ctx,center,s_w/2-X(3));
    graphics_context_set_stroke_width(ctx,1);graphics_context_set_stroke_color(ctx,C(0xaaaaaa));graphics_draw_circle(ctx,center,s_w/2-X(6));
  }else{
    box(ctx,0,0,5,h,C(0x555555));box(ctx,w-5,0,5,h,C(0x555555));box(ctx,5,3,w-10,5,C(0xaaaaaa));
    box(ctx,5,h-5,w-10,5,C(0x555555));stroke(ctx,6,12,6,h-9,C(0xaaaaaa));stroke(ctx,w-7,12,w-7,h-9,C(0xaaaaaa));
    box(ctx,10,33,1,22,C(0x55aaaa));box(ctx,12,34,1,10,C(0x55aaaa));
  }
  s_frames++;
  if(s_last_second!=t.tm_sec){APP_LOG(APP_LOG_LEVEL_INFO,"Tank time=%02d:%02d:%02d fish=%d kelp=%d nuggets=%d frames=%lu heap=%lu",t.tm_hour,t.tm_min,t.tm_sec,parts.hour,parts.tens,parts.ones,(unsigned long)s_frames,(unsigned long)heap_bytes_free());s_last_second=t.tm_sec;}
  (void)layer;
}
static bool integer(DictionaryIterator*d,int key,int32_t*out){Tuple*t=dict_find(d,key);if(!t||(t->type!=TUPLE_INT&&t->type!=TUPLE_UINT)||t->length!=4)return false;*out=t->value->int32;return true;}
static void inbox(DictionaryIterator*d,void*context){(void)context;Tide next=s_tide;int32_t rev;if(!integer(d,MESSAGE_KEY_REVISION,&rev)||rev<0)return;
 if(dict_find(d,MESSAGE_KEY_ENABLED)){if(dict_find(d,MESSAGE_KEY_AT)||dict_find(d,MESSAGE_KEY_SERIES))return;if(rev!=next.revision)memset(&next,0,sizeof(next));next.revision=rev;if(!integer(d,MESSAGE_KEY_ENABLED,&next.enabled))return;if(!next.enabled){next.at=0;memset(next.series,0,sizeof(next.series));}}
 else{if(rev!=next.revision||!next.enabled||!integer(d,MESSAGE_KEY_AT,&next.at)||next.at<s_tide.at)return;Tuple*t=dict_find(d,MESSAGE_KEY_SERIES);if(!t||t->type!=TUPLE_CSTRING||t->length!=99)return;memcpy(next.series,t->value->cstring,99);if(next.series[98])return;}
 if(!tide_valid(&next))return;
 s_tide=next;persist_write_data(71,&s_tide,sizeof(s_tide));if(s_layer)layer_mark_dirty(s_layer);
 APP_LOG(APP_LOG_LEVEL_INFO,"Tide enabled=%ld level=%d",(long)s_tide.enabled,tide_level(&s_tide,time(NULL)));
}
static void request_tide(void){DictionaryIterator*d;if(app_message_outbox_begin(&d)==APP_MSG_OK){dict_write_int32(d,MESSAGE_KEY_REVISION,s_tide.revision);app_message_outbox_send();}}
static void cancel(void){if(s_timer){app_timer_cancel(s_timer);s_timer=NULL;}}
static void animate(void *context){s_timer=NULL;if(!s_focused||!s_burst)return;s_burst--;layer_mark_dirty(s_layer);if(s_burst)s_timer=app_timer_register(125,animate,NULL);(void)context;}
static void tap(AccelAxisType axis,int32_t direction){cancel();if(s_focused){s_burst=48;layer_mark_dirty(s_layer);s_timer=app_timer_register(125,animate,NULL);} (void)axis;(void)direction;}
static void tick(struct tm *t,TimeUnits units){if(s_focused&&!s_burst)layer_mark_dirty(s_layer);if(t->tm_min%30==0&&t->tm_sec==0)request_tide();(void)units;}
static void focus(bool focused){s_focused=focused;if(focused)layer_mark_dirty(s_layer);else{cancel();s_burst=0;}}
static void load(Window *window){GRect b=layer_get_bounds(window_get_root_layer(window));s_w=b.size.w;s_h=b.size.h;s_lw=ROUND?180:144;s_lh=ROUND?180:168;
  s_layer=layer_create(b);layer_set_update_proc(s_layer,draw);layer_add_child(window_get_root_layer(window),s_layer);}
static void unload(Window *w){cancel();layer_destroy(s_layer);(void)w;}
static void init(void){Tide saved;if(persist_get_size(71)==sizeof(saved)&&persist_read_data(71,&saved,sizeof(saved))==sizeof(saved)&&tide_valid(&saved))s_tide=saved;app_message_register_inbox_received(inbox);app_message_open(256,64);s_window=window_create();window_set_window_handlers(s_window,(WindowHandlers){.load=load,.unload=unload});window_stack_push(s_window,false);tick_timer_service_subscribe(SECOND_UNIT,tick);accel_tap_service_subscribe(tap);app_focus_service_subscribe(focus);focus(true);}
static void deinit(void){app_message_deregister_callbacks();s_focused=false;cancel();tick_timer_service_unsubscribe();accel_tap_service_unsubscribe();app_focus_service_unsubscribe();window_destroy(s_window);}
int main(void){init();app_event_loop();deinit();}
