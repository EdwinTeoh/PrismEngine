// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <map>
#include <set>
#include <string>
#include <memory>
#include <exception>
#include <queue>
#include <functional>
#include <unordered_set>

// Vector Library
#include <iostream>			// cout, endl
#include <cmath>			// sqrt
#include <algorithm>		// transform, copy
#include <array>			// array
#include <random>			// RNG

#include "Math/Vector2D.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/Matrix4x4.h"