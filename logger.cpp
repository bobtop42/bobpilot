#include "logger.h"

/* WORK ON THIS ONE LATER
void LOGGER::setFilename(std::string filetype)
{
  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  std::tm local_tm = *std::localtime(&now_time);

  std::string time = static_cast<std::string>(std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S") + filetype);
}
*/

void LOGGER::log(PLANE* plane, std::string filename, int num)
{
  std::ofstream o(filename);

  o<<"$";
  //plane angle data
  o<<plane->pAngle[0][0]<<"\t"<<plane->pAngle[0][1]<<"\t"<<plane->pAngle[0][2]<<std::endl;
  o<<plane->pAngle[1][0]<<"\t"<<plane->pAngle[1][1]<<"\t"<<plane->pAngle[1][2]<<std::endl;
  o<<plane->pAngle[2][0]<<"\t"<<plane->pAngle[2][1]<<"\t"<<plane->pAngle[2][2]<<std::endl;

  //pitch and roll data
  o<<plane->PA.pitch<<"\t"<<plane->PA.roll<<std::endl;
  o<<"\v";
  
  //current waypoint data
  o<<plane->WPXYZ[plane->npc][0]<<"\t"<<plane->WPXYZ[plane->npc][1]<<"\t"<<plane->WPXYZ[plane->npc][2]<<std::endl;

  //waypoint angle data
  o<<plane->WPA.pitch<<"\t"<<plane->WPA.roll<<std::endl;
  o<<"\v";

  //flap pos. data
  o<<plane->ep.elevator.fL<<"\t"<<plane->ep.elevator.fR<<"\t\t"<<plane->ap.aileron.fL<<"\t"<<plane->ap.aileron.fR<<std::endl;
  o<<"\v\v";
  //UTC time (from satalite atomic clock)
  o<<plane->time.hrs_<<":"<<plane->time.min_<<":"<<plane->time.sec_<<" UTC"<<std::endl;
  //plane lat/long and alt
  o<<plane->loc.x<<"\t"<<plane->loc.y<<"\t"<<plane->loc.z<<std::endl;
  //gps accuracy
  o<<"-"<<plane->gpsAcc<<std::endl;
  //hemisphere data
  o<<"-"<<plane->hemisphere.NS<<"\t"<<plane->hemisphere.EW<<std::endl;
  o<<"\v\v"<<std::endl;
  //speed data
  //airspeed
  /*
  o<<plane-> FINISH PLANE!!!!!
  */
}

auto LOGGER::parseLog(const std::string data, int logPos[67])
{
  float pALog[3][3];

  pALog[0][0] = floatPositionReturn(data, logPos, 0);
  pALog[0][1] = floatPositionReturn(data, logPos, 2);
  pALog[0][2] = floatPositionReturn(data, logPos, 4);
  pALog[1][0] = floatPositionReturn(data, logPos, 6);
  pALog[1][1] = floatPositionReturn(data, logPos, 8);
  pALog[1][2] = floatPositionReturn(data, logPos, 10);
  pALog[2][0] = floatPositionReturn(data, logPos, 12);
  pALog[2][1] = floatPositionReturn(data, logPos, 14);
  pALog[2][2] = floatPositionReturn(data, logPos, 16);

  float PApitch = floatPositionReturn(data, logPos, 18);
  float PAroll = floatPositionReturn(data, logPos, 20);

  float WPX = floatPositionReturn(data, logPos, 22);
  float WPY = floatPositionReturn(data, logPos, 24);
  float WPZ = floatPositionReturn(data, logPos, 26);

  int UTChrs = intPositionReturn(data, logPos, 44);
  int UTCmin = intPositionReturn(data, logPos, 45);
  int UTCsec = intPositionReturn(data, logPos, 46);

  float pX = floatPositionReturn(data, logPos, 47);
  float pY = floatPositionReturn(data, logPos, 49);
  float pZ = floatPositionReturn(data, logPos, 51);

  int gpsAcc = intPositionReturn(data, logPos, 53);

  char NS = charPositionReturn(data, logPos, 54);
  char EW = charPositionReturn(data, logPos, 55);

  float speed = floatPositionReturn(data, logPos, 58);

  float accX = floatPositionReturn(data, logPos, 60);
  float accY = floatPositionReturn(data, logPos, 62);
  float accZ = floatPositionReturn(data, logPos, 64);

  int num = std::stoi(data.substr(logPos[66]+1, (logPos[67]-logPos[66])));
  
  num = intPositionReturn(data, logPos, 67);

  

  //ADD NPC COUNTER DATA!!!
  int npc = 0; // Initialize npc variable
  return std::make_tuple(pALog, PApitch, PAroll, WPX, WPY, WPZ, UTChrs, UTCmin, UTCsec, pX, pY, pZ, gpsAcc, NS, EW, speed, accX, accY, accZ, npc, num);

}

auto LOGGER::fillPlaneData(const std::string data, int *logPos, PLANE *plane)
{
  auto [pALog, PApitch, PAroll, WPX, WPY, WPZ, UTChrs, UTCmin, UTCsec, pX, pY, pZ, gpsAcc, NS, EW, speed, accX, accY, accZ, npc, num] = parseLog(data, logPos);

  for(int i=0; i<3; ++i)
    {
      for(int j=3; j<3; ++j)
        {
          plane->pAngle[i][j] = pALog[i][j];
        }
    }

  plane->WPXYZ[npc][0] = WPX;
  plane->WPXYZ[npc][1] = WPY;
  plane->WPXYZ[npc][2] = WPZ;

  plane->loc.x = pX;
  plane->loc.y = pY;
  plane->loc.z = pZ;

  //gps acc
  plane->gpsAcc = gpsAcc;

  //hemisphere
  plane->hemisphere.NS = NS;
  plane->hemisphere.EW = EW;

  //speed
  plane->speed = speed;

  plane->AC[0] = accX;
  plane->AC[1] = accY;
  plane->AC[2] = accZ;

  plane->npc = npc;

  //work on this later
  plane->time.updateTime(UTChrs, UTCmin, UTCsec, plane);
  
}
