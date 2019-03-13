// test.cpp : Defines the entry point for the console application.
//
#include "../include/hyyun.h"
using namespace hyyun;

const tchar*         host   = _T("192.168.2.4");
const PORT           port   = 20000;
hyc::ProtocolSupport ps     = hyc::ProtocolSupport_IPv4;

#include "01.demo_c.hpp"