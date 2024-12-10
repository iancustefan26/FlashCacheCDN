#pragma once
#ifndef USABLE_H
#define USABLE_H
#include <string>
#include <iostream>
#include <curl/curl.h>

using namespace std;

// TODO: rename the file to "utils"

size_t call_back(void* contents, size_t size, size_t nmemb, void* userp); // Usable for the CURL command

void clear_screen();

string get_public_ipv4();

string get_private_ipv4();

#endif