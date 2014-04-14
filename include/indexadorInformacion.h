#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "tokenizador.h"

using namespace std::tr1;

#ifndef INDEXADORINFORMACION_H_
#define INDEXADORINFORMACION_H_

class Date {
	friend ostream& operator<<(ostream& s, const Date& p);

public:
	Date();
	Date(long int milisegundos, int segundos, int hora, int minutos, int dia,
			int mes, int anyo);
	Date(Date &original);
	~Date();
	Date & operator=(const Date &);
	bool operator<(const Date& toCompare) const;
	bool operator>(const Date& toCompare) const;

	int getAnyo() const {
		return anyo;
	}

	void setAnyo(int anyo) {
		this->anyo = anyo;
	}

	int getDia() const {
		return dia;
	}

	void setDia(int dia) {
		this->dia = dia;
	}

	int getHora() const {
		return hora;
	}

	void setHora(int hora) {
		this->hora = hora;
	}

	int getMes() const {
		return mes;
	}

	void setMes(int mes) {
		this->mes = mes;
	}

	int getMinutos() const {
		return minutos;
	}

	void setMinutos(int minutos) {
		this->minutos = minutos;
	}

	int getSegundos() const {
		return segundos;
	}

	void setSegundos(int segundos) {
		this->segundos = segundos;
	}

	long int getMilisegundos() const {
		return milisegundos;
	}

	void setMilisegundos(long int milisegundos) {
		this->milisegundos = milisegundos;
	}

private:
	long int milisegundos;
	int segundos;
	int hora;
	int minutos;
	int dia;
	int mes;
	int anyo;
};

class InfTermDoc {
	friend ostream& operator<<(ostream& s, const InfTermDoc& p);
public:
	InfTermDoc(const InfTermDoc &);
	InfTermDoc();
	// Inicializa ft = 0
	~InfTermDoc();
	// Pone ft = 0
	InfTermDoc & operator=(const InfTermDoc &);

	int getFt() const {
		return ft;
	}

	void setFt(int ft) {
		this->ft = ft;
	}

	const list<int>& getPosTerm() const {
		return posTerm;
	}

	void setPosTerm(const list<int>& posTerm) {
		this->posTerm = posTerm;
	}

	// Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
	int ft;
	// Frecuencia del término en el documento
	list<int> posTerm;
	// Lista de números de palabra en los que aparece el término en el documento. Los números de palabra comenzarán desde cero (la primera
	//// palabra del documento). Esta información siempre se debe almacenar en disco duro
	//
	//	ostream& operator<<(ostream& s, const InfTermDoc& p) {
	//		s << "ft: " << p.ft;
	//// A continuación se mostrarían todos los elementos de p.posTerm ("posición TAB posición TAB ... posición, es decir nunca finalizará en un TAB"): s << "\t" << posicion;
	//		s << endl;
	//		return s;
	//	}
};

class InfDoc {
	//Hay que implementar el operador de comparación == para los métodos de almacenar en Disco
	friend ostream& operator<<(ostream& s, const InfDoc& p);
public:
	InfDoc(const InfDoc &);
	InfDoc();
	~InfDoc();
	InfDoc & operator=(const InfDoc &);

	const Date& getFechaModificacion() {
		return fechaModificacion;
	}

	void setFechaModificacion(const Date& fechaModificacion) {
		this->fechaModificacion = fechaModificacion;
	}

	long int getIdDoc() const {
		return idDoc;
	}

	void setIdDoc(long int idDoc) {
		this->idDoc = idDoc;
	}

	int getNumPal() const {
		return numPal;
	}

	void setNumPal(int numPal) {
		this->numPal = numPal;
	}

	int getNumPalDiferentes() const {
		return numPalDiferentes;
	}

	void setNumPalDiferentes(int numPalDiferentes) {
		this->numPalDiferentes = numPalDiferentes;
	}

	int getNumPalSinParada() const {
		return numPalSinParada;
	}

	void setNumPalSinParada(int numPalSinParada) {
		this->numPalSinParada = numPalSinParada;
	}

	int getTamBytes() const {
		return tamBytes;
	}

	void setTamBytes(int tamBytes) {
		this->tamBytes = tamBytes;
	}

	// Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
	long int idDoc; // Identificador del documento
	int numPal;
	// No total de palabras del documento
	int numPalSinParada;
	// No total de palabras sin stop-words del documento
	int numPalDiferentes;
	// No total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
	int tamBytes;
	// Tamaño en bytes del documento
	Date fechaModificacion;
	// Atributo correspondiente a la fecha de modificación del documento. El tipo "Fecha" lo elegirá/implementará el alumno
};

class InfColeccionDocs {
	friend ostream& operator<<(ostream& s, const InfColeccionDocs& p);
public:
	InfColeccionDocs(const InfColeccionDocs &);
	InfColeccionDocs();
	~InfColeccionDocs();
	InfColeccionDocs & operator=(const InfColeccionDocs &);

	long int getNumDocs() const {
		return numDocs;
	}

	void setNumDocs(long int numDocs) {
		this->numDocs = numDocs;
	}

	long int getNumTotalPal() const {
		return numTotalPal;
	}

	void setNumTotalPal(long int numTotalPal) {
		this->numTotalPal = numTotalPal;
	}

	long int getNumTotalPalSinParada() const {
		return numTotalPalSinParada;
	}

	void setNumTotalPalSinParada(long int numTotalPalSinParada) {
		this->numTotalPalSinParada = numTotalPalSinParada;
	}

	long int getTamBytes() const {
		return tamBytes;
	}

	void setTamBytes(long int tamBytes) {
		this->tamBytes = tamBytes;
	}

	// Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
	long int numDocs;
	// No total de documentos en la colección
	long int numTotalPal;
	// No total de palabras en la colección

	long int numTotalPalSinParada;
	// No total de palabras sin stop-words en la colección
	long int tamBytes;
	// Tamaño total en bytes de la colección
};

class InformacionTerminoPregunta {
	friend ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p);
public:
	InformacionTerminoPregunta(const InformacionTerminoPregunta &);
	InformacionTerminoPregunta();
	~InformacionTerminoPregunta();
	InformacionTerminoPregunta & operator=(const InformacionTerminoPregunta&);
	// Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase

	void setFt(int n) {
		this->ft = n;
	}

	void setPosTerm(list<int> l) {
		this->posTerm = l;
	}

	int getFt() const {
		return ft;
	}

	const list<int>& getPosTerm() const {
		return posTerm;
	}

private:
	int ft;
	// Frecuencia total del término en la pregunta
	list<int> posTerm;
	// Lista de números de palabra en los que aparece el término en la pregunta. Los números de palabra comenzarán desde cero (la primera palabra de la pregunta)
};

class InformacionPregunta {
	friend ostream& operator<<(ostream& s, const InformacionPregunta& p);
public:
	InformacionPregunta(const InformacionPregunta &);
	InformacionPregunta();
	~InformacionPregunta();
	InformacionPregunta & operator=(const InformacionPregunta &);

	long int getNumTotalPal() const {
		return numTotalPal;
	}

	void setNumTotalPal(long int numTotalPal) {
		this->numTotalPal = numTotalPal;
	}

	long int getNumTotalPalSinParada() const {
		return numTotalPalSinParada;
	}

	void setNumTotalPalSinParada(long int numTotalPalSinParada) {
		this->numTotalPalSinParada = numTotalPalSinParada;
	}

	// Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
	long int numTotalPal;
	// No total de palabras en la pregunta
	long int numTotalPalSinParada;
	// No total de palabras sin stop-words en la pregunta
};

class InformacionTermino {
	friend ostream& operator<<(ostream& s, const InformacionTermino& p);
public:
	InformacionTermino(const int& fft, const int& ffd);
	InformacionTermino(const InformacionTermino &);
	InformacionTermino();
	// Inicializa ftc = fd = 0
	~InformacionTermino();
	// Pone ftc = fd = 0 y vacía l_docs
	InformacionTermino & operator=(const InformacionTermino &);

	int getFd() const {
		return fd;
	}

	void setFd(int fd) {
		this->fd = fd;
	}

	int getFtc() const {
		return ftc;
	}

	void setFtc(int ftc) {
		this->ftc = ftc;
	}

	const unordered_map<long int, InfTermDoc>& getDocs() const {
		return l_docs;
	}

	void setDocs(const unordered_map<long int, InfTermDoc>& docs) {
		l_docs = docs;
	}

	// Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
private:
	int ftc; // Frecuencia total del término en la colección
	int fd;
	// Número de documentos que contienen al término
	unordered_map<long int, InfTermDoc> l_docs;
	// Tabla Hash que se accederá por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene toda la información de
	// aparición del término en el documento
	// Si el campo privado del indexador almacenarEnDisco == true entonces esta información se almacenará en disco duro
};

#endif /* INDEXADORINFORMACION_H_ */
