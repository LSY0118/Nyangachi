#pragma once
#pragma comment(lib, "ws2_32.lib")

#include "targetver.h"

#include <stdio.h>
#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <Windows.h>
#include <vector>
#include <assert.h>
#include <concurrent_vector.h>
#include <list>
#include <set>

using namespace std;
using namespace concurrency;