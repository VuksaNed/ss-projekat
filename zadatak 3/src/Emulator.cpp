#include "Emulator.h"
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <fstream>

using namespace std;

void Emulator::Emuliraj(ifstream& input){
    ucitajUlazniFajl(input);
    inicijalizuj();
    ucitajMemMapReg();
    ucitajPC(0);

    izvrsavajProgram();

}

void Emulator::ucitajUlazniFajl(ifstream& input){
    string line;
    while (getline(input, line)) {
        int x = line.find(":");
        if (x == string::npos) {
            return;
        }

        string adresa = line.substr(0, 4);
        Memorija::MemJedinica* pom = new Memorija::MemJedinica();
        pom->adresa = adresa;
        x = 7;
        for (int i = 0; i < 8; i++) {
            pom->sadrzaj[i] = line.substr(x, 2);
            x += 3;
        }
        poslednja = pom->adresa;
        memorija.mem.push_back(pom);

    }
}

void Emulator::inicijalizuj(){
    for (int i = 0; i < 16; i++)
        psw[i] = false;


    string adr = poslednja;
    char hex[8];
    int x = stoi(adr, nullptr, 16);
    x += 8;
    sprintf(hex, "%X", x);
    adr = hex;
    for (; adr.size() < 4;) adr = "0" + adr;
    r[6] = x;

    while (adr!="FF00") {
        r[6] = x;

        Memorija::MemJedinica* pom = new Memorija::MemJedinica();
        pom->adresa = adr;
        for (int i = 0; i < 8; i++) {
            pom->sadrzaj[i] = "00";
        }
        memorija.mem.push_back(pom);
        
        x = stoi(adr, nullptr, 16);
        x += 8;
        sprintf(hex, "%X", x);
        adr = hex;
        for (; adr.size() < 4;) adr = "0" + adr;
       
    }


}

void Emulator::azurirajTerminal() {
    char x;
    //x = getch();
    int p = x;
    if ((p != -1)&&(x!='\n')) {
        char hex[8];
        sprintf(hex, "%X", x);
        string s = hex;
        for (; s.size() < 4;)s = "0" + s;
        memorija.upisiUMemorijuNaAdresi(65282, s.substr(0, 2));
        memorija.upisiUMemorijuNaAdresi(65283, s.substr(2, 2));
        terminal = true;
    }
        
    
}


void Emulator::ucitajMemMapReg(){
    string adr = "FF00";
    char hex[8];
    for (int i = 0; i < 32; i++) {
        Memorija::MemJedinica* pom = new Memorija::MemJedinica();
        pom->adresa = adr;
        for (int i = 0; i < 8; i++) {
            pom->sadrzaj[i] = "00";
        }
        memorija.mem.push_back(pom);

        int x = stoi(adr, nullptr, 16);
        x += 8;
        sprintf(hex, "%X", x);
        adr = hex;
        for (; adr.size() < 4;) adr = "0" + adr;

    }

}

void Emulator::ucitajPC(int adr){
    string s1 = memorija.dohvatiSadrzajSaAdrese(adr);
    adr++;
    string s2 = memorija.dohvatiSadrzajSaAdrese(adr);
    s2 = s2 + s1;
    if (s2 == "##" || s1 == "##") {
        cout << "PC ne moze da ima trazenu vrednost" << endl;
        exit(1);
    }
    r[7] = stoi(s2, nullptr, 16);
}

void Emulator::izvrsavajProgram(){

    while (true) {

        if (memorija.dosaoupis) {
            string s1 = memorija.dohvatiSadrzajSaAdrese(65280);
            string s2 = memorija.dohvatiSadrzajSaAdrese(65281);
            s1 = s1 + s2;
            int x = stoi(s1, nullptr, 16);
            char out = x;
            cout << x;
        }

        if (terminal && (!psw[14])) {
            //push pc
            int x = r[7];
            char hex[10];
            sprintf(hex, "%X", x);
            string nastek = hex;
            for (; nastek.size() < 4;)nastek = "0" + nastek;
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));

            //push psw
            nastek = pswUString();
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));

            //pc <= mem16[(regD mod 8)*2];
            int registar = (3 % 8) * 2;
            ucitajPC(registar);

        }

        if (memorija.dosaoupis) {
            string s1 = memorija.dohvatiSadrzajSaAdrese(65280);
            string s2 = memorija.dohvatiSadrzajSaAdrese(65281);
            s1 = s1 + s2;
            int x = stoi(s1, nullptr, 16);
            char out = x;
            cout << x;
        }

        if (tajmer && (!psw[13])) {
            tajmer = false;
            cout << "T";
            //push pc
            int x = r[7];
            char hex[10];
            sprintf(hex, "%X", x);
            string nastek = hex;
            for (; nastek.size() < 4;)nastek = "0" + nastek;
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));

            //push psw
            nastek = pswUString();
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));

            //pc <= mem16[(regD mod 8)*2];
            int registar = (2 % 8) * 2;
            ucitajPC(registar);
        }


        string instr = memorija.dohvatiSadrzajSaAdrese(r[7]);
        r[7]++;
        if (instr == "00") {//halt
            return;
        }

        if (instr == "10") {//int
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            s1 = s1.substr(0, 1);
            int registar = stoi(s1, nullptr, 16);
            if (!psw[15]) {
                //push pc
                int x = r[7];
                char hex[10];
                sprintf(hex, "%X", x);
                string nastek = hex;
                for (; nastek.size() < 4;)nastek = "0" + nastek;
                r[6]--;
                memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
                r[6]--;
                memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));

                //push psw
                nastek = pswUString();
                r[6]--;
                memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
                r[6]--;
                memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));

                //pc <= mem16[(regD mod 8)*2];
                registar = (r[registar] % 8) * 2;
                ucitajPC(registar);
            }
        }

        if (instr == "20") {//iret
            //pop psw
            string s1=memorija.dohvatiSadrzajSaAdrese(r[6]);
            r[6]++;
            s1=memorija.dohvatiSadrzajSaAdrese(r[6])+s1;
            r[6]++;
            stringUPSW(s1);
            //pop pc
            s1 = memorija.dohvatiSadrzajSaAdrese(r[6]);
            r[6]++;
            s1 = memorija.dohvatiSadrzajSaAdrese(r[6])+s1;
            r[6]++;
            int x = stoi(s1, nullptr, 16);
            r[7] = x;

        }

        if (instr == "30") {//call
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            int noviPc= dohvatiNacinAdresiranja(s1,s2);
            //push psw
            int x = r[7];
            char hex[10];
            sprintf(hex, "%X", x);
            string nastek = hex;
            for (; nastek.size() < 4;)nastek = "0" + nastek;
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(0, 2));
            r[6]--;
            memorija.upisiUMemorijuNaAdresi(r[6], nastek.substr(2, 2));
            // pc <= operand;
            r[7] = noviPc;
        }

        if (instr == "40") {//ret
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[6]);
            r[6]++;
            s1 = memorija.dohvatiSadrzajSaAdrese(r[6])+s1;
            r[6]++;
            int x = stoi(s1, nullptr, 16);
            r[7] = x;
        }

        if (instr.substr(0, 1) == "5") {//jmp, jeq, jne, jgt
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            int noviPc = dohvatiNacinAdresiranja(s1, s2);
            string operacija = instr.substr(1, 1);

            if (operacija == "0") {
              //  /* jmp */ pc <= operand;
                r[7] = noviPc;
            }
            else {
                if (operacija == "1") {
                    // /* jeq */ if (equal) pc<=operand;
                    if (psw[0])
                        r[7] = noviPc;
                }
                else {
                    if (operacija == "2") {
                        // /* jne */ if (not equal) pc<=operand;
                        if (!psw[0])
                            r[7] = noviPc;
                    }
                    else {
                        if (operacija == "3") {
                            //  /* jgt */ if (signed greater) pc<=operand;
                            if (!(psw[1]^psw[2])&!psw[0])////
                                r[7] = noviPc;
                        }
                        else {
                           //nece se desiti
                        }
                    }
                }
            }
        }

        if (instr == "60") {//xchg
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = s1.substr(1, 1);
            s1 = s1.substr(0, 1);
            int registar1 = stoi(s1, nullptr, 16);
            int registar2 = stoi(s2, nullptr, 16);
            int pomocna = r[registar1];
            r[registar1] = r[registar2];
            r[registar2] = pomocna;
        }

        if (instr.substr(0, 1) == "7") {//add,sub, mul, div, cmp
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = s1.substr(1, 1);
            s1 = s1.substr(0, 1);
            int registar1 = stoi(s1, nullptr, 16);
            int registar2 = stoi(s2, nullptr, 16);

            string operacija = instr.substr(1, 1);

            if (operacija == "0") {
                r[registar1] = r[registar1] + r[registar2];
            }
            else {
                if (operacija == "1") {
                    r[registar1] = r[registar1] - r[registar2];
                }
                else {
                    if (operacija == "2") {
                        r[registar1] = r[registar1] * r[registar2];
                    }
                    else {
                        if (operacija == "3") {
                            r[registar1] = r[registar1] / r[registar2];
                        }
                        else {
                            if (operacija == "4") {
                                int a = registar1;
                                int b = registar2;
                                psw[0] = (a == b);
                                psw[1] = ((a > 0 && b < 0 && (a - b) < 0) || (a < 0 && b>0 && (a - b) > 0));
                                psw[2] = (a < b);
                                psw[3] = ((a - b) < 0);
                            }
                            else {
                                //nece se desiti
                            }
                        }
                    }
                }
            }

        }


        if (instr.substr(0, 1) == "8") {//not, and, or, xor, test
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = s1.substr(1, 1);
            s1 = s1.substr(0, 1);
            int registar1 = stoi(s1, nullptr, 16);
            int registar2 = stoi(s2, nullptr, 16);

            string operacija = instr.substr(1, 1);

            if (operacija == "0") {
                r[registar1] = ~r[registar1];
            }
            else {
                if (operacija == "1") {
                    r[registar1] = r[registar1] & r[registar2];
                }
                else {
                    if (operacija == "2") {
                        r[registar1] = r[registar1] | r[registar2];
                    }
                    else {
                        if (operacija == "3") {
                            r[registar1] = r[registar1] ^ r[registar2];
                        }
                        else {
                            if (operacija == "4") {
                                int a = registar1;
                                int b = registar2;
                                psw[0] = ((a & b) == 0);
                                psw[3] = ((a & b) < 0);
                            }
                            else {
                                //nece se desiti
                            }
                        }
                    }
                }
            }

        }


        if (instr.substr(0, 1) == "9") {//shl, shr
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = s1.substr(1, 1);
            s1 = s1.substr(0, 1);
            int registar1 = stoi(s1, nullptr, 16);
            int registar2 = stoi(s2, nullptr, 16);

            string operacija = instr.substr(1, 1);

            if (operacija == "0") {
                r[registar1] = r[registar1] << r[registar2];
                //azuriraj bite
            }
            else {
                if (operacija == "1") {
                    r[registar1] = r[registar1] >> r[registar2];
                    //azuriraj bite
                }
                else {
                    //nece se desiti
                }
            }

        }

        if (instr == "A0") {//ldr
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string posalji = s1.substr(1, 1) + s1.substr(0, 1);
            int operand = dohvatiNacinAdresiranja(posalji, s2);
            s1 = s1.substr(0, 1);
            int registar1 = stoi(s1, nullptr, 16);
            r[registar1] = operand;
        }

        if (instr == "B0") {//str
            string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            string s2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
            r[7]++;
            int operand = dohvatiNacinAdresiranja(s1, s2);
            s1 = s1.substr(0, 1);
            int registar1 = stoi(s1, nullptr, 16);
            char hex[10];
            sprintf(hex, "%X", r[registar1]);
            string upis = hex;
            for (; upis.size() < 4;)upis = "0" + upis;
            memorija.upisiUMemorijuNaAdresi(operand, upis);
        }

        azurirajTajmer(); 
        //azurirajTerminal();

    }
}

void Emulator::azurirajTajmer(){
    clock_t pom = clock();
    int period;
    string s1 = memorija.dohvatiSadrzajSaAdrese(65296);
    string s2 = memorija.dohvatiSadrzajSaAdrese(65297);
    s1 = s2 + s1;
    int vrednost = stoi(s1, nullptr, 16);
    switch (vrednost)
    {
    case 0:
        period = 500;
        break;
    case 1:
        period = 1000;
        break;
    case 2:
        period = 1500;
        break;
    case 3:
        period = 2000;
        break;
    case 4:
        period = 5000;
        break;
    case 5:
        period = 10000;
        break;
    case 6:
        period = 30000;
        break;
    case 7:
        period = 60000;
        break;
    default:
        exit(1);
    }

    if (period < (pom - vreme)) {
        vreme = pom;
        tajmer = true;
    }

}




string Emulator::pswUString(){
    string ret,s1,s2;
    char hex[8];
    int x = 0;
    for (int i = 0; i < 4;i++) {
        if (psw[i]) x += 1 << i;
    }
    sprintf(hex, "%X", x);
    s1 = hex;
    s1 = "0" + s1;

    x = 0;
    for (int i = 1; i < 4; i++) {
        if (psw[i+12]) x += 1 << i;
    }
    sprintf(hex, "%X", x);
    s2 = hex;
    s2 = s2 + "0";

    ret = s1 + s2;

    return ret;
}

void Emulator::stringUPSW(string s){
    string s1 = s.substr(0, 2);
    int x = stoi(s1, nullptr, 16);
    for (int i = 0; i < 4; i++) {
        psw[i] = (x % 2);
        x = x / 2;
    }

    string s2 = s.substr(2, 2);
    x = stoi(s2, nullptr, 16);
    x = x >> 5;
    for (int i = 0; i < 3; i++) {
        psw[i+13] = (x % 2);
        x = x / 2;
    }
}

int Emulator::dohvatiNacinAdresiranja(string op1, string op2){
    string nacinadresiranja = op2.substr(1, 1);
    if (nacinadresiranja == "0") {
        string s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
        r[7]++;
        string s2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
        r[7]++;
        s1 = s1 + s2;
        int ret = stoi(s1, nullptr, 16);
        return ret;
    }
    else {
        if (nacinadresiranja == "1") {
            string s1 = op1.substr(0, 1);
            int reg = stoi(s1, nullptr, 16);
            return r[reg];
        }
        else {
            if (nacinadresiranja == "2") {
                string s1 = op1.substr(0, 1);
                int index = stoi(s1, nullptr, 16);
                int reg = r[index];
                string u = op2.substr(0, 1);
                if (u == "0") {
                    int ret = dohvatiIzMemorijeDveSusedneAdrese(reg);
                    return ret;
                }
                else {
                    if (u == "1") {
                        reg = reg - 2;
                        r[index] = reg;
                        int ret = dohvatiIzMemorijeDveSusedneAdrese(reg);
                        return ret;
                    }
                    else {
                        if (u == "2") {
                            reg = reg + 2;
                            r[index] = reg;
                            int ret = dohvatiIzMemorijeDveSusedneAdrese(reg);
                            return ret;
                        }
                        else {
                            if (u == "3") {
                                int ret = dohvatiIzMemorijeDveSusedneAdrese(reg);
                                reg = reg - 2;
                                r[index] = reg;
                                return ret;
                            }
                            else {
                                if (u == "4") {
                                    int ret = dohvatiIzMemorijeDveSusedneAdrese(reg);
                                    reg = reg + 2;
                                    r[index] = reg;
                                    return ret;
                                }
                                else {
                                    //nece se de
                                }
                            }
                        }
                    }
                }

            }
            else {
                if (nacinadresiranja == "3") {
                    string oper1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
                    r[7]++;
                    string oper2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
                    r[7]++;
                    oper1 = oper2 + oper1;
                    int dodaj= stoi(oper1, nullptr, 16);
                    string s1 = op1.substr(0, 1);
                    int index = stoi(s1, nullptr, 16);
                    int reg = r[index];
                    string u = op2.substr(0, 1);
                    if (u == "0") {
                        int ret = dohvatiIzMemorijeDveSusedneAdrese(reg)+ dodaj;
                        return ret;
                    }
                    else {
                        if (u == "1") {
                            reg = reg - 2;
                            r[index] = reg;
                            int ret = dohvatiIzMemorijeDveSusedneAdrese(reg)+ dodaj;
                            return ret;
                        }
                        else {
                            if (u == "2") {
                                reg = reg + 2;
                                r[index] = reg;
                                int ret = dohvatiIzMemorijeDveSusedneAdrese(reg)+ dodaj;
                                return ret;
                            }
                            else {
                                if (u == "3") {
                                    int ret = dohvatiIzMemorijeDveSusedneAdrese(reg)+ dodaj;
                                    reg = reg - 2;
                                    r[index] = reg;
                                    return ret;
                                }
                                else {
                                    if (u == "4") {
                                        int ret = dohvatiIzMemorijeDveSusedneAdrese(reg)+ dodaj;
                                        reg = reg + 2;
                                        r[index] = reg;
                                        return ret;
                                    }
                                    else {
                                        //nece se de
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    if (nacinadresiranja == "4") {
                        string oper1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
                        r[7]++;
                        string oper2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
                        r[7]++;
                        oper1 = oper1 + oper2;
                        int r = stoi(oper1, nullptr, 16);
                        oper1 = memorija.dohvatiSadrzajSaAdrese(r);
                        r++;
                        oper2 = memorija.dohvatiSadrzajSaAdrese(r);
                        oper1 = oper2 + oper1;
                        int ret = stoi(oper1, nullptr, 16);
                        return ret;
                    }
                    else {
                        if (nacinadresiranja == "5") {
                            string s1 = op1.substr(1, 1);
                            int reg = stoi(s1, nullptr, 16); 
                            s1 = memorija.dohvatiSadrzajSaAdrese(r[7]);
                            r[7]++;
                            string s2 = memorija.dohvatiSadrzajSaAdrese(r[7]);
                            r[7]++;
                            s1 = s1 + s2;
                            int ret = stoi(s1, nullptr, 16);
                            ret += r[reg];
                            return ret;
                        }
                        else {
                            //nece se desiti
                        }
                    }
                }
            }
        }
    }
}

int Emulator::dohvatiIzMemorijeDveSusedneAdrese(int x){
    string s2 = memorija.dohvatiSadrzajSaAdrese(x);
    string s3 = memorija.dohvatiSadrzajSaAdrese(x + 1);
    s3 = s3 + s2;
    int ret = stoi(s3, nullptr, 16);
    return ret;
}
