#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <regex>
#include "Simbol.h"
#include "Sekcija.h"
#include "Relokacija.h"
#include "TabelaSekcija.h"

using namespace std;
class Asembler
{
public:
	void asembliraj(ifstream& input, ofstream& output);

private:

	int offset = 0;
	int absoffset = -1;
	string sekcija = "";
	list<string> ulazniFajl;
	list<Simbol *> tabelaSimbola;
	list<Sekcija *> listasekcija;
	list<Relokacija*> Relokacioni_zapis;
	list<TabelaSekcija*> svesekcije;

	void procitajIzFajla(ifstream& input);
	void prviProlaz();
	void DrugiProlaz();
	bool daLiJeVecUtabeli(string line);
	void daLiJeLabelaUlaz(string line);
	void obradiSekcijuPrviProlaz(string line);
	void obradiInstrukcijuPrviProlaz(string line);
	string skratistring(string line);
	bool daLiJeRegistar(string s);
	int daLiJeOperandSkoka(string s, int op);
	int daLiJeOperandPodatka(string s, int op);
	void obradiInstrukcijuDrugiProlaz(string line);
	void obradiSekcijuDrugiProlazi(string line);
	void nadjiUTabeliIstaviUglobal(string s);
	void dodajUTabeluSekcija(string s);
	void odstampajTabelecout();
	void odstampajTabeleout(ofstream& output);
	string obradiOperandPodataka(string s1, string s2, int x);
	void obradiWordLiteralISimbol(string s);
	string dohvatiSimbolPodatak(string s);
	string dohvatiSimbolPodatakPC(string s);
	string obradiOperandSkoka(string s2, int x);

	static string instrukcijeZaZaustavljanjeProcesora[];
	static string instrukcijeSoftverskogPrekida[];
	static string instrukcijePovratakaIzPrekidneRutine[];
	static string instrukcijePozivaPotprograma[];
	static string instrukcijePovratkaIzPotprograma[];
	static string instrukcijeSkoka[];
	static string instrukcijeSaStekom[];
	static string instrukcijeAtomicneZameneVrednosti[];
	static string instrukcijeAritmetickihOperacija[];
	static string instrukcijeLogickihOperacija[];
	static string instrukcijePomerackihOperacija[];
	static string instrukcijeUcitavanjaPodataka[];
	static string instrukcijeSmestanjaPodataka[];

};

///		g++ -o executable *.cpp
///		./executable