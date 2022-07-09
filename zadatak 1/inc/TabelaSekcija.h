#pragma once
#include <string>

using namespace std;
class TabelaSekcija{
public:
	string sekcija;
	int fajl = -1;
	string size;
	string pocetna="";
	string krajnja="";
	int ofsetic = 0;
	int dalijekorektno = 1;
	TabelaSekcija(string s, int si);
	TabelaSekcija(string s, string si);
	void postaviPocetnuIKrajnju(int x, int par);
	void postaviPocetnuIKrajnjuzaofset(int x);
};

