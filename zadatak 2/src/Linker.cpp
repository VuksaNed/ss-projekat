#include "Linker.h"
#include <iterator>

void Linker::linkable(list<string> input, std::ofstream& output){
	
	otvoriFajlove(input);
	obradiLinkable();
	odstampajTabelu(output);

}

void Linker::hexa(list<string> place, list<string> input, std::ofstream& output){

	otvoriFajlove(input);
	placeProcitaj(place);
	objediniSveSekcije();
	napraviTabeluSimbola();
	razresiRelokacioniZapis();

	ispisiMaisnkiKodUhex(output);
	

}

void Linker::otvoriFajlove(list<string> input){

	for (string in : input) {
		ifstream ifstr(in);
		
		if (!ifstr.is_open()) {
			cout << "Nije bilo moguce otvoriti fajl " << in << endl;
			exit(1);
		}
		SadrzajUlaza s(in);
		string line;
		while (getline(ifstr, line)) {
			s.sadrzaj.push_back(line);
		}
		sadrzajUlaznihFajlova.push_back(s);
		ifstr.close();
	}
	
}

void Linker::obradiLinkable(){

	for (SadrzajUlaza sadrzaj : sadrzajUlaznihFajlova) {
		list<string> listasadrzaja = sadrzaj.sadrzaj;
		list<Simbol*> ts;
		list<Sekcija*> ls;
		list<Relokacija*> rz;
		list<TabelaSekcija*> svesek;
		int x = 0;
		string masniski = "";
		for (string s : listasadrzaja) {
			int prvo = s.find_first_not_of(char(13));
			int poslednje = s.find_last_not_of(char(13));
			if ((prvo != -1) && (poslednje != -1))s = s.substr(prvo, poslednje - prvo + 1);
			if (s!="")
			if (s == "Sve sekcije") x = 1;
			else {
				if (s=="Tabela simbola:") {
					x = 2;
				}else
				if (s=="Masinski kod:") {
					x = 3;
				}
				else {
					if (s == "Relokacioni zapis:") {
						x = 4;
					}
					else {
						switch (x)
						{
						case 1: {
							int pom = s.find("  ");
							svesek.push_back( new TabelaSekcija(s.substr(0,pom),s.substr(pom+2,s.size()-pom-2)));
							break;
						}
						case 2: {
							string s1, s2, s3, s4, s5;
							int pom = s.find('\t');
							s1 = s.substr(0, pom);
							s = s.substr(pom + 1, s.size() - pom - 1);
							pom = s.find('\t');
							s2 = s.substr(0, pom);
							s = s.substr(pom + 1, s.size() - pom - 1);
							pom = s.find('\t');
							s3 = s.substr(0, pom);
							s = s.substr(pom + 1, s.size() - pom - 1);
							pom = s.find('\t');
							s4 = s.substr(0, pom);
							s5 = s.substr(pom + 1, s.size() - pom - 1);
							if (tabelaSimbola.empty()) ts.push_back(new Simbol(s1, s2, s3, s4));
							else ts.push_back(new Simbol(s1, s2, s3, s4,s5));
							break;
						}
						case 3: {
							if (masniski == "") masniski = s;
							else {
								Sekcija* sek = new Sekcija(masniski);
								sek->dodajUtabelu(s);
								ls.push_back(sek);
								masniski = "";
							}
							break;
						}
						case 4: {
							string s1, s2, s3, s4, s5;
							int pom = s.find('\t');
							s1 = s.substr(0, pom);
							s = s.substr(pom + 1, s.size() - pom - 1);
							pom = s.find('\t');
							s2 = s.substr(0, pom);
							s = s.substr(pom + 1, s.size() - pom - 1);
							pom = s.find('\t');
							s3 = s.substr(0, pom);
							s = s.substr(pom + 1, s.size() - pom - 1);
							pom = s.find('\t');
							s4 = s.substr(0, pom);
							s5 = s.substr(pom + 1, s.size() - pom - 1);
							rz.push_back(new Relokacija(s1, s2, s3, s5, s4));
							break;
							break;
						}
						}
					}
				}
			}
		}

		if (svesekcije.empty()) {
			tabelaSimbola = ts;
			listasekcija=ls;
			Relokacioni_zapis=rz;
			svesekcije=svesek;
		}
		else {

			for (Sekcija* sve : ls) {
				dodajMasinskiKod(sve);
			}
			
			for (Simbol* simb : ts) {
				int stara = simb->redniBroj;
				if (daLiJeUTabeliSimbola(simb)) {
					if (simb->sekcija != "und") {
						for (Simbol* simbo : tabelaSimbola) {
							if (simbo->labela == simb->labela) {
								if (((simbo->sekcija!=simbo->labela)&&(simbo->sekcija != "und"))) {//||(simbo->lokal!=simb->lokal)
									cout << "Dvostruko definisan simbol " << simb->labela << endl;
									exit(1);
								}
								else {
									if (simbo->sekcija != simbo->labela) {
										simbo->sekcija = simb->sekcija;
										if (simb->lokal == "local") {
											relacionatabelaglobalispeglaj(Relokacioni_zapis, simbo->redniBroj, dohvatiRedniBrojSekcije(simbo->sekcija), simb->ofset + ofsetSekcije(simb->sekcija));
										}
										simbo->lokal = simb->lokal;
										simbo->ofset = simb->ofset + ofsetSekcije(simb->sekcija);
										daLiSeRedniBrojNalaziURelokaciji(rz, stara, simbo->redniBroj, simbo->sekcija,false);
									}
									else {
										daLiSeRedniBrojNalaziURelokaciji(rz, stara,simbo->redniBroj, simbo->sekcija, true);
									}
								}
							}
						}
					}
					else {
						for (Simbol* simbo : tabelaSimbola) {
							if (simbo->labela == simb->labela){
								if (simbo->sekcija == "und") {
									//////////STA TREBA OVDE//////////
									daLiSeRedniBrojNalaziURelokaciji(rz, stara, simbo->redniBroj,simbo->sekcija,false);
								}
								else {
									if (simbo->lokal == "global")
										daLiSeRedniBrojNalaziURelokaciji(rz, stara, simbo->redniBroj, simbo->sekcija, false);
									else
										daLiSeRedniBrojNalaziURelokaciji(rz, stara, dohvatiRedniBrojSekcije(simbo->sekcija), simbo->sekcija, false);
								}
								}
							}
						}
				}
				else {
					Simbol* si = new Simbol(simb->labela, simb->sekcija, simb->ofset + ofsetSekcije(simb->sekcija), simb->lokal);
					daLiSeRedniBrojNalaziURelokaciji(rz, stara, si->redniBroj, si->sekcija, false);
					tabelaSimbola.push_back(si);
				}
			}

			for (Relokacija* r : rz) {
				Relokacioni_zapis.push_back(r);
			}
			

			for (TabelaSekcija* sve : svesek) {
				dodajSveSekcije(sve);
			}
		}
	}

}


void Linker::odstampajTabelu(ofstream& output){
	output << "Sve sekcije" << endl;
	for (TabelaSekcija* ts : svesekcije) {
		output << ts->sekcija << "  " << ts->size << endl;
	}

	output << endl;

	output << "Tabela simbola:" << endl;
	for (Simbol* s : tabelaSimbola) {
		output << *s;
	}
	output << endl;
	output << "Masinski kod:" << endl;
	for (Sekcija* s : listasekcija) {
		output << s->ime << endl;
		output << *s << endl;
	}

	output << endl;
		
	output << "Relokacioni zapis:"<< endl;
	for (Relokacija* r : Relokacioni_zapis) {
		output << *r << endl;
	}
	
}

void Linker::dodajSveSekcije(TabelaSekcija* t){

	for (TabelaSekcija* sve : svesekcije) {
		if (sve->sekcija == t->sekcija) {
			char s[20];
			string s1 = t->size;
			string s2 = sve->size;
			int x1 = stoi(s1, nullptr, 16);
			int x2 = stoi(s2, nullptr, 16);
			int x = x1 + x2;
			sprintf(s, "%X", x);
			string res = s;
			sve->size = res;
			return;
		}
	}
	svesekcije.push_back(t);

}

void Linker::dodajMasinskiKod(Sekcija* s){
	for (Sekcija* sve : listasekcija) {
		if (sve->ime == s->ime) {
			sve->dodajUtabelu(s->tabela);
			return;
		}
	}
	listasekcija.push_back(s);

}

void Linker::daLiSeRedniBrojNalaziURelokaciji(list<Relokacija*> rz, int rb, int sa, string s, bool dodaj){
	for (Relokacija* r : rz) {
		if ((r->rb == rb)&&(r->x==-1)) {
			r->rb = sa;
			r->x = 1;
			int ofsetic = ofsetSekcije(r->sekcija);
			r->ofset += ofsetic;
			if (dodaj) {
				for (Sekcija* sve : listasekcija) {
					if (sve->ime == s) {
						string s1 = sve->tabela.substr(r->ofset * 2, 2);
						string s2 = sve->tabela.substr(r->ofset * 2+2, 2);
						s2 = s1 + s2;
						int x=stoi(s2, nullptr, 16);
						x += ofsetic;
						char hex[4];
						sprintf(hex, "%X", x);
						s2 = hex;
						for (; s2.size() < 4;) s2 = "0" + s2;
						s1 = s2.substr(2, 2);
						s2 = s2.substr(0, 2);
						s2 = s2 + s1;
						sve->tabela[r->ofset * 2] = s2[0];
						sve->tabela[r->ofset * 2+1] = s2[1];
						sve->tabela[r->ofset * 2+2] = s2[2];
						sve->tabela[r->ofset * 2+3] = s2[3];
					}
				}
			}
		}
	}
}

bool Linker::daLiJeUTabeliSimbola(Simbol* s){
	for (Simbol* simb : tabelaSimbola) {
		if (simb->labela == s->labela){
			return true;
		}
	}
	return false;
}

int Linker::ofsetSekcije(string s){
	for (TabelaSekcija* sve : svesekcije) {
		if (sve->sekcija == s) {
			return stoi(sve->size,nullptr,16);
		}
	}
	return 0;
}

int Linker::dohvatiRedniBrojSekcije(string s){
	for (Simbol* simb : tabelaSimbola) {
		if (simb->labela == s) {
			return simb->redniBroj;
		}
	}
	return -1;
}

void Linker::relacionatabelaglobalispeglaj(list<Relokacija*> rz, int rb, int sa, int x){
	for (Relokacija* r : rz) {
		if ((r->rb == rb)) {
			r->rb = sa;
			char hex[4];
			sprintf(hex, "%X", x);
			string s2 = hex;
			for (; s2.size() < 4;) s2 = "0" + s2;
			string s1 = s2.substr(2, 2);
			s2 = s2.substr(0, 2);
			s2 = s2 + s1;
			for (Sekcija* sve : listasekcija) {
				if (sve->ime == r->sekcija) {
					sve->tabela[r->ofset * 2] = s2[0];
					sve->tabela[r->ofset * 2 + 1] = s2[1];
					sve->tabela[r->ofset * 2 + 2] = s2[2];
					sve->tabela[r->ofset * 2 + 3] = s2[3];
					break;
				}
			}
		}
	}
}

void Linker::placeProcitaj(list<string> place){

	placeplace= new string[place.size()];
	placesekcija = new string[place.size()];
	placesize = place.size();
	int i = 0;
	for (string s : place) {
		stigaoplace = 1;
		int x = s.find("=");
		s = s.substr(x + 1, s.size() - x - 1);
		x = s.find("@");
		placesekcija[i]=s.substr(0,x);
		placeplace[i]=s.substr(x+1,s.size()-x-1);
		i++;
	}

}

void Linker::objediniSveSekcije(){
	int brojac = 0;
	for (SadrzajUlaza sadrzaj : sadrzajUlaznihFajlova) {
		brojac++;
		list<string> listasadrzaja = sadrzaj.sadrzaj;
		int x = 0;
		for (string s : listasadrzaja) {
			if (s == "Sve sekcije") x = 1;
			else {
				if (s == "") break;
				if (x == 1) {
					int ijk = s.find("  ");
					if ((!daLiJeSekcijaUSvimSekcijama(s.substr(0, ijk)))&&(s.substr(0,ijk)!=".equ")) {
						int pocetnaadresa = dohvatiPocetnuAdresuZaSekciju(s.substr(0, ijk));
						TabelaSekcija* tabela = new TabelaSekcija(s.substr(0, ijk), s.substr(ijk + 2, s.size() - ijk - 2));
						tabela->postaviPocetnuIKrajnju(pocetnaadresa,1);
						tabela->fajl = brojac;
						int potencijalnimax = stoi(tabela->krajnja,nullptr,16)*tabela->dalijekorektno;
						if (potencijalnimax > maxadresa) maxadresa = potencijalnimax;
						svesekcije.push_back(tabela);
						dodajDrugeUlistuSvihSekcijaIsetujImAdrese(brojac);
					}
					else {
						//samo je preskacemo	vec smo je dodali
					}
				}
			}
		}
		
	}

	if (stigaoplace== 0) maxadresa = -1;

	for (TabelaSekcija* ts : svesekcije) {
		if (ts->dalijekorektno == 0) {
			ts->postaviPocetnuIKrajnjuzaofset(maxadresa+1);
			maxadresa = stoi(ts->krajnja, nullptr, 16);
		}
	}

	for (TabelaSekcija* ts : svesekcije) 
		dodajMasinkiKodSaPocetnomAdresom(ts->sekcija, ts->fajl, ts->pocetna);


	string s = "";
	int ofset = 0;

	for (TabelaSekcija* ts : svesekcije) {
		if (s==ts->sekcija) {
			ts->ofsetic = ofset;
			ofset = ofset + stoi(ts->size, nullptr, 16);
		}
		else {
			s = ts->sekcija;
			ofset = 0;
			ts->ofsetic = 0;
			ofset = ofset+ stoi(ts->size,nullptr,16);
		}
	}

}

bool Linker::daLiJeSekcijaUSvimSekcijama(string s){
	for (TabelaSekcija* ts : svesekcije) {
		if (s == ts->sekcija)return true;
	}
	return false;
}

int Linker::dohvatiPocetnuAdresuZaSekciju(string s){
	for (int i = 0; i < placesize; i++)
		if (placesekcija[i] == s) return stoi(placeplace[i], nullptr, 0);
	return -1;
}

void Linker::dodajDrugeUlistuSvihSekcijaIsetujImAdrese(int i){
	int brojac = 0;
	for (SadrzajUlaza sadrzaj : sadrzajUlaznihFajlova) {
		brojac++;
		if (brojac > i) {
			list<string> listasadrzaja = sadrzaj.sadrzaj;
			int x = 0;
			for (string s : listasadrzaja) {
				if (s == "Sve sekcije") x = 1;
				else {
					if (s == "") break;
					if (x == 1) {
						int ijk = s.find("  ");
						TabelaSekcija* pom = dohvPoslednjuIzListe(svesekcije);
						if ((s.substr(0, ijk) == pom->sekcija)) {
							int pocetnaadresa = stoi(pom->krajnja, nullptr, 16)+1;
							TabelaSekcija* tabela = new TabelaSekcija(s.substr(0, ijk), s.substr(ijk + 2, s.size() - ijk - 2));
							tabela->postaviPocetnuIKrajnju(pocetnaadresa,pom->dalijekorektno);
							tabela->fajl = brojac;
							int potencijalnimax = stoi(tabela->krajnja, nullptr, 16)*tabela->dalijekorektno;
							if (potencijalnimax > maxadresa) maxadresa = potencijalnimax;
							svesekcije.push_back(tabela);
							break;
						}
						else {
							//nista
						}
					}
				}
			}
		}
		

	}

}

void Linker::napraviTabeluSimbola(){
	int brojac = 0;
	for (SadrzajUlaza sadrzaj : sadrzajUlaznihFajlova) {
		list<string> listasadrzaja = sadrzaj.sadrzaj;
		brojac++;
		int x = 0;
		for (string s : listasadrzaja) {
			if (s == "Tabela simbola:") x = 1;
			else {
				if ((s == "")&&(x==1)) break;
				if (x == 1) {
					string s1, s2, s3, s4, s5;
					int pom = s.find('\t');
					s1 = s.substr(0, pom);
					s = s.substr(pom + 1, s.size() - pom - 1);
					pom = s.find('\t');
					s2 = s.substr(0, pom);
					s = s.substr(pom + 1, s.size() - pom - 1);
					pom = s.find('\t');
					s3 = s.substr(0, pom);
					s = s.substr(pom + 1, s.size() - pom - 1);
					pom = s.find('\t');
					s4 = s.substr(0, pom);
					s5 = s.substr(pom + 1, s.size() - pom - 1);
					Simbol* simbolicic = new Simbol(s1, s2, s3, s4, s5);
					if ((simbolicic->lokal == "global") || (simbolicic->labela == simbolicic->sekcija)) {
						if (daLiJeUTabeliSimbola(simbolicic)) {
							Simbol::RedniBr r;
							r.starirednibroj = simbolicic->redniBroj;
							r.fajl = brojac;
							Simbol* poj = dohvatiSimbolKojiJeIsti(simbolicic);
							poj->satrirednibroj.push_back(r);
							if (poj->sekcija == "und") {
								poj->sekcija = simbolicic->sekcija;
								poj->ofset = simbolicic->ofset + dohvOfsetSekcijeIzFajla(brojac, poj->sekcija);
							}
							else {
								if ((simbolicic->sekcija != "und")&&(simbolicic->sekcija!=simbolicic->labela)) {
									cout << "Greska visestruko definisan simbol " << poj->labela << endl;
									exit(1);
								}
							}
						}
						else {
							Simbol* poj = new Simbol(s1, s2, s3, s4);
							poj->ofset += dohvOfsetSekcijeIzFajla(brojac, poj->sekcija);
							Simbol::RedniBr r;
							r.starirednibroj = simbolicic->redniBroj;
							r.fajl = brojac;
							poj->satrirednibroj.push_back(r);
							tabelaSimbola.push_back(poj);
						}
					}
					else {
						//nista
					}
				}
			}
		}

	}

	for (Simbol* s : tabelaSimbola) {
		if (s->sekcija == "und") {
			cout << "Greska nerazresen simbol: " << s->labela << endl;
			exit(1);
		}
	}
	listasekcija.sort([](const Sekcija* a, const Sekcija* b) 
		{ return stoi(a->pocetnaAdresa, nullptr, 16) < stoi(b->pocetnaAdresa, nullptr, 16); });

}

Simbol* Linker::dohvatiSimbolKojiJeIsti(Simbol* s){
	for (Simbol* simb : tabelaSimbola) {
		if (simb->labela == s->labela) {
			return simb;
		}
	}
	return nullptr;
}

int Linker::dohvOfsetSekcijeIzFajla(int x, string s){
	for (TabelaSekcija* t : svesekcije) {
		if ((t->fajl == x) && (t->sekcija == s))
			return stoi(t->pocetna,nullptr,16);
	}
	return 0;
}

void Linker::dodajMasinkiKodSaPocetnomAdresom(string s, int fajl, string pocadresa){
	int brojac = 0;
	for (SadrzajUlaza sadrzaj : sadrzajUlaznihFajlova) {
		brojac++;
		if (brojac == fajl) {
			list<string> listasadrzaja = sadrzaj.sadrzaj;
			int x = 0;
			int y = 0;
			for (string str : listasadrzaja) {
				if (str == "Masinski kod:") x = 1;
				else {
					if ((str == "")&&(x==1)) break;
					if (x == 1) {
						if (y == 1) {
							for (Sekcija* sekcijaa : listasekcija) {
								if (sekcijaa->ime == s) {
									sekcijaa->dodajUtabelu(str);
									return;
								}
							}
							Sekcija* sekcijaa = new Sekcija(s);
							sekcijaa->dodajUtabelu(str);
							sekcijaa->pocetnaAdresa = pocadresa;
							listasekcija.push_back(sekcijaa);
							return;
						}
						if (str == s) {
							y = 1;
						}
					}
					else {
						//nista
					}
					
				}
			}
		}


	}
}

void Linker::ispisiMaisnkiKodUhex(ofstream& output){
	string masinski = "";
	string pocetna;
	string krajnja = "-5";
	for (Sekcija* s : listasekcija) {
		if ((krajnja != "-5") && (stoi(krajnja, nullptr, 16) + 1 == stoi(s->pocetnaAdresa,nullptr,16))) {
			masinski += s->tabela;
			krajnja = s->vratiKrajnjuAdresu();
		}
		else {
			if (krajnja != "-5") {
				ispisimainskiuformatu(pocetna, masinski, output);
			}
			pocetna = s->pocetnaAdresa;
			masinski = s->tabela;
			krajnja = s->vratiKrajnjuAdresu();
		}
	}
	if (masinski!="") ispisimainskiuformatu(pocetna, masinski, output);
}

void Linker::ispisimainskiuformatu(string pocetnaadresa, string s, ofstream& output){

	int poc = stoi(pocetnaadresa, nullptr, 16);

	if (poc % 8 != 0) {
		for (int k = 0; k < poc % 8; k++)
			s = "00" + s;
		poc = poc - poc % 8;
	}

	for (int i = 0; i < s.size(); i+=16) {
		char hex[20];
		sprintf(hex, "%X", poc);
		string pocetna = hex;
		for (; pocetna.size() < 4;)pocetna = "0" + pocetna;
		output << pocetna<<":  ";
		int j=i;
		for (; (j < i + 16) && (j < s.size()); j = j + 2)
			output << s[j] << s[j + 1] << " ";
		if (j>=s.size()){
			for (; j < i + 16; j=j + 2)
				output << "00" << " ";
		}
		poc += 8;
		output << endl;
	}

}

string Linker::dohvatiMaisnskiKod(int adresa, int velicina){

	for (Sekcija* s : listasekcija) {
		int pocetna=stoi(s->pocetnaAdresa,nullptr,16);
		int krajnja=pocetna+s->tabela.size()/2;
		if ((adresa >= pocetna) && (adresa < krajnja)) {
			velicina = velicina / 4;
			int ofset = adresa - pocetna;
			return s->tabela.substr(ofset*2, velicina);
		}
	}
	return "";

}

void Linker::promeniMasinskiKod(int adresa, int velicina, string stavi){

	for (Sekcija* s : listasekcija) {
		int pocetna = stoi(s->pocetnaAdresa, nullptr, 16);
		int krajnja = pocetna + s->tabela.size() / 2;
		if ((adresa >= pocetna) && (adresa < krajnja)) {
			velicina = velicina / 4;
			int ofset = (adresa - pocetna);
			for (int i = ofset*2; i < 2*ofset + velicina; i++) {
				s->tabela[i] = stavi[i-ofset*2];
			}
			return;
		}
	}

}

void Linker::razresiRelokacioniZapis(){
	int brojac = 0;
	for (SadrzajUlaza sadrzaj : sadrzajUlaznihFajlova) {
		list<string> listasadrzaja = sadrzaj.sadrzaj;
		brojac++;
		int x = 0;
		for (string s : listasadrzaja) {
			if (s == "Relokacioni zapis:") x = 1;
			else {
				if ((s == "") && (x == 1)) break;
				if (x == 1) {
					string s1, s2, s3, s4, s5;
					int pom = s.find('\t');
					s1 = s.substr(0, pom);
					s = s.substr(pom + 1, s.size() - pom - 1);
					pom = s.find('\t');
					s2 = s.substr(0, pom);
					s = s.substr(pom + 1, s.size() - pom - 1);
					pom = s.find('\t');
					s3 = s.substr(0, pom);
					s = s.substr(pom + 1, s.size() - pom - 1);
					pom = s.find('\t');
					s4 = s.substr(0, pom);
					s5 = s.substr(pom + 1, s.size() - pom - 2);
					
					int adresaumasinskom = dohvatiOfsetSekcije(s1) + stoi(s2) + dohvatiOfsetSekcijeUodnosuNaFajl(s1, brojac);
					string izmeni = dohvatiMaisnskiKod(adresaumasinskom, 16);
					if (s3.find("R_data")!=string::npos) 
						izmeni = izmeni.substr(2, 2) + izmeni.substr(0, 2);
					int resenje = dohvatiOfsetIzTabeleSimbolaZaFajlPoRednomBroju(stoi(s4), brojac);
					resenje += stoi(izmeni, nullptr, 16) + stoi(s5);
					if (s3.find("PC") != string::npos)
						resenje -= adresaumasinskom;
					char karakteri[20];
					sprintf(karakteri, "%X", resenje);
					izmeni = karakteri;
					for (; izmeni.size() < 4;) izmeni = "0" + izmeni;
					if (izmeni.size()>4)
						izmeni = izmeni.substr(izmeni.size() - 4, 4);
					if (s3.find("R_data") != string::npos)
						izmeni = izmeni.substr(2, 2) + izmeni.substr(0, 2);
					promeniMasinskiKod(adresaumasinskom, 16, izmeni);


				}
			}
		}

	}

}

int Linker::dohvatiOfsetSekcije(string s){
	for (Simbol* simb : tabelaSimbola) {
		if (simb->labela == s)
			return simb->ofset;
	}
	return 0;
}

int Linker::dohvatiOfsetSekcijeUodnosuNaFajl(string s, int f)
{
	for (TabelaSekcija* ts : svesekcije) {
		if ((ts->sekcija == s) && (ts->fajl == f))
			return ts->ofsetic;
	}
	return 0;
}

int Linker::dohvatiOfsetIzTabeleSimbolaZaFajlPoRednomBroju(int rb, int f){
	for (Simbol* simb : tabelaSimbola) {
		for (Simbol::RedniBr r : simb->satrirednibroj) {
			if ((r.fajl == f) && (r.starirednibroj == rb))
				return simb->ofset + dohvatiOfsetSekcijeUodnosuNaFajl(simb->labela,f) ;
		}
	}
	return 0;
}



TabelaSekcija* Linker::dohvPoslednjuIzListe(list<TabelaSekcija*> s){
	int size = s.size();
	size--;
	for (std::list<TabelaSekcija*>::iterator it = s.begin(); it != s.end(); ++it) {
		if (size == 0) return (*it);
		size--;
	}
	return nullptr;
}

