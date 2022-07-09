#include "Simbol.h"

int Simbol::redniBrojj = 0;

Simbol::Simbol(string labelaa, string sekcijaa, int ofsett, string lokall){
	labela = labelaa;
	sekcija = sekcijaa;
	ofset = ofsett;
	lokal = lokall;
	redniBroj = redniBrojj++;
}

Simbol::Simbol(string labelaa, string sekcijaa, string ofsett, string lokall){
	labela = labelaa;
	sekcija = sekcijaa;
	ofset = stoi(ofsett);
	lokal = lokall;
	redniBroj = redniBrojj++;
}

Simbol::Simbol(string labelaa, string sekcijaa, string ofsett, string lokall, string rb){
	labela = labelaa;
	sekcija = sekcijaa;
	ofset = stoi(ofsett);
	lokal = lokall;
	redniBroj = stoi(rb);
}

ostream& operator<<(ostream& os, const Simbol& s){
	os << s.labela << "\t" << s.sekcija << "\t" << s.ofset << "\t"<< s.lokal << "\t" << s.redniBroj << endl;
	return os;
}
