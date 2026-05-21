#ifndef BUILDING_INFO_H_
#define BUILDING_INFO_H_

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

template <typename CharT>
struct std::formatter<ObjectInfo, CharT> : univ::formatter {};

template <typename CharT>
struct std::formatter<BuildingInfo, CharT> : univ::formatter {};

#endif  // BUILDING_INFO_H_
