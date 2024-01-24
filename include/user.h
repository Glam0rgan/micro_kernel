#pragma once

void test();

void send(OsCPtr dest, OsMessageInfo msgInfo);

void receive(OsCPtr dest, OsMessageInfo* msgInfo);

void print(char* fmt, ...);
