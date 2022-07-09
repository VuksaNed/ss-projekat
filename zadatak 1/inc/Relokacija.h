#pragma once
#include <string>
#include <iostream>
using namespace std;
class Relokacija{
public:
	string sekcija;
	int ofset;
	string tip_relokacije;
	int addend;
	int rb;
	int x = -1;

	Relokacija(string sek, int of, string tip, int add, int r);
	Relokacija(string sek, string of, string tip, string add, string r);
	friend ostream& operator<<(ostream& os, const Relokacija& s);

};

