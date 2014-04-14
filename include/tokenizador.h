#include <iostream>
#include <string.h>
#include <list>
#include <fstream>

using namespace std;

#ifndef TOKENIZADOR_H_
#define TOKENIZADOR_H_

class Tokenizador {
	friend ostream& operator<<(ostream&, const Tokenizador&); //Imprime
	//"delimiters" y realiza un salto de línea
public:
	Tokenizador(const Tokenizador&);
	Tokenizador(const string& delimitadoresPalabra);
	Tokenizador();
	~Tokenizador();
	Tokenizador& operator=(const Tokenizador&);

	void Tokenizar(const string& str, list<string>& tokens);
	//Tokeniza
	// str devolviendo el resultado en tokens
	void Tokenizar(const string& str, string::size_type& posInicio,
			string::size_type& posFinal);
	//Tokeniza str desde su posición
	//posInicio devolviendo la posición inicial y final del término
	//extraído en posInicio y posFinal
	void Tokenizar(const string& i, const string& f);
	//Tokeniza el
	//fichero i guardando la salida en el fichero f
	//(una palabra en cada línea del fichero)
	void Tokenizar(const string & i);
	//Tokeniza el fichero i guardando la
	//salida en un fichero de nombre i añadiéndole extensión .tk, y que
	//contendrá una palabra en cada línea del fichero
	void TokenizarListaFicheros(const string& i);
	//Tokeniza el fichero i
	//que contiene un nombre de fichero por línea guardando la
	//salida en un fichero cuyo nombre será el de entrada
	//añadiéndole extensión .tk, y que contendrá una palabra en cada
	//línea del fichero
	void DelimitadoresPalabra(const string& nuevoDelimiters);
	//Cambia
	//"delimiters" por "nuevoDelimiters"
	void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters);

	const string& getDelimiters() const {
		return delimiters;
	}

	void setDelimiters(const string& delimiters) {
		this->delimiters = delimiters;
	}

	// Tokeniza un fichero insertando los tokens en la lista.
	void TokenizarFichero(const string& rutaFichero, list<string> &tokens);

	//Añade
	//a "delimiters" los nuevos delimitadores que aparezcan en
	//"nuevoDelimiters" (no se permiten caracteres repetidos)

private:
	string delimiters; //Delimitadores de términos
};

#endif /* TOKENIZADOR_H_ */
