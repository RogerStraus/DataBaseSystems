#ifndef PRINT_H
#define PRINT_H
#include <iostream>
#include <cstdio>
#include "write.h"
#include "catalog.h"
#include "change_back_to_json.h"
#include "my_string.h"
#include "my_vector.cpp"
#include <string>
#include <fstream>

void print(FILE *in, FILE *out, catalog* CATALOG);

#endif