#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include "Memorija.h"

using namespace std;

class Emulator{
public:
	void Emuliraj(ifstream& input);

	Memorija memorija;
	int r[8];/////r7-pc//////r6-sp
	bool psw[16];
	string poslednja;
	bool tajmer=false;
	bool terminal=false;
	clock_t vreme;
	bool kraj = true;

	void ucitajUlazniFajl(ifstream& input);
	void inicijalizuj();
	void ucitajMemMapReg();
	void ucitajPC(int adr);
	void azurirajTerminal();
	void izvrsavajProgram();
	void azurirajTajmer();
	string pswUString();
	void stringUPSW(string s);
	int dohvatiNacinAdresiranja(string op1, string op2);
	int dohvatiIzMemorijeDveSusedneAdrese(int x);

};
