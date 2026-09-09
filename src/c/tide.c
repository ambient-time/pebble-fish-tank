// Copyright 2026 Luke Steuber. MIT License.
#include "tide.h"
static int hex(char c){return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;}
bool tide_valid(const Tide*t){if(t->revision<0||t->enabled<0||t->enabled>1||t->at<0)return false;if(!t->at)return t->series[0]==0;if(!t->enabled||t->at%3600||t->series[98])return false;for(int i=0;i<98;i++)if(hex(t->series[i])<0)return false;return true;}
static int sample(const Tide*t,int i){return hex(t->series[i*2])*16+hex(t->series[i*2+1]);}
int tide_level(const Tide*t,int64_t now){if(!tide_valid(t)||!t->enabled||!t->at||now<t->at||now>(int64_t)t->at+48*3600)return -1;int age=(int)(now-t->at),i=age/3600;if(i==48)return sample(t,48);int a=sample(t,i),b=sample(t,i+1);return (a*(3600-age%3600)+b*(age%3600))/3600;}
int tide_surface(int level,bool round){if(level<0)return 8;if(level>255)level=255;return (round?60:59)-level*(round?34:43)/255;}
