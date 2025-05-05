#pragma once

#ifdef PDF_MILL_EXPORTS
#define ICVS_DLL __declspec(dllexport)
#else
#define ICVS_DLL __declspec(dllimport)
#endif
