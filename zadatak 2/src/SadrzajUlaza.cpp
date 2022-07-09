#include "SadrzajUlaza.h"

SadrzajUlaza::SadrzajUlaza(string n) {
	nazivFajla = n;
}

ostream& operator<<(ostream& os, const SadrzajUlaza& s){
	
	os << s.nazivFajla << endl;
	for (string o : s.sadrzaj) {
		os << o << endl;
	}
	return os;
}


