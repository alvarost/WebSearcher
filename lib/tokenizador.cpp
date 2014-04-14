#include "tokenizador.h"

ostream& operator <<(ostream& char_traits, const Tokenizador& tokenizador) {
	char_traits << tokenizador.delimiters << endl;
	return char_traits;
}

Tokenizador::Tokenizador(const Tokenizador& tokenizador) {
	delimiters = tokenizador.delimiters;
}

Tokenizador::Tokenizador(const string& delimitadoresPalabra) {
	delimiters = delimitadoresPalabra;
}

Tokenizador::Tokenizador() {
	delimiters = "";
}

Tokenizador::~Tokenizador() {
	delimiters = "";
}

Tokenizador& Tokenizador::operator =(const Tokenizador& tokenizador) {
	delimiters = tokenizador.delimiters;
	return (*this);
}

void Tokenizador::Tokenizar(const string& str, list<string>& tokens) {
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void Tokenizador::Tokenizar(const string& str, string::size_type& posInicio,
		string::size_type& posFinal) {
	posInicio = str.find_first_not_of(delimiters, posInicio);
	posFinal = str.find_first_of(delimiters, posInicio);
}

void Tokenizador::Tokenizar(const string& i, const string& f) {
	ifstream iv;
	ofstream fv;
	string cadena;
	list<string> tokens;

	iv.open(i.c_str());
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << i << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena.length() != 0) {
				Tokenizar(cadena, tokens);
			}
		}
	}
	iv.close();

	fv.open(f.c_str());
	list<string>::iterator itS;
	for (itS = tokens.begin(); itS != tokens.end(); itS++) {
		fv << (*itS) << endl;
	}
	fv.close();
}

void Tokenizador::Tokenizar(const string& i) {
	Tokenizar(i, i + ".tk");
}

void Tokenizador::TokenizarListaFicheros(const string& i) {
	ifstream iv;
	string cadena;

	iv.open(i.c_str());
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << i << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena.length() != 0) {
				Tokenizar(cadena);
			}
		}
	}
	iv.close();
}

void Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters) {
	delimiters = nuevoDelimiters;
}

void Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters) {
	string::size_type lastPos = nuevoDelimiters.find_first_of(delimiters, 0);
	string::size_type pos = nuevoDelimiters.find_first_not_of(delimiters,
			lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		delimiters = delimiters
				+ (nuevoDelimiters.substr(lastPos, pos - lastPos));
		lastPos = nuevoDelimiters.find_first_of(delimiters, pos);
		pos = nuevoDelimiters.find_first_not_of(delimiters, lastPos);
	}

}

void Tokenizador::TokenizarFichero(const string& rutaFichero,
		list<string>& tokens) {
	ifstream iv;
	string cadena;

	iv.open(rutaFichero.c_str());
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << rutaFichero << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena.length() != 0) {
				Tokenizar(cadena, tokens);
			}
		}
	}
	iv.close();
}
