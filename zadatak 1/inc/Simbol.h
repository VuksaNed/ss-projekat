#pragma once
#include <string>
#include <iostream>
#include <list>

using namespace std;

class Simbol{
public:

	struct RedniBr
	{
		int starirednibroj;
		int fajl;
	};

	string labela;
	string sekcija;
	int ofset;
	string lokal;
	int redniBroj;
	list<RedniBr> satrirednibroj;
	static int redniBrojj;

	Simbol(string labelaa, string sekcijaa, string ofsett, string lokall);
	Simbol(string labelaa,string sekcijaa,int ofsett,string lokall);
	Simbol(string labelaa, string sekcijaa, string ofsett, string lokall, string rb);
	friend ostream& operator<<(ostream& os, const Simbol& s);
	

};

