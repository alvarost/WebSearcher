#include "buscador.h"
#include <dirent.h>
#include <sstream>


// ResultadoRI
ResultadoRI::ResultadoRI(const double& kvSimilitud, const long int& kidDoc,
		const int& np) {
	vSimilitud = kvSimilitud;
	idDoc = kidDoc;
	numPregunta = np;
}
double ResultadoRI::VSimilitud() const {
	return vSimilitud;
}
long int ResultadoRI::IdDoc() const {
	return idDoc;
}
bool ResultadoRI::operator<(const ResultadoRI& lhs) const {
	if (numPregunta == lhs.numPregunta)
		return (vSimilitud < lhs.vSimilitud);
	else
		return (numPregunta > lhs.numPregunta);
}
ostream&
operator<<(ostream &os, const ResultadoRI &res) {
	os << res.vSimilitud << "\t\t" << res.idDoc << "\t" << res.numPregunta
			<< endl;
	return os;
}

// Buscador
Buscador::Buscador(const string& directorioIndexacion, const int& f) :
		IndexadorHash(directorioIndexacion) {
	formSimilitud = f;
	c = 2;
	k1 = 1.2;
	b = 0.75;
	numPreguntas = 1;
}

Buscador::Buscador(const Buscador& busc) :
		IndexadorHash(busc) {
	formSimilitud = busc.formSimilitud;
	c = busc.c;
	k1 = busc.k1;
	b = busc.b;
	numPreguntas = 1;
}

Buscador::~Buscador() {
	//El profesor lo deja vacío en el PDF así que DON'T TOUCH
}

Buscador& Buscador::operator =(const Buscador& busc) {
	formSimilitud = busc.formSimilitud;
	c = busc.c;
	k1 = busc.k1;
	b = busc.b;

	return *this;
}

bool Buscador::Buscar(const int& numDocumentos) {
	this->docsOrdenados = priority_queue<ResultadoRI>();
	priority_queue<ResultadoRI> colaAux;

	unordered_map<string,InfDoc>::const_iterator itdoc = this->getIndiceDocs().begin();

	double avrld = 0;
	unordered_map<string,InfDoc> inddocs = this->getIndiceDocs();
	for(unordered_map<string,InfDoc>::const_iterator itdoc = inddocs.begin(); itdoc != inddocs.end(); itdoc++){
		avrld += itdoc->second.getTamBytes();
	}
	avrld /= inddocs.size();

	double avgdl;
	for(unordered_map<string,InfDoc>::const_iterator itdoc = this->getIndiceDocs().begin(); itdoc != this->getIndiceDocs().end(); itdoc++){
		avgdl += itdoc->second.getNumPalSinParada();
	}
	avgdl /= inddocs.size();

	for(;itdoc != this->getIndiceDocs().end(); itdoc++){
		double simil = 0.0;
		int np = 0;
		if(formSimilitud){
			simil = this->BuscarBM25(itdoc->first,itdoc->second, avgdl);
		} else{
			simil = this->BuscarDFR(itdoc->first,itdoc->second, avrld);
		}
		ResultadoRI ri = ResultadoRI(simil, itdoc->second.getIdDoc(), np);
		ri.setNomDoc(itdoc->first);
		colaAux.push(ri);
	}

	// Meto en los resultados los numDocumentos más relevantes
	for(int i=0; i<numDocumentos; i++){
		this->docsOrdenados.push(colaAux.top());
		colaAux.pop();
	}
	return true;
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos,
		const int& numPregInicio, const int& numPregFin) {

	// Realizará la búsqueda entre el número de pregunta “numPregInicio” y “numPregFin”,
	// ambas preguntas incluidas. El corpus de preguntas estará en el directorio
	// “dirPreguntas”, y tendrá la estructura de cada pregunta en un fichero
	// independiente, de nombre el número de pregunta, y extensión “.txt”
	// (p.ej. 1.txt 2.txt 3.txt … 83.txt). Esto significa que habrá que indexar
	// cada pregunta por separado y ejecutar una búsqueda por cada pregunta
	// añadiendo los resultados de cada pregunta (junto con su número de pregunta)
	// en la variable privada “docsOrdenados”.
	// Se guardarán los primeros “numDocumentos” documentos más relevantes para
	// cada pregunta en la variable privada “docsOrdenados”
	// La búsqueda se realiza con la fórmula de similitud indicada en la
	// variable privada “formSimilitud”.
	// Devuelve falso si no finaliza la búsqueda (p.ej. por falta de memoria),
	// mostrando el mensaje de error correspondiente, indicando el documento,
	// pregunta y término en el que se ha quedado.

	//Not implemented yet

	numPreguntas = numPregFin - numPregInicio + 1;

	priority_queue<ResultadoRI> colaAux;

	string refactorDirAIndexar = dirPreguntas;
	ofstream fo;
	string cadena = "";
	fo.open("data2.tmp", ios::out);

	double avrld = 0;
	unordered_map<string,InfDoc> inddocs = this->getIndiceDocs();
	for(unordered_map<string,InfDoc>::const_iterator itdoc = inddocs.begin(); itdoc != inddocs.end(); itdoc++){
		avrld += itdoc->second.getTamBytes();
	}
	avrld /= inddocs.size();

	double avgdl;
	for(unordered_map<string,InfDoc>::const_iterator itdoc = this->getIndiceDocs().begin(); itdoc != this->getIndiceDocs().end(); itdoc++){
		avgdl += itdoc->second.getNumPalSinParada();
	}
	avgdl /= inddocs.size();

	this->docsOrdenados = priority_queue<ResultadoRI>();

	if (dirPreguntas.substr(0, 2) == "./")
		refactorDirAIndexar.erase(0, 2);

	for(int i = numPregInicio; i < numPregFin+1; i++) {
		stringstream ss;
		ss << i;

		string rutacompleta = refactorDirAIndexar + "/" + ss.str() + ".txt";
		ifstream fi (rutacompleta);

		getline(fi,cadena);

		this->setPregunta(cadena);

		this->IndexarPregunta(this->getPregunta());

		unordered_map<string,InfDoc>::const_iterator itdoc = this->getIndiceDocs().begin();

		colaAux = priority_queue<ResultadoRI>();

		for(;itdoc != this->getIndiceDocs().end(); itdoc++){
			double simil = 0.0;

			if(formSimilitud){
				simil = this->BuscarBM25(itdoc->first,itdoc->second, avgdl);
			} else{
				simil = this->BuscarDFR(itdoc->first,itdoc->second, avrld);
			}

			ResultadoRI ri = ResultadoRI(simil, itdoc->second.getIdDoc(), i);
			ri.setNomDoc(itdoc->first);
			colaAux.push(ri);
		}

		// Meto en los resultados los numDocumentos más relevantes
		for(int i=0; i<numDocumentos; i++){
			this->docsOrdenados.push(colaAux.top());
			colaAux.pop();
		}
	}

	return true;
}

void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const {
	priority_queue<ResultadoRI> copia = priority_queue<ResultadoRI>(this->docsOrdenados);
	for(int i=0; i<numDocumentos*numPreguntas; i++){
		ResultadoRI ri = copia.top();
		cout << ri.getNumPregunta() << " ";
		if(formSimilitud)
			cout << "BM25" << " ";
		else cout << "DFR" << " ";
		cout << ri.getNomDoc() << " " << i << " " << ri.getSimilitud() << " ";
		if(ri.getNumPregunta() == 0)
			cout << this->getPregunta() << endl;
		else {
			cout << "ConjuntoDePreguntas" << endl;
		}
		copia.pop();
	}
}

bool Buscador::ImprimirResultadoBusqueda(const int& numDocumentos,
		const string& nombreFichero) const {
	ofstream fo (nombreFichero);
	priority_queue<ResultadoRI> copia = priority_queue<ResultadoRI>(this->docsOrdenados);
	for(int i=0; i<numDocumentos*numPreguntas; i++){
		ResultadoRI ri = copia.top();
		cout << ri.getNumPregunta() << " ";
		fo << ri.getNumPregunta() << " ";
		if(formSimilitud){
			cout << "BM25" << " ";
			fo << "BM25" << " ";
		}
		else{
			cout << "DFR" << " ";
			fo << "DFR" << " ";
		}
		cout << ri.getNomDoc() << " " << i << " " << ri.getSimilitud() << " ";
		fo << ri.getNomDoc() << " " << i << " " << ri.getSimilitud() << " ";
		if(ri.getNumPregunta() == 0){
			cout << this->getPregunta() << endl;
			fo << this->getPregunta() << endl;
		}
		else {
			cout << "ConjuntoDePreguntas" << endl;
			fo << "ConjuntoDePreguntas" << endl;
		}
		copia.pop();
		}
	fo.close();
	return true;
}

int Buscador::DevolverFormulaSimilitud() const {
	return formSimilitud;
}

bool Buscador::CambiarFormulaSimilitud(const int& f) {
	if (f == 0 || f == 1) {
		formSimilitud = f;
		return true;
	} else
		return false;
}

void Buscador::CambiarParametrosDFR(const double& kc) {
	c = kc;
}

double Buscador::DevolverParametrosDFR() const {
	return c;
}

void Buscador::CambiarParametrosBM25(const double& kk1, const double& kb) {
	k1 = kk1;
	b = kb;
}

void Buscador::DevolverParametrosBM25(double& kk1, double& kb) const {
	kk1 = k1;
	kb = b;
}
Buscador::Buscador() {
	formSimilitud = 0;
	c = 2;
	k1 = 1.2;
	b = 0.75;
	numPreguntas = 1;
}

double Buscador::BuscarDFR(const string& nomdoc, const InfDoc& doc, const double avrld) {
	unordered_map<string,InformacionTermino> indinter = this->getIndice();
	unordered_map<string,InfDoc> inddocs = this->getIndiceDocs();
	unordered_map<string,InformacionTerminoPregunta> indtp = this->getIndicePregunta();

	double wtq;
	double wtd;
	double result = 0;
	// Primero itero para hacerme una lista de las palabras de la pregunta que me hará falta para el ft
	// Calculo la media de bytes de los documentos

	for(unordered_map<string,InformacionTerminoPregunta>::const_iterator itpre = indtp.begin(); itpre != indtp.end(); itpre++){
		// Calculo ft (Las veces que aparece el término en la colección)
		//Trie<InformacionTermino> trieinter(this->getIndice());

		double ft = indinter[itpre->first].getFtc();
		// Calculo lambdat
		double lambdat = ft/inddocs.size();
		// Calculo ftd
		unordered_map<long int, InfTermDoc> docs = indinter[itpre->first].getDocs();
		InfDoc documento = inddocs[nomdoc];
		double ftd = docs[doc.getIdDoc()].getFt();
		ftd = ftd*log2(1+(c*avrld/documento.getTamBytes()));
		// Calculo wtq
		wtq = (double)itpre->second.getFt() / indtp.size();
		// Calculo wtd
		if(lambdat != 0 && indinter[itpre->first].getFd() != 0)
			wtd = (log2(1+lambdat)+ftd*log2((1+lambdat)/lambdat))*((ft+1)/(indinter[itpre->first].getFd()*(ftd + 1)));
		else
			wtd = 0;

		result += wtq * wtd;
	}
	return result;
}

double Buscador::BuscarBM25(const string& nomdoc, const InfDoc& doc, const double avgdl) {
	unordered_map<string,InformacionTermino> indinter = this->getIndice();
	// Calculo la media de todas las |D|

	double score = 0.0;

	for(unordered_map<string,InformacionTerminoPregunta>::const_iterator itpre = this->getIndicePregunta().begin(); itpre != this->getIndicePregunta().end(); itpre++){
		// Calculo el IDF del termino de la pregunta
		InformacionTermino it = indinter[itpre->first];
		double idf = log10((this->getIndiceDocs().size() - it.getDocs().size() + 0.5) / (it.getDocs().size() + 0.5));
		unordered_map<long int, InfTermDoc> docs = it.getDocs();
		double tmp1 = ((double)docs[doc.getIdDoc()].getFt()*(k1 + 1.0));
		double tmp2 = ((double)docs[doc.getIdDoc()].getFt() + (k1 * (1.0 - b + (b*((double)doc.getNumPalSinParada() / avgdl)))));

		score += idf*
					(
							((double)docs[doc.getIdDoc()].getFt()*(k1 + 1.0))
							/
							((double)docs[doc.getIdDoc()].getFt() + (k1 * (1.0 - b + (b*((double)doc.getNumPalSinParada() / avgdl)))))
					);
	}
	return score;
}
