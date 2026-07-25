#pragma once

void init_curl();
bool download_file(const char* url, const char* filename);
void end_curl();
