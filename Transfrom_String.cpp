#include "stdafx.h"
#include "Transfrom_String.h"

Transfrom_String::Transfrom_String()
{
}


Transfrom_String::~Transfrom_String()
{
}

string Transfrom_String::int_to_string(int input)
{
	char temp[8];
	string output;
	_itoa_s(input, temp, 10);
	output = temp;
	return output;
}

string Transfrom_String::float_to_string(float input)
{
	stringstream temp_sstream;
	temp_sstream << input;
	string out_put;
	temp_sstream >> out_put;
	temp_sstream.clear();
	return out_put;
}

/*doubleתstring*/
string Transfrom_String::double_to_string(double input)
{
	string out_put;
	char temp[20];
	sprintf_s(temp, "%f", input);
	out_put = temp;
	return out_put;
}
