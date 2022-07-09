#include "Sekcija.h"

Sekcija::Sekcija(string s){
	ime = s;
}

void Sekcija::dodajUtabelu(string s){
	tabela += s;
}

string Sekcija::vratiKrajnjuAdresu(){
	int p = stoi(pocetnaAdresa, nullptr, 16);
	int x = tabela.size() / 2-1;
	x = x + p;
	char s[20];
	sprintf(s, "%X", x);
	string str = s;
	return str;
}

ostream& operator<<(ostream& os, const Sekcija& s){
	os << s.tabela;
	
	return os;
}
