// Copyright 2026 Luke Steuber. MIT License.
#include "tank.h"
static int mod(int x,int n){return (x%n+n)%n;}
TankTime tank_time(int hour,int minute,int second){
  hour=mod(hour,24);minute=mod(minute,60);second=mod(second,60);
  return (TankTime){hour%12?hour%12:12,minute/10,minute%10,second+1,hour>=12,hour>=7&&hour<19};
}
uint32_t tank_hash(uint32_t x){x^=x>>16;x*=0x7feb352dU;x^=x>>15;x*=0x846ca68bU;x^=x>>16;return x;}
TankFish tank_fish(int index,int count,int width,int height,bool round,uint32_t eighths){
  (void)height;int lane=index%4,column=index/4;
  int y=(round?47:42)+lane*(round?20:21);
  // Each fish has a lane segment, so even a twelve-fish school stays countable.
  int columns=(count+3)/4;if(columns<1)columns=1;
  int left=round?28:16,right=width-(round?28:16),span=(right-left)/columns;
  int travel=span-18;if(travel<2)travel=2;
  uint32_t phase=eighths/(3+(index%3))+tank_hash(index+count*13)%1000;
  int p=phase%(travel*2),x=left+column*span+9+(p<travel?p:travel*2-p);
  int bob=((eighths/6+index*19)%8);if(bob>4)bob=8-bob;
  return (TankFish){x,y+bob-2,p<travel?1:-1,index%3};
}
