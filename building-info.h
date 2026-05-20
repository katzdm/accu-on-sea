#ifndef _BUILDING_INFO_H_
#define _BUILDING_INFO_H_

#include <string>

#include "univ/formatter.h"


struct ObjectInfo {
  std::string name;
};

struct BuildingInfo : ObjectInfo {
  std::string address;
  std::string city;
  int zip;
  long phone;
  int year;
};

template <> struct std::formatter<ObjectInfo> : univ::formatter {};
template <> struct std::formatter<BuildingInfo> : univ::formatter {};

#endif
