#ifndef WPROUTE_H
#define WPROUTE_H
#include "stdint.h"
#include "cstdint"
#include <string>
#include <iostream>
#include <fstream>

template<typename T>
class WPROUTE
{
public:
struct WP
{
float x; float y; float z;
float lat; float lng; float alt;
WP *next_ = nullptr;
WP *prev_ = nullptr;

void operator=(const WP& other)
{
x = other.x; lat = other.lat;
y = other.y; alt = other.alt;
z = other.z; lng = other.lng;
next_ = other.next_;
prev_ = other.prev_;
}
};

WP WPNULL = {0.0f, 0.0f, 0.0f, 0x00, 0x00};
WP *start_ = nullptr;
WP *nextWPpos = nullptr;

WPROUTE();

int routeLen = 0; int npc = 0;

void push_front(float lat, float alt, float lng)
{
  WP *newWP = new WP;
  newWP->lat = lat; newWP->alt = alt; newWP->lng = lng;

  uintptr_t addr = reinterpret_cast<uintptr_t>(start_);

  start_->next_ = reinterpret_cast<WP*>((!!addr) * reinterpret_cast<uintptr_t>(newWP));
  newWP->prev_ = reinterpret_cast<WP*>(addr);
  start_ = newWP;
  start_->x = longToFeet(start_->lng, start_->lat);
  start_->y = start_->alt;
  start_->z = latToFeet(start_->lat);
  routeLen++;
}

void pop_front()/*not nullptr safe*/
{
  WP temp = start_->prev_;
  delete start_;
  temp->next_ = nullptr;
  start_ = temp;
}

void safe_pop_front()
{
  if(start_!=nullptr)
    pop_front();
}

void clearall()
{
  while(start_->prev_!=nullptr)
  {
    pop_front();
  }
  delete start_;
}

void createRouteFromFile(std::string filename)
{
  /*READ ME: this func automates the creation of a WP route from a file.

  In order function properly, follow these instructions, and read closely:

  1. line 1 contains the number of waypoints to be created. this is the amout of x, y, and z pairs in the file. Put the number here, and hit enter. Make sure there are no spaces or other characters on this line other than the WP length number.
  2. Use '#' to indicate that the files wp are below. this line can be a comment line and is skipped by the parser.

  3. Creating a WP position:
    -each x, y, and z must be exactly 17 characters long. this includes a decimal point and negatation if needed. if your lat/long/alt is under 17 characters, include zeros to get to the 17 charatcer requirement. if your lat/long/alt is over 17 characters, YOU (not me, the programmer) have a problem.

    -for lat and long, remember to convert from DDMM.mmm to DD.DD format for GPS quards.

    -for alt, use feet not meters. and also if you are in higher elevation remember feet from sea level, not ground level

    - between every x, y, and z you must include one character. this character can be anything, but it must be there. this is to separate the x, y, and z values, and hopefully a bit more readable.

  4. after your very last z pos, use only the '#' character to indicate the end of the file. this is to make sure the parser knows when to stop reading the file. otherwise it will continue, and you will not be very happy. after the '#' character, you can put anything you want, it will be ignored by the parser too.
  */

  std::ifstream routefile;
  routefile.open(filename);

  std::string LEN = ""; 
  char ch = ' ';
  char chs[17];
  while(ch != '\n')
    {
      routefile>>ch;
      LEN += ch;
    }
  routeLen = std::stoi(LEN);

  routefile>>ch;
  while(ch!='\n'){routefile>>ch;}
  routefile>>ch;

  while(true)
    {
      routefile>>chs;
      float x = std::stof(chs);
      routefile>>ch;

      routefile>>chs;
      float y = std::stof(chs);
      routefile>>ch;

      routefile>>chs;
      float z = std::stof(chs);
      routefile>>ch;

      push_front(x, y, z);
      if(ch == '#'){break;}
    }
  
  routefile.close();
  nextWPpos = start_;
  while(nextWPpos->prev_!=nullptr)
    {
      nextWPpos = nextWPpos->prev_;
    }

  /*next WP pos is now at the start of the WP route. use nextWPpos to track current WP data*/
}
};

#endif
