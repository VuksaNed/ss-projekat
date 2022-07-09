#include "TabelaSekcija.h"
using namespace std;

TabelaSekcija::TabelaSekcija(string s, int si){
	sekcija = s;
	char broj[20];
	sprintf(broj, "%X", si);
	size = broj;
}

TabelaSekcija::TabelaSekcija(string s, string si){
	sekcija = s;
	size = si;
}

void TabelaSekcija::postaviPocetnuIKrajnju(int x, int par){
	if (x == -1) {
		x = 0;
		dalijekorektno = 0;
	}
	else
	dalijekorektno = par;
	char hex[20];
	sprintf(hex, "%X", x);
	pocetna = hex;
	int d = stoi(size, nullptr, 16);
	sprintf(hex, "%X", x+d-1);
	krajnja = hex;
}

void TabelaSekcija::postaviPocetnuIKrajnjuzaofset(int x){
	dalijekorektno = 1;
	char hex[20];
	sprintf(hex, "%X", x);
	pocetna = hex;
	int d = stoi(size, nullptr, 16);
	sprintf(hex, "%X", x + d-1);
	krajnja = hex;
}
