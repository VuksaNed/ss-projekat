#pragma once
#include <string>
#include <list>

using namespace std;

class Memorija{
public:
	struct MemJedinica {
		string adresa;
		string sadrzaj[8];
	};

	list<MemJedinica*> mem;
	bool dosaoupis = false;

	void dodajUmemoriju(MemJedinica* m);
	string dohvatiSadrzajSaAdrese(int adr);
	void upisiUMemorijuNaAdresi(int adr, string sadrzaj);

};
