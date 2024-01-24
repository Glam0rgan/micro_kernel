#pragma once


typedef struct OsMessageInfo_ {
  uint64_t label : 52;
  uint64_t capsUnwrapped : 3;
  uint64_t extraCaps : 2;
  uint64_t length : 7;
}OsMessageInfo;

