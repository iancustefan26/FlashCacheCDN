#pragma once
#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <iostream>
#include <vector>
#include <curl/curl.h>


using namespace std;

size_t call_back(void* contents, size_t size, size_t nmemb, void* userp); // Needed for the CURL command

void clear_screen();

string get_public_ipv4();

string get_private_ipv4();

float get_cpu_load();

pair<float, float> get_memory_usage();

void get_available_resources(vector<string> &resources);

string get_machine_name();



#endif