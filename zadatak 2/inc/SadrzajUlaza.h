#pragma once
#include <string>
#include <list>
#include <fstream>
#include <iostream>

using namespace std;
class SadrzajUlaza{
public:
	string nazivFajla;
	list<string> sadrzaj;
	SadrzajUlaza(string n);
	friend ostream& operator<<(ostream& os, const SadrzajUlaza& s);
};

