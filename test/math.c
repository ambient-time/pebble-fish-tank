// Copyright 2026 Luke Steuber. MIT License.
#include "../src/c/tank.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/c/tide.h"
int main(void){
  for(int h=0;h<24;h++)for(int m=0;m<60;m++)for(int s=0;s<60;s++){
    TankTime t=tank_time(h,m,s);assert(t.hour>=1&&t.hour<=12);assert(t.tens*10+t.ones==m);
    assert(t.bubbles==s+1);assert(t.pm==(h>=12));
  }
  assert(tank_time(23,59,59).hour==11);assert(tank_time(24,60,60).hour==12);
  assert(tank_time(12,0,0).pm);assert(!tank_time(0,0,0).pm);
  for(int round=0;round<2;round++)for(int count=1;count<=12;count++)for(int i=0;i<count;i++)for(int f=0;f<4800;f+=7){
    int width=round?180:144;TankFish fish=tank_fish(i,count,width,round?180:168,round,f);
    assert(fish.x>=16&&fish.x<=width-16);assert(fish.y>=35&&fish.y<=115);
    assert(fish.direction==1||fish.direction==-1);
  }
  Tide tide={.enabled=1,.at=1800000000};
  for(int i=0;i<49;i++){tide.series[i*2]=i%2?'f':'0';tide.series[i*2+1]=i%2?'f':'0';}
  assert(tide_valid(&tide));assert(tide_level(&tide,tide.at)==0);
  assert(tide_level(&tide,tide.at+1800)==127);assert(tide_level(&tide,tide.at+3600)==255);
  assert(tide_level(&tide,tide.at-1)==-1);assert(tide_level(&tide,(int64_t)tide.at+48*3600+1)==-1);
  assert(tide_surface(0,false)>tide_surface(255,false));
  tide.series[30]='!';assert(!tide_valid(&tide));assert(tide_level(&tide,tide.at)==-1);
  memset(&tide,0,sizeof(tide));assert(tide_valid(&tide));assert(tide_level(&tide,1800000000)==-1);
  puts("Fish Tank: daily encodings, rollovers and bounded fish motion pass.");
}
