#pragma once
#include <array>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <set>
#include <iomanip>
#include <algorithm>
#include <ostream>
#include <string>



#ifdef PDF_MILL_EXPORTS
#define ICVS_DLL __declspec(dllexport)
#else
#define ICVS_DLL __declspec(dllimport)
#endif

#define PDF_MILL_VALID 1
#define PDF_MILL_INVALID 2

