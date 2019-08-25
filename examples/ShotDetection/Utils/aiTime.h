#ifndef AISAAC_TIME
#define AISAAC_TIME

namespace aisaac {

  struct Time{
  		int hours;
  		int minutes;
  		int seconds;
  };

  inline Time getTime(int seconds)
  {
      Time t;
      t.hours = (int) seconds/3600;
      t.minutes = (int) (seconds % 3600)/60;
      t.seconds = seconds%60;
      return t;
  }
}

#endif
