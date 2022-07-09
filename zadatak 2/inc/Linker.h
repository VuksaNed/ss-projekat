#pragma once
#include <iostream>
#include <list>
#include <fstream>
#include <string>
#include "SadrzajUlaza.h"
#include "Simbol.h"
#include "Sekcija.h"
#include "Relokacija.h"
#include "TabelaSekcija.h"

using namespace std;

class Linker{
public:
	void linkable(list<string> input, std::ofstream& output);
	void hexa(list<string> place,list<string> input, std::ofstream& output);

private:

	list<Simbol*> tabelaSimbola;
	list<Sekcija*> listasekcija;
	list<Relokacija*> Relokacioni_zapis;
	list<TabelaSekcija*> svesekcije;
	list<SadrzajUlaza> sadrzajUlaznihFajlova;
	int placesize;
	int maxadresa=-1;
	int stigaoplace = 0;
	string* placeplace;
	string* placesekcija;

	void otvoriFajlove(list<string> input);
	void obradiLinkable();
	void odstampajTabelu(ofstream& output);
	void dodajSveSekcije(TabelaSekcija* t);
	void dodajMasinskiKod(Sekcija* s);
	void daLiSeRedniBrojNalaziURelokaciji(list<Relokacija*> rz, int rb, int sa, string s, bool dodaj);
	bool daLiJeUTabeliSimbola(Simbol* s);
	int ofsetSekcije(string s);
	int dohvatiRedniBrojSekcije(string s);
	void relacionatabelaglobalispeglaj(list<Relokacija*> rz, int rb, int sa, int x);
	void placeProcitaj(list<string> place);
	void objediniSveSekcije();
	bool daLiJeSekcijaUSvimSekcijama(string s);
	int dohvatiPocetnuAdresuZaSekciju(string s);
	void dodajDrugeUlistuSvihSekcijaIsetujImAdrese(int i);
	void napraviTabeluSimbola();
	Simbol* dohvatiSimbolKojiJeIsti(Simbol* s);
	int dohvOfsetSekcijeIzFajla(int x, string s);
	void dodajMasinkiKodSaPocetnomAdresom(string s, int fajl, string pocadresa);
	void ispisiMaisnkiKodUhex(ofstream& output);
	void ispisimainskiuformatu(string pocetnaadresa, string s, ofstream& output);
	string dohvatiMaisnskiKod(int adresa, int velicina);
	void promeniMasinskiKod(int adresa, int velicina, string stavi);
	void razresiRelokacioniZapis();
	int dohvatiOfsetSekcije(string s);
	int dohvatiOfsetSekcijeUodnosuNaFajl(string s, int f);
	int dohvatiOfsetIzTabeleSimbolaZaFajlPoRednomBroju(int rb, int f);

	TabelaSekcija* dohvPoslednjuIzListe(list<TabelaSekcija*> s);

};

