//
// Created by 58226 on 2025/1/2.
//
#include "globalsignal.h"


GlobalSignal GlobalSignal::s_signalProxy;

GlobalSignal* GlobalSignal::instance()
{
    return &s_signalProxy;
}
