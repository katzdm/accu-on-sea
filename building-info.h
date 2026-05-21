#ifndef BUILDING_INFO_H_
#define BUILDING_INFO_H_

// Standard C++ headers.
#include <string>

// Application headers.
#include "univ/formatter.h"
#include "univ/formatter-old.h"


// 'BuildingInfo' demonstrates a type that inherits from a base class.

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

// The 'std::formatter' class is specialized, for both 'ObjectInfo' and
// 'BuildingInfo', using 'univ::formatter'.

template <typename CharT>
struct std::formatter<ObjectInfo, CharT> : univ::formatter {};

template <typename CharT>
struct std::formatter<BuildingInfo, CharT> : univ::formatter {};

#endif  // BUILDING_INFO_H_
