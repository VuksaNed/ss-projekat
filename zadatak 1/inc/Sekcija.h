#pragma once
#include <string>
#include <list>
#include "Simbol.h"

using namespace std;

class Sekcija{
public:
	string ime;
	string tabela;
	string pocetnaAdresa="";
	Sekcija(string s);
	void dodajUtabelu(string s);
	string vratiKrajnjuAdresu();
	friend ostream& operator<<(ostream& os, const Sekcija& s);

};

