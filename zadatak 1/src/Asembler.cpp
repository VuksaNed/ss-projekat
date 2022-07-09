#include "Asembler.h"

string Asembler::instrukcijeZaZaustavljanjeProcesora[] = {"halt"};
string Asembler::instrukcijeSoftverskogPrekida[] = {"int"};
string Asembler::instrukcijePovratakaIzPrekidneRutine[] = {"iret"};
string Asembler::instrukcijePozivaPotprograma[] = {"call"};
string Asembler::instrukcijePovratkaIzPotprograma[] = {"ret"};
string Asembler::instrukcijeSkoka[] = {"jmp","jeq","jne","jgt"};
string Asembler::instrukcijeSaStekom[] = { "push","pop" };
string Asembler::instrukcijeAtomicneZameneVrednosti[] = {"xchg"};
string Asembler::instrukcijeAritmetickihOperacija[] = {"add","sub","mul","div","cmp"};
string Asembler::instrukcijeLogickihOperacija[] = {"not","and","or","xor","test"};
string Asembler::instrukcijePomerackihOperacija[] = {"shr","shl"};
string Asembler::instrukcijeUcitavanjaPodataka[] = {"ldr"};
string Asembler::instrukcijeSmestanjaPodataka[] = {"str"};


void Asembler::asembliraj(ifstream& input, ofstream& output){
    procitajIzFajla(input);

    prviProlaz();

    cout << "Tabela simbola nakon prvog prolaska" << endl;
    odstampajTabelecout();
    cout << endl;

    DrugiProlaz();

    cout << "Tabela simbola nakon drugog prolaska" << endl;

    odstampajTabelecout();
    odstampajTabeleout(output);
}

void Asembler::procitajIzFajla(ifstream& input){
    string line;
    while (getline(input, line)) {
        if (line.find(".end") != string::npos) {
            break;
        }
        if (line.find("#") == string::npos)
            daLiJeLabelaUlaz(line);
        else {
            int x = line.find("#");
            if (x != 0) daLiJeLabelaUlaz(line.substr(0, x));
        }
    }
}

void Asembler::prviProlaz(){
    for (string s : ulazniFajl) {
        int prvo = s.find_first_not_of("\t");
        int poslednje = s.find_last_not_of("\t");
        if ((prvo == -1) || (poslednje == -1)) continue;
        s = s.substr(prvo, poslednje - prvo + 1);
        prvo = s.find_first_not_of(" ");
        poslednje = s.find_last_not_of(" ");
        if ((prvo == -1) || (poslednje == -1)) continue;
        s = s.substr(prvo, poslednje - prvo + 1);
        if (s.find(":") != string::npos) {
            string lab = s.substr(0, s.size() - 1);
            regex labela("[a-zA-z][a-zA-Z0-9_]*");
            if (!regex_match(lab, labela)) {
                cout << "Neispravno uneta labela "<< lab << endl;
                exit(1);
            }
            if(daLiJeVecUtabeli(lab)) {
                cout << "Duplo uneta labela" << lab << endl;
                exit(1);
            }
            tabelaSimbola.push_back(new Simbol(lab, sekcija, offset, "local"));
        }
        else {
            if (s.substr(0, 1) == ".") {
                obradiSekcijuPrviProlaz(s);
            }
            else {
                obradiInstrukcijuPrviProlaz(s);
            }
        }
    }
    if (sekcija != "") {
        svesekcije.push_back(new TabelaSekcija(sekcija, offset));
    }
    if (absoffset != -1) {
        svesekcije.push_back(new TabelaSekcija(".equ", absoffset));
    }
}

void Asembler::DrugiProlaz(){

    offset = 0;
    sekcija = "";

    for (string s : ulazniFajl) {
        int prvo = s.find_first_not_of("\t");
        int poslednje = s.find_last_not_of("\t");
        if ((prvo == -1) || (poslednje == -1)) continue;
        s = s.substr(prvo, poslednje - prvo + 1);
        prvo = s.find_first_not_of(" ");
        poslednje = s.find_last_not_of(" ");
        if ((prvo == -1) || (poslednje == -1)) continue;
        s = s.substr(prvo, poslednje - prvo + 1);
        if (s.find(":") != string::npos) {
            //ZA LABELU NISTA
        }
        else {
            if (s.substr(0, 1) == ".") {
                obradiSekcijuDrugiProlazi(s);
            }
            else {
                obradiInstrukcijuDrugiProlaz(s);
            }
        }
    }


}

bool Asembler::daLiJeVecUtabeli(string line)
{
    
    for (Simbol *s : tabelaSimbola) {
        if (s->labela == line) return true;
    }
    return false;
}

void Asembler::daLiJeLabelaUlaz(string line){
    line = skratistring(line);
    if (line.size() == 0)return;
    if (line.find(":") == string::npos) {
        ulazniFajl.push_back(line);
        return;
    }
    int x = line.find(":");
    ulazniFajl.push_back(line.substr(0, x+1));
    ulazniFajl.push_back(line.substr(x+1, line.size()-x));

}

void Asembler::obradiSekcijuPrviProlaz(string line){

    if (line.find(".global") != string::npos) {
        //ne radimo nista u prvom prolazu samo proveravamo ispravnost
        regex simbol("[a-zA-z][a-zA-Z0-9_]*");
        string str = line.substr(7, line.size() - 7);
        int kraj = str.find(",");
        string s;
        while (kraj != -1) {
            s = str.substr(0, kraj);
            s = skratistring(s);
            if (!regex_match(s, simbol)) {
                cout << "Neispravno uneta linija nije simbol " << line << endl;
                exit(1);
            }
            str = str.substr(kraj + 1, str.size() - kraj - 1);
            kraj = str.find(",");
        }
        s = str;
        s = skratistring(s);
        if (!regex_match(s, simbol)) {
            cout << "Neispravno uneta linija nije simbol " << line << endl;
            exit(1);
        }
        return;

        return;
    }

    if (line.find(".extern") != string::npos) {
        regex simbol("[a-zA-z][a-zA-Z0-9_]*");
        string str = line.substr(7, line.size() - 7);
        int kraj=str.find(",");
        string s;
        while (kraj!=-1){
            s = str.substr(0, kraj);
            s = skratistring(s);
            if (!regex_match(s, simbol)) {
                cout << "Neispravno uneta linija nije simbol " << line << endl;
                exit(1);
            }
            if (daLiJeVecUtabeli(s)) {
                cout << "Duplo uneta labela" << s << endl;
                exit(1);
            }
            tabelaSimbola.push_back( new Simbol(s, "und", -1, "global"));
            str = str.substr(kraj + 1, str.size() - kraj - 1);
            kraj = str.find(",");
        }
        s = str;
        s = skratistring(s);
        if (!regex_match(s, simbol)) {
            cout << "Neispravno uneta linija nije simbol " << line << endl;
            exit(1);
        }
        if (daLiJeVecUtabeli(s)) {
            cout << "Duplo uneta labela" << s << endl;
            exit(1);
        }
        tabelaSimbola.push_back(new Simbol(s, "und", -1, "global"));
        return;
    }

    if (line.find(".section") != string::npos) {
        if (sekcija != "") {
            svesekcije.push_back(new TabelaSekcija(sekcija, offset));
        }
        string s = line.substr(8, line.size() - 8);
        s = skratistring(s);
        sekcija = s;
        offset = 0;
        if (daLiJeVecUtabeli(s)) {
            cout << "Vise puta pojavljivanje sekcije nije dozvoljeno " << sekcija << endl;
            exit(1);
        }
        listasekcija.push_back( new Sekcija(sekcija));
        tabelaSimbola.push_back(new Simbol(s, sekcija, offset, "local"));
        return;
    }

    if (line.find(".word") != string::npos) {
        if (sekcija == "") {
            cout << "Greska poziva se direktiva pre .section direktive" << endl;
            exit(1);
        }
        regex simbol("[a-zA-z][a-zA-Z0-9_]*");
        regex literal("[0-9]+|0x[0-9A-Fa-f]+");
        string str = line.substr(5, line.size() - 5);
        int kraj = str.find(",");
        string s;
        while (kraj != -1) {
            s = str.substr(0, kraj);
            s = skratistring(s);
            if ((!regex_match(s, literal))&&(!regex_match(s,simbol))) {
                cout << "Greska u .word insturkciji " << line << endl;
                exit(1);
            }
            offset += 2;
            str = str.substr(kraj + 1, str.size() - kraj - 1);
            kraj = str.find(",");
        }
        s = str;
        s = skratistring(s);
        if ((!regex_match(s, literal)) && (!regex_match(s, simbol))) {
            cout << "Greska u .word insturkciji " << line << endl;
            exit(1);
        }
        offset += 2;
        return;
    }

    if (line.find(".skip") != string::npos) {
        if (sekcija == "") {
            cout << "Greska poziva se direktiva pre .section direktive" << endl;
            exit(1);
        }
        string s = line.substr(5, line.size() - 5);
        s = skratistring(s);
        regex literal("[0-9]+|0x[0-9A-Fa-f]+");
        if (!regex_match(s, literal)) {
            cout << "Greska u literalu "<<line << endl;
            exit(1);
        }
        int x = stoi(s,nullptr,0);
        offset += x;

        return;
    }

    if (line.find(".equ") != string::npos) {
        string str = line.substr(5, line.size() - 5);
        int kraj = str.find(",");
        string s1= str.substr(0, kraj);
        string s2 = str.substr(kraj + 1, str.size() - kraj - 1);


        s1 = skratistring(s1);
        s2 = skratistring(s2);

        regex literal("[0-9]+|0x[0-9A-Fa-f]+");
        if (!regex_match(s2, literal)) {
            cout << "Neispravna direktiva! " << line << endl;
            exit(1);
        }
        if (daLiJeVecUtabeli(s1)) {
            cout << "Duplo uneta labela" << s1 << endl;
            exit(1);
        }
        if (absoffset == -1) {
            absoffset = 0;
            listasekcija.push_back(new Sekcija(".equ"));
        }
        tabelaSimbola.push_back(new Simbol(s1, ".equ", absoffset, "local"));

        char broj[20];
        int br = stoi(s2, nullptr, 0);
        sprintf(broj, "%X", br);
        string pomocni = broj;
        for (; pomocni.size() < 4;) pomocni = "0" + pomocni;
        absoffset += pomocni.size();
        for (Sekcija* sekcijaa : listasekcija) {
            if (sekcijaa->ime == ".equ") {
                sekcijaa->dodajUtabelu(pomocni);
                return;
            }
        }

        return;
    }

    cout << "Neispravna direktiva! "<< line << endl;
    exit(1);

}

void Asembler::obradiInstrukcijuPrviProlaz(string line){

    line = skratistring(line);


    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeZaZaustavljanjeProcesora[i]) == 0) {
            string s=skratistring(line);
            if (s.size() != 4) {
                cout << "Neispravna instrukcija " << line << endl;
                exit(1);
            }
            offset += 1;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeSoftverskogPrekida[i]) == 0) {
            string s = line;
            line=line.substr(3, line.size() - 3);
            line = skratistring(line);
            if (!daLiJeRegistar(line)) {
                cout << "Neispravna instrukcija "<<s << endl;
                exit(1);
            }
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijePovratakaIzPrekidneRutine[i]) == 0) {
            string s = skratistring(line);
            if (s.size() != 4) {
                cout << "Neispravna instrukcija " << line << endl;
                exit(1);
            }
            offset += 1;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijePozivaPotprograma[i]) == 0) {
            string s = line;
            line = line.substr(4, line.size() - 4);
            line = skratistring(line);
            int x = daLiJeOperandSkoka(line,1);
            if (x == -1) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += x;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijePovratkaIzPotprograma[i]) == 0) {
            string s=skratistring(line);
            if (s.size() != 3) {
                cout << "Neispravna instrukcija " << line << endl;
                exit(1);
            }
            offset += 1;
            return;
        }
    }
        
    for (int i = 0; i < 4; i++) {
        if (line.find(instrukcijeSkoka[i]) == 0) {
            string s = line;
            line = line.substr(4, line.size() - 4);
            line = skratistring(line);
            int x = daLiJeOperandSkoka(line, 1);
            if (x == -1) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += x;
            return;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (line.find(instrukcijeSaStekom[i]) == 0) {
            string s = line;
            line = line.substr(instrukcijeSaStekom[i].size(), line.size() - instrukcijeSaStekom[i].size());
            line = skratistring(line);
            if (!daLiJeRegistar(line)) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += 3;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeAtomicneZameneVrednosti[i]) == 0) {
            string s = line;
            line = line.substr(4, line.size() - 4);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            if ((!daLiJeRegistar(s1)) || (!daLiJeRegistar(s2))) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (line.find(instrukcijeAritmetickihOperacija[i]) == 0) {
            string s=line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            if ((!daLiJeRegistar(s1))||(!daLiJeRegistar(s2))) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (line.find(instrukcijeLogickihOperacija[i]) == 0) {
            string s = line;
            if (i != 2) {
                if (i!=4)
                line = line.substr(3, line.size() - 3);
                else
                    line = line.substr(4, line.size() - 4);
            }
            else {
                line = line.substr(2, line.size() - 2);
            }
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            if ((!daLiJeRegistar(s1)) || (!daLiJeRegistar(s2))) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (line.find(instrukcijePomerackihOperacija[i]) == 0) {
            string s = line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            if ((!daLiJeRegistar(s1)) || (!daLiJeRegistar(s2))) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeUcitavanjaPodataka[i]) == 0) {
            string s = line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            int x = daLiJeOperandPodatka(s2,1);
            if ((!daLiJeRegistar(s1)) || (x==-1)) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += x;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeSmestanjaPodataka[i]) == 0) {
            string s = line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2); 
            int x = daLiJeOperandPodatka(s2,1);
            if ((!daLiJeRegistar(s1)) || (x == -1)) {
                cout << "Neispravna instrukcija " << s << endl;
                exit(1);
            }
            offset += x;
            return;
        }
    }

    cout << "Neposotjeca instrukcija" << endl;
    exit(1);

}

string Asembler::skratistring(string line)
{
    int prvo = line.find_first_not_of("\t");
    int poslednje = line.find_last_not_of("\t");
    if ((prvo!=-1)&&(poslednje!=-1))line = line.substr(prvo, poslednje - prvo + 1);
    prvo = line.find_first_not_of(" ");
    poslednje = line.find_last_not_of(" ");
    if ((prvo != -1) && (poslednje != -1))line = line.substr(prvo, poslednje - prvo + 1);
    prvo = line.find_first_not_of(char(10));
    poslednje = line.find_last_not_of(char(10));
    if ((prvo != -1) && (poslednje != -1))line = line.substr(prvo, poslednje - prvo + 1);
    prvo = line.find_first_not_of(char(13));
    poslednje = line.find_last_not_of(char(13));
    if ((prvo != -1) && (poslednje != -1))line = line.substr(prvo, poslednje - prvo + 1);
    return line;
}

bool Asembler::daLiJeRegistar(string s){

    regex r("([r][0-7])");
    regex psw("psw");
    
    if (regex_match(s, r)|| regex_match(s, psw)) {
        return true;
    }

    return false;
}

int Asembler::daLiJeOperandSkoka(string s, int op)
{
    regex literal("[0-9]+|0x[0-9A-Fa-f]+");
    regex simbol("[a-zA-z][a-zA-Z0-9_]*");
    regex simbolsaprocentom("%[a-zA-z][a-zA-Z0-9_]*");
    if (regex_match(s, literal)) {
        if (op == 1)return 5;
        else return 1;
    }
    if (regex_match(s, simbol)) {
        if (op == 1)return 5;
        else return 2;
    }
    if ( regex_match(s, simbolsaprocentom)) {
        if (op == 1)return 5;
        else return 3;
    }

    if (s.find("*") == 0) {
        s = s.substr(1, s.size() - 1);
        regex r("([r][0-7])");
        if (regex_match(s, literal)) {
            if (op == 1)return 5;
            else return 4;
        }
        if (regex_match(s, r)) {
            if (op == 1)return 3;
            else return 6;
        }
        if (regex_match(s, simbol)) {
            if (op == 1)return 5;
            else return 5;
        }
        int prvo = s.find_first_of("[");
        int poslednje = s.find_last_of("]");
        if ((prvo == 0) && (poslednje == s.size() - 1)) {
            s = s.substr(1, s.size() - 2);
            if (regex_match(s, r)) {
                if (op == 1)return 3;
                else return 7;
            }
            int kraj = s.find("+");
            string s1 = s.substr(0, kraj);
            string s2 = s.substr(kraj + 1, s.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);

            if (regex_match(s1, r) && (regex_match(s2, literal) )) {
                if (op == 1)return 5;
                else return 8;
            }
            if (regex_match(s1, r) && (regex_match(s2, simbol))) {
                if (op == 1)return 5;
                else return 9;
            }

        }
    }

    return -1;
}

int Asembler::daLiJeOperandPodatka(string s, int op){

    regex literal("[0-9]+|0x[0-9A-Fa-f]+");
    regex simbol("[a-zA-z][a-zA-Z0-9_]*");
    regex simbolsaprocentom("%[a-zA-z][a-zA-Z0-9_]*");
    regex r("([r][0-7])");
    if (regex_match(s, r)) {
        if (op == 1)return 3;
        else return 6;
    }

    if (s.find("$") == 0) {
        s = s.substr(1, s.size() - 1);
        if (regex_match(s, literal) ) {
            if (op == 1)return 5;
            else return 1;
        }
        if (regex_match(s, simbol)) {
            if (op == 1)return 5;
            else return 2;
        }
        return -1;
    }

    if (regex_match(s, literal)) {
        if (op == 1)return 5;
        else return 3;
    }
    if (regex_match(s, simbol)) {
        if (op == 1)return 5;
        else return 4;
    }
    if (regex_match(s, simbolsaprocentom)) {
        if (op == 1)return 5;
        else return 5;
    }
    
    int prvo = s.find_first_of("[");
    int poslednje = s.find_last_of("]");
    if ((prvo == 0) && (poslednje == s.size() - 1)) {
        s = s.substr(1, s.size() - 2);
        if (regex_match(s, r)) {
            if (op == 1)return 3;
            else return 7;
        }
        int kraj = s.find("+");
        string s1 = s.substr(0, kraj);
        string s2 = s.substr(kraj + 1, s.size() - kraj - 1);
        s1 = skratistring(s1);
        s2 = skratistring(s2);

        if (regex_match(s1, r) && (regex_match(s2, literal))) {
            if (op == 1)return 5;
            else return 8;
        }
        if (regex_match(s1, r) && ( regex_match(s2, simbol))) {
            if (op == 1)return 5;
            else return 9;
        }

    }
    return -1;
}

void Asembler::obradiInstrukcijuDrugiProlaz(string line){
    line = skratistring(line);

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeZaZaustavljanjeProcesora[i]) == 0) {
            string out = "00";
            dodajUTabeluSekcija(out);
            offset += 1;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeSoftverskogPrekida[i]) == 0) {
            string s = line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            string out = "10";
            out = out + line.substr(1,1);
            out = out + "F";
            dodajUTabeluSekcija(out);
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijePovratakaIzPrekidneRutine[i]) == 0) {
            string out = "20";
            dodajUTabeluSekcija(out);
            offset += 1;
            return;
        }
    }
    
    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijePozivaPotprograma[i]) == 0) {
            string s = line;
            line = line.substr(4, line.size() - 4);
            line = skratistring(line);
            int x = daLiJeOperandSkoka(line, 2);
            int d = daLiJeOperandSkoka(line, 1);
            offset += d;
            string out = "30";
            out = out + obradiOperandSkoka(line, x);
            dodajUTabeluSekcija(out);

            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijePovratkaIzPotprograma[i]) == 0) {
            string out = "40";
            dodajUTabeluSekcija(out);
            offset += 1;
            return;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        if (line.find(instrukcijeSkoka[i]) == 0) {
            string s = line;
            s = s.substr(0, 3);
            line = line.substr(4, line.size() - 4);
            line = skratistring(line);
            int x = daLiJeOperandSkoka(line, 2); 
            int d = daLiJeOperandSkoka(line, 1);
            offset += d;
            string out = "5";
            if (s == "jmp") {
                out += "0";
            }
            else {
                if (s == "jeq") {
                    out += "1";
                }
                else {
                    if (s == "jne") {
                        out += "2";
                    }
                    else {
                        if (s == "jgt") {
                            out += "3";
                        }
                        else {
                            //nee se desiti
                            cout << "Greska" << endl;
                            exit(1);
                        }
                    }
                }
            }
            out = out + obradiOperandSkoka(line, x);
            dodajUTabeluSekcija(out);

            return;
        }
    }
    
    for (int i = 0; i < 2; i++) {
        if (line.find(instrukcijeSaStekom[i]) == 0) {
            string s = line;
            string op = line.substr(0, instrukcijeSaStekom[i].size());
            line = line.substr(instrukcijeSaStekom[i].size(), line.size() - instrukcijeSaStekom[i].size());
            line = skratistring(line);
            string out = "";
            if (op == "push") {
                out = "B06"+line.substr(1,1)+"12";
            }
            else {//pop
                out = "A0" + line.substr(1, 1) + "642";
            }
            offset += 3;
            dodajUTabeluSekcija(out);

            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeAtomicneZameneVrednosti[i]) == 0) {
            string s = line;
            line = line.substr(4, line.size() - 4);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            string out = "60";
            out = out + s1.substr(1, 1) + s2.substr(1,1);
            offset += 2;
            dodajUTabeluSekcija(out);
            return;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (line.find(instrukcijeAritmetickihOperacija[i]) == 0) {
            string s = line;
            string operacija = line.substr(0, 3);
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            offset += 2;
            string out;
            if (operacija == "add") {
                out = "70";
            }
            else {
                if (operacija == "sub") {
                    out = "71";
                }
                else {
                    if (operacija == "mul") {
                        out = "72";
                    }
                    else {
                        if (operacija == "div") {
                            out = "73";
                        }
                        else {
                            if (operacija == "cmp") {
                                out = "74";
                            }
                            else {
                                //nece se desiti
                            }
                        }
                    }
                }
            }

            out = out + s1.substr(1, 1) + s2.substr(1, 1);

            dodajUTabeluSekcija(out);
            return;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (line.find(instrukcijeLogickihOperacija[i]) == 0) {
            string s = line;
            string operacija;
            if (i != 2) {
                if (i != 4) {
                    operacija = line.substr(0, 3);
                    line = line.substr(3, line.size() - 3);
                }
                else {
                    operacija = line.substr(0, 4);
                    line = line.substr(4, line.size() - 4);
                }
            }
            else {
                operacija = line.substr(0, 2);
                line = line.substr(2, line.size() - 2);
            }
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            string out;
            if (operacija == "not") {
                out = "80";
            }
            else {
                if (operacija == "and") {
                    out = "81";
                }
                else {
                    if (operacija == "or") {
                        out = "82";
                    }
                    else {
                        if (operacija == "xor") {
                            out = "83";
                        }
                        else {
                            if (operacija == "test") {
                                out = "84";
                            }
                            else {
                                //nece se desiti
                            }
                        }
                    }
                }
            }

            out = out + s1.substr(1, 1) + s2.substr(1, 1);
            dodajUTabeluSekcija(out);
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 2; i++) {
        if (line.find(instrukcijePomerackihOperacija[i]) == 0) {
            string s = line;
            string operacija;
            operacija = line.substr(0, 3);
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            string out = "";
            if (operacija == "shl") {
                out = "90";
            }
            else {
                out = "91";
            }
            out = out + s1.substr(1, 1) + s2.substr(1, 1);
            dodajUTabeluSekcija(out);
            offset += 2;
            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeUcitavanjaPodataka[i]) == 0) {
            string s = line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2);
            int x = daLiJeOperandPodatka(s2,2);
            int d = daLiJeOperandPodatka(s2, 1);
            offset += d;
            string out = "A0" + obradiOperandPodataka(s1, s2, x);
            
            dodajUTabeluSekcija(out);

            return;
        }
    }

    for (int i = 0; i < 1; i++) {
        if (line.find(instrukcijeSmestanjaPodataka[i]) == 0) {
            string s = line;
            line = line.substr(3, line.size() - 3);
            line = skratistring(line);
            int kraj = line.find(",");
            string s1 = line.substr(0, kraj);
            string s2 = line.substr(kraj + 1, line.size() - kraj - 1);
            s1 = skratistring(s1);
            s2 = skratistring(s2); 
            int x = daLiJeOperandPodatka(s2,2);
            int d = daLiJeOperandPodatka(s2, 1);
            offset += d;
            string out = "B0" + obradiOperandPodataka(s1, s2, x);
            dodajUTabeluSekcija(out);
            return;
        }
    }

}

void Asembler::obradiSekcijuDrugiProlazi(string line){


    if (line.find(".global") != string::npos) {
        string str = line.substr(7, line.size() - 7);
        int kraj = str.find(",");
        string s;
        while (kraj != -1) {
            s = str.substr(0, kraj);
            s=skratistring(s);
            nadjiUTabeliIstaviUglobal(s);
            
            str = str.substr(kraj + 1, str.size() - kraj - 1);
            kraj = str.find(",");
        }
        s = str;
        s = skratistring(s);

        nadjiUTabeliIstaviUglobal(s);

        return;
    }

    if (line.find(".extern") != string::npos) {
       
        //ne treba nista sve odradjeno u prvom prolazu

        return;
    }

    if (line.find(".section") != string::npos) {
        string s = line.substr(8, line.size() - 8);
        s = skratistring(s);
        sekcija = s;
        offset = 0;
        return;
    }

    if (line.find(".word") != string::npos) {
        string str = line.substr(5, line.size() - 5);
        int kraj = str.find(",");
        string s;
        while (kraj != -1) {
            s = str.substr(0, kraj);
            s = skratistring(s); 
            offset += 2;
            obradiWordLiteralISimbol(s);
            str = str.substr(kraj + 1, str.size() - kraj - 1);
            kraj = str.find(",");
        }
        s = str;
        s = skratistring(s);
        offset += 2;
        obradiWordLiteralISimbol(s);
        return;
    }

    if (line.find(".skip") != string::npos) {
        string s = line.substr(5, line.size() - 5);
        s = skratistring(s);
        int x = stoi(s, nullptr, 0);
        offset += x;
        for (int i = 0; i < x; i++)
            dodajUTabeluSekcija("00");

        return;
    }

    if (line.find(".equ") != string::npos) {
        //nista ne radimo
        return;
    }

    cout << "Neispravna direktiva! " << line << endl;
    exit(1);

}

void Asembler::nadjiUTabeliIstaviUglobal(string s){
    for (Simbol* simbol : tabelaSimbola) {
        if (simbol->labela == s) {
            simbol->lokal = "global";
            return;
        }
    }
    cout << "Greska .global " << s << endl;
    exit(1);
}

void Asembler::dodajUTabeluSekcija(string s){
    for (Sekcija* sekcijaa : listasekcija) {
        if (sekcijaa->ime == sekcija) {
            sekcijaa->dodajUtabelu(s);
            return;
        }

    }
}

void Asembler::odstampajTabelecout(){

    cout << "Tabela simbola: " << endl;
    cout << "Labela" << "\t" << "Sekcija" << "\t" << "Offset" << "\t" << "Local?" << "\t" << "RB" << endl;
    for (Simbol* s : tabelaSimbola) {
        std::cout << *s;
    }
    cout << endl;
    cout << endl;
    for (Sekcija* s : listasekcija) {
        std::cout << "Sekcija " << s->ime << endl;
        std::cout << *s<<endl;
    }

    cout << endl;
    cout << "Sekcija" << "\t" << "Ofset" << "\t" << "Tip relokacije" << "\t" << "RB" << "\t" << "addend" << endl;
    for (Relokacija* r : Relokacioni_zapis) {
        std::cout << *r << endl;
    }

    cout << endl;
    cout << endl;
    cout << "Sve sekcije" << endl;
    for (TabelaSekcija* ts : svesekcije) {
        cout << ts->sekcija << "  " << ts->size << endl;
    }

}

void Asembler::odstampajTabeleout(ofstream& output){
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
    output <<"Masinski kod:"<< endl;
    for (Sekcija* s : listasekcija) {
        output << s->ime << endl;
        output << *s << endl;
    }
    output << endl;
    output << "Relokacioni zapis:" << endl;
    for (Relokacija* r : Relokacioni_zapis) {
        output << *r << endl;
    }
}

string Asembler::obradiOperandPodataka(string s1, string s2, int x){
    string ret = "";
    switch (x)
    {
    case 1: {
        ret = s1.substr(1, 1) + "F00";
        char s[4];
        s2 = s2.substr(1, 1 - s2.size());
        int br = stoi(s2, nullptr, 0);
        sprintf(s, "%X", br);
        string res = s;
        for (int i = 0; i < 4 - res.size();) {
            res = "0" + res;
        }
        ret += res;
        return ret;
        break;
    }
    case 2: {
        ret = s1.substr(1, 1) + "F00";
        s2 = s2.substr(1, s2.size() - 1);
        string pom = dohvatiSimbolPodatak(s2);
        ret = ret + pom;
        return ret;
        break;
    }
    case 3: {
        ret = s1.substr(1, 1) + "F04";
        char s[4];
        int br = stoi(s2, nullptr, 0);
        sprintf(s, "%X", br);
        string res = s;
        for (int i = 0; i < 4 - res.size();) {
            res = "0" + res;
        }
        ret += res;
        return ret;
        break;
    }
    case 4: {
        ret = s1.substr(1, 1) + "F04";
        string pom = dohvatiSimbolPodatak(s2);
        ret = ret + pom;
        return ret;
        break;
    }
    case 5: {
        ret = s1.substr(1, 1) + "705";
        s2 = s2.substr(1, s2.size() - 1);
        string pom = dohvatiSimbolPodatakPC(s2);
        ret = ret + pom;
        return ret;
        break;
        break;
    }
    case 6:
        ret = s1.substr(1, 1) + s2.substr(1, 1) + "01";
        return ret;
        break;
    case 7:
        ret = s1.substr(1, 1) + s2.substr(2, 1) + "02";
        return ret;
        break;
    case 8:
    {
        int prvo = s2.find_first_of("[");
        int poslednje = s2.find_last_of("]");
        s2 = s2.substr(prvo + 1, poslednje - prvo - 1);
        int kraj = s2.find("+");
        string s21 = s2.substr(0, kraj);
        string s22 = s2.substr(kraj + 1, s2.size() - kraj - 1);
        s21 = skratistring(s21);
        s22 = skratistring(s22);
        ret = s1.substr(1, 1) + s21.substr(1, 1) + "03";
        char s[4];
        int br = stoi(s22, nullptr, 0);
        sprintf(s, "%X", br);
        string res = s;
        for (int i = 0; i < 4 - res.size();) {
            res = "0" + res;
        }
        ret += res;
        return ret;
        break;
    }
    case 9:{
        int prvo = s2.find_first_of("[");
        int poslednje = s2.find_last_of("]");
        s2 = s2.substr(prvo + 1, poslednje - prvo - 1);
        int kraj = s2.find("+");
        string s21 = s2.substr(0, kraj);
        string s22 = s2.substr(kraj + 1, s2.size() - kraj - 1);
        s21 = skratistring(s21);
        s22 = skratistring(s22);

        ret = s1.substr(1, 1) + s21.substr(1, 1) + "03";
        string res = dohvatiSimbolPodatak(s22);
        ret += res;
        return ret;
        break;
    }
    }
    return "";
}

void Asembler::obradiWordLiteralISimbol(string s){
    regex simbol("[a-zA-z][a-zA-Z0-9_]*");
    s=skratistring(s);
    if (regex_match(s, simbol)) {//simbol
        for (Simbol *sim : tabelaSimbola) {
            if (sim->labela == s) {
                if (sim->lokal == "local") {
                    string sek = "";
                    for (Simbol* simb : tabelaSimbola) {
                        if (simb->labela == s)
                            sek = simb->sekcija;
                    }
                    int pos=-1;
                    for (Simbol* simb : tabelaSimbola) {
                        if (simb->labela == sek)
                            pos = simb->redniBroj;
                    }
                    Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_data_16", 0, pos));
                    char broj[20];
                    sprintf(broj, "%X", sim->ofset);
                    string pomocni = broj;
                    for (; pomocni.size() < 4;) pomocni = "0" + pomocni;
                    dodajUTabeluSekcija(pomocni.substr(2, 2) + pomocni.substr(0, 2));
                }
                else {
                    Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_data_16", 0, sim->redniBroj));
                    dodajUTabeluSekcija("0000");
                }
                return;
            }

        }
        Simbol* pomocnica = new Simbol(s, "und", -1, "global");
        dodajUTabeluSekcija("0000");
        tabelaSimbola.push_back(pomocnica);
        Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_data_16", 0, pomocnica->redniBroj));
    }
    else {//literal
        char broj[20];
        int br = stoi(s, nullptr, 0);
        sprintf(broj, "%X", br);
        string pomocni = broj;
        for (; pomocni.size() < 4;) pomocni = "0" + pomocni;
        string out = pomocni.substr(2,2)+pomocni.substr(0,2);  
        dodajUTabeluSekcija(out);
    }
}

string Asembler::dohvatiSimbolPodatak(string s){
    
    Simbol* sim=nullptr;
    for (Simbol* simb : tabelaSimbola) {
        if (simb->labela == s)
            sim = simb;
    }
    if (sim == nullptr) {
        cout << "Greska nije definisan simbol "<<s;
        exit(1);
    }
    if (sim->sekcija == ".equ") {
        int ofset = sim->ofset;
        for (Sekcija* x:listasekcija)
            if (x->ime == sim->sekcija) {
                string out = x->tabela.substr(ofset, 4);
                return out;
            }
    }
    else {
        if (sim->sekcija == "und") {
            Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_16", 0, sim->redniBroj));
            return "0000";
        }
        else {
             if (sim->lokal == "local"){ 
                 int pos = -1;
                 for (Simbol* simb : tabelaSimbola) {
                     if (simb->labela == sim->sekcija)
                         pos = simb->redniBroj;
                 }
                 Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_16", 0, pos));
                 char broj[20];
                 sprintf(broj, "%X", sim->ofset);
                 string pomocni = broj;
                 for (; pomocni.size() < 4;) pomocni = "0" + pomocni;
                 string out = pomocni;
                 return out;
             }
             else{
                 Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_16", 0, sim->redniBroj));
                 return "0000";
             }
        }
    }

    return "";
}

string Asembler::dohvatiSimbolPodatakPC(string s)
{
    Simbol* sim = nullptr;
    for (Simbol* simb : tabelaSimbola) {
        if (simb->labela == s)
            sim = simb;
    }
    if (sim == nullptr) {
        cout << "Greska nije definisan simbol " << s ;
        exit(1);
    }
    if (sim->sekcija == ".equ") {
        int ofset = sim->ofset;
        for (Sekcija* x : listasekcija)
            if (x->ime == sim->sekcija) {
                Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_PC16", -2, sim->redniBroj));
                string out = x->tabela.substr(ofset, 4);
                return out;
            }
    }
    else {
        if (sim->sekcija == "und") {
            Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_PC16", -2, sim->redniBroj));
            return "0000";
        }
        else {
            if (sim->lokal == "local") {
                int pos = -1;
                for (Simbol* simb : tabelaSimbola) {
                    if (simb->labela == sim->sekcija)
                        pos = simb->redniBroj;
                }
                Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_PC16", -2, pos));
                char broj[20];
                sprintf(broj, "%X", sim->ofset);
                string pomocni = broj;
                for (; pomocni.size() < 4;) pomocni = "0" + pomocni;
                string out = pomocni;
                return out; 
            }
            else {
                Relokacioni_zapis.push_back(new Relokacija(sekcija, offset-2, "R_instruction_PC16", -2, sim->redniBroj));
                return "0000";
            }
        }
    }

    return "";
}

string Asembler::obradiOperandSkoka(string s2, int x){
    string ret = "";
    switch (x)
    {
    case 1: {
        ret = "FF00";
        char s[4];
        int br = stoi(s2, nullptr, 0);
        sprintf(s, "%X", br);
        string res = s;
        for (int i = 0; i < 4 - res.size();) {
            res = "0" + res;
        }
        ret += res;
        return ret;
        break;
    }
    case 2: {
        ret ="FF00";
        string pom = dohvatiSimbolPodatak(s2);
        ret = ret + pom;
        return ret;
        break;
    }
    case 3: {
        ret ="F705";
        s2 = s2.substr(1, s2.size() - 1);
        string pom = dohvatiSimbolPodatakPC(s2);
        ret = ret + pom;
        return ret;
        break;
    }
    case 4: {
        ret = "FF04";
        s2 = s2.substr(1, s2.size() - 1);
        char s[4];
        int br = stoi(s2, nullptr, 0);
        sprintf(s, "%X", br);
        string res = s;
        for (int i = 0; i < 4 - res.size();) {
            res = "0" + res;
        }
        ret += res;
        return ret;
        break;
    }
    case 5: {
        ret = "FF04";
        s2 = s2.substr(1, s2.size() - 1);
        string pom = dohvatiSimbolPodatak(s2);
        ret = ret + pom;
        return ret;
        break;
    }
    case 6:
        ret = "F" + s2.substr(2, 1) + "01";
        return ret;
        break;
    case 7:
        ret = "F" + s2.substr(3, 1) + "02";
        return ret;
        break;
    case 8:
    {
        int prvo = s2.find_first_of("[");
        int poslednje = s2.find_last_of("]");
        s2 = s2.substr(prvo + 1, poslednje - prvo - 1);
        int kraj = s2.find("+");
        string s21 = s2.substr(0, kraj);
        string s22 = s2.substr(kraj + 1, s2.size() - kraj - 1);
        s21 = skratistring(s21);
        s22 = skratistring(s22);
        ret = "F"+ s21.substr(1,1) +"03";
        char s[4];
        int br = stoi(s22, nullptr, 0);
        sprintf(s, "%X", br);
        string res = s;
        for (int i = 0; i < 4 - res.size();) {
            res = "0" + res;
        }
        ret += res;
        return ret;
        break;
    }
    case 9: {
        int prvo = s2.find_first_of("[");
        int poslednje = s2.find_last_of("]");
        s2 = s2.substr(prvo + 1, poslednje - prvo - 1);
        int kraj = s2.find("+");
        string s21 = s2.substr(0, kraj);
        string s22 = s2.substr(kraj + 1, s2.size() - kraj - 1);
        s21 = skratistring(s21);
        s22 = skratistring(s22);

        ret = "F" + s21.substr(1, 1) + "03";
        string res = dohvatiSimbolPodatak(s22);
        ret += res;
        return ret;
        break;
    }
    }
    return "";
}