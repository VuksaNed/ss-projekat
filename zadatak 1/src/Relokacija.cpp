#include "Relokacija.h"

Relokacija::Relokacija(string sek, int of, string tip, int add, int r){
	sekcija = sek;
	ofset = of;
	tip_relokacije = tip;
	addend = add;
	rb = r;
}


Relokacija::Relokacija(string sek, string of, string tip, string add, string r) {
	sekcija = sek;
	ofset = stoi(of);
	tip_relokacije = tip;
	addend = stoi(add);
	rb = stoi(r);

}

ostream& operator<<(ostream& os, const Relokacija& s){

	os << s.sekcija << "\t" << s.ofset << "\t" << s.tip_relokacije << "\t" << s.rb << "\t" << s.addend << "\t";

	return os;
}
