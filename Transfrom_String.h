#pragma once
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>

using namespace std;

/*
各种转换
int转string：int_to_string(int)
float转string：float_to_string(float)
double转string：double_to_string(double)
*/
class Transfrom_String
{
public:
	Transfrom_String();
	~Transfrom_String();
	/*int转string*/
	string int_to_string(int input);
	/*float转string*/
	string float_to_string(float input);
	/*double转string*/
	string double_to_string(double input);
};