#include "Memorija.h"

void Memorija::dodajUmemoriju(MemJedinica* m){
	mem.push_back(m);
}

string Memorija::dohvatiSadrzajSaAdrese(int adr){
	int off = adr % 8;
	adr = adr - off;

	for (MemJedinica* m : mem) {
		int pocetna = stoi(m->adresa, nullptr, 16);
		if (pocetna == adr) {
			return m->sadrzaj[off];
		}
			
	}

	return "##";
}

void Memorija::upisiUMemorijuNaAdresi(int adr, string sadrzaj){
	int off = adr % 8;
	adr = adr - off;

	for (MemJedinica* m : mem) {
		int pocetna = stoi(m->adresa, nullptr, 16);
		if (pocetna == adr) {
			m->sadrzaj[off]=sadrzaj;
		}

	}
}
