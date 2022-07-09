#include "Asembler.h"
#include "Linker.h"
#include "Emulator.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Nedovoljan broj argumenata" << endl;
        return 1;
    }
    string s = argv[1];
    if (s=="asembler") {
        if (argc < 4) {
            cout << "Nedovoljan broj argumenata" << endl;
            return 1;
        }

        ifstream input(argv[4]);

        if (!input.is_open()) {
            cout << "Nije bilo moguce otvoriti fajl " << argv[4] << endl;
            return 1;
        }

        ofstream output(argv[3]);

        if (!output.is_open()) {
            cout << "Nije bilo moguce otvoriti fajl output" << argv[3] << endl;
            input.close();
            return 1;
        }
        Asembler a;
        a.asembliraj(input, output);
        input.close();
        output.close();
    }
    else {
        if (s == "linker") {
            bool hexa=true;
            list<string> in;
            ofstream output;
            list<string> place;
            for (int i = 2; i < argc; i++) {
                string stringic = argv[i];
                if (stringic == "-hex") hexa = true;
                else {
                    if (stringic == "-linkable") {
                        hexa = false;
                    }
                    else {
                        if (stringic == "-o") {
                            i++;
                            output.open(argv[i]);
                        } 
                        else {
                            if (stringic.find("-place=") != std::string::npos) {
                                place.push_back(stringic);
                            }
                            else {
                                in.push_back(stringic);
                            }
                        }
                    }
                }
            }


            if (!output.is_open()) {
                cout << "Nije bilo moguce otvoriti fajl output" << argv[argc-1] << endl;
                return 1;
            }

            Linker l;
            if (hexa) l.hexa(place, in, output);
            else l.linkable(in,output);
        }
        else {
            if (argc <2) {
                cout << "Nedovoljan broj argumenata" << endl;
                return 1;
            }
            if (s == "emulator") {
                ifstream input(argv[2]);

                if (!input.is_open()) {
                    cout << "Nije bilo moguce otvoriti fajl " << argv[2] << endl;
                    return 1;
                }
                Emulator e;
                e.Emuliraj(input);
                input.close();
            }
        }
    }

    return 0;

}