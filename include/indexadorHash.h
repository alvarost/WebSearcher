#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include "tokenizador.h"
#include "indexadorInformacion.h"
#include "stemmer.h"

using namespace std::tr1;

#ifndef INDEXADORHASH_H_
#define INDEXADORHASH_H_

class IndexadorHash {
	friend ostream& operator<<(ostream& s, const IndexadorHash& p) {
		s << "Fichero con el listado de palabras de parada: "
				<< p.ficheroStopWords << endl;
		s << "Delimitadores usados en el tokenizador: " << p.tok;
		s << "Directorio donde se almacenará el índice generado: "
				<< p.directorioIndice << endl;
		s << "Stemmer utilizado: " << p.stemmerType << endl;
		s << "Información de la colección indexada: "
				<< p.informacionColeccionDocs;
		s << "Se almacenará parte del índice en disco duro: "
				<< p.almacenarEnDisco << endl;
		return s;
	}
public:
	IndexadorHash();
	// Crea el índice sin elementos ni palabras de parada, con todos los parámetros de configuración vacíos (p.ej. directorioIndice =ficheroStopWords = "" y stemmer = 0)
	IndexadorHash(const string& fichStopWords, const string& delimitadores,
			const string& dirIndice, const int& tipoStemmer,
			const bool& almEnDisco);
	// "fichStopWords" será el nombre del archivo que contendrá todas las palabras de parada (una palabra por cada línea del fichero) y se almacenará en el campo privado
	// "ficheroStopWords". Asimismo, almacenará todas las palabras de parada que contenga el archivo en el campo privado "stopWords", el índice de palabras de parada.
	// "delimitadores" será el string que contiene todos los delimitadores utilizados por el tokenizador (campo privado "tok")
	// "dirIndice" será el directorio del disco duro donde se almacenará el índice (campo privado "directorioIndice"). Si dirIndice="" entonces se almacenará en el directorio donde se ejecute el programa
	// "tipoStemmer" inicializará la variable privada "stemmer":
	// 0 = no se aplica stemmer: se indexa el término tal y comoaparece tokenizado
	// 1 = stemmer de Porter para español
	// 2 = stemmer de Porter para inglés
	// "almEnDisco" inicializará la variable privada "almacenarEnDisco"
	// Estos parámetros de la indexación no se cambiarán una vez creado el objeto IndexadorHash
	// Los índices (p.ej. índice, indiceDocs e informacionColeccionDocs) quedarán vacíos
	IndexadorHash(const string& directorioIndexacion);
	// Constructor para inicializar IndexadorHash a partir de una indexación previamente realizada que habrá sido almacenada en "directorioIndexacion" mediante el método "bool GuardarIndexacion()".
	// Con ello toda la parte privada se inicializará convenientemente, igual que si se acabase de indexar la colección de documentos
	~IndexadorHash();
	IndexadorHash& operator=(const IndexadorHash&);
	bool InicializarParametros(const string& fichStopWords,
			const string& delimitadores, const string& dirIndice,
			const int& tipoStemmer, const bool& almEnDisco);
	// Devolverá true si el indexador estaba previamente vacío, en cuyo caso realizará un proceso de inicialización idéntico al descrito en el
	// constructor "IndexadorHash(const string& fichStopWords, const string& delimitadores, const string& dirIndice, const int& tipoStemmer, const bool& almEnDisco)".
	// Devolverá falso en caso contrario, sin cambiar ninguno de los parámetros del indexador, mostrando el mensaje de error correspondiente (cerr)
	bool Indexar(const string& ficheroDocumentos);
	// Devuelve true si consigue crear el índice para la colección de documentos detallada en ficheroDocumentos, el cual contendrá un nombre de documento por línea.
	// Devuelve falso si no finaliza la indexación (p.ej. por falta de memoria), mostrando el mensaje de error correspondiente, indicando el documento y término en el que se ha quedado.
	// En el caso que aparezcan documentos repetidos o que ya estuviesen previamente indexados (ha de coincidir el nombre del documento y el directorio en que se encuentre), se
	// mostrará el mensaje de excepción correspondiente, y se re-indexarán (borrar el documento previamente indexado e indexar el nuevo) en caso que la fecha de modificación del
	// documento sea más reciente que la almacenada previamente (class "InfDoc" campo "fechaModificacion")
	// En caso que no existiese el directorio directorioIndice, habría que crearlo previamente
	bool IndexarDirectorio(const string& dirAIndexar);
	// Devuelve true si consigue crear el índice para la colección de documentos que se encuentra en el directorio (y subdirectorios que contenga) dirAIndexar (independientemente de la extensión de los mismos).
	// Se considerará que todos los documentos del directorio serán ficheros de texto.
	// Devuelve falso si no finaliza la indexación (p.ej. por falta de memoria), mostrando el mensaje de error correspondiente, indicando el documento y término en el que se ha quedado.
	// En el caso que aparezcan documentos repetidos o que ya estuviesen previamente indexados (ha de coincidir el nombre del documento y el directorio en que se encuentre), se
	// mostrará el mensaje de excepción correspondiente, y se re-indexarán (borrar el documento previamente indexado e indexar el nuevo) en caso que la fecha de modificación del
	// documento sea más reciente que la almacenada previamente (class "InfDoc" campo "fechaModificacion")
	// En caso que no existiese el directorio directorioIndice, habría que crearlo previamente
	bool GuardarIndexacion();
	// Se guardará en disco duro (directorio contenido en la variable privada "directorioIndice") la indexación actualmente en memoria. La forma de almacenamiento la determinará el alumno. El objetivo es que
	// esta indexación se pueda recuperar posteriormente mediante el constructor "IndexadorHash(const string& directorioIndexacion)". Por ejemplo, supongamos que se ejecuta esta secuencia de comandos:
	// "IndexadorHash a("./fichStopWords.txt", "[ ,.", "./dirIndexPrueba", 0, false); a.Indexar("./fichConDocsAIndexar.txt"); a.GuardarIndexacion();", entonces mediante
	// el comando: "IndexadorHash b("./dirIndexPrueba");" se recuperará la indexación realizada en la secuencia anterior, cargándola en "b"
	// Devuelve falso si no finaliza la operación (p.ej. por falta de memoria), mostrando el mensaje de error correspondiente
	// En caso que no existiese el directorio directorioIndice, habría que crearlo previamente
	void ImprimirIndexacion() {
		cout << "Términos indexados: ";
		ListarTerminos();
		// A continuación aparecerá un listado del contenido del campo privado "índice" donde para cada término indexado se imprimirá: cout << termino << '\t' << InformacionTermino;
		cout << "Documentos indexados: ";
		ListarDocs();
		// A continuación aparecerá un listado del contenido del campo privado "indiceDocs" donde para cada documento indexado se imprimirá: cout << nomDoc << '\t' << InfDoc;
	}
	bool IndexarPregunta(const string& preg);
	// Devuelve true si consigue crear el índice para la pregunta "preg". Antes de realizar la indexación vaciará los campos privados indicePregunta e infPregunta
	// Generará la misma información que en la indexación de documentos, pero dejándola toda accesible en memoria principal (mediante las variables privadas "pregunta, indicePregunta, infPregunta")
	// Devuelve falso si no finaliza la operación (p.ej. por falta de memoria), mostrando el mensaje de error correspondiente
	bool DevuelvePregunta(string& preg);
	// Devuelve true si hay una pregunta indexada (diferente de la cadena vacía ""), devolviéndola en "preg"
	bool DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf);
	// Devuelve true si word está indexado en la pregunta, devolviendo su información almacenada "inf". En caso que no esté, devolvería "inf" vacío
	bool DevuelvePregunta(InformacionPregunta& inf);
	// Devuelve true si hay una pregunta indexada, devolviendo su información almacenada (campo privado "infPregunta") en "inf". En caso que no esté, devolvería "inf" vacío
	void ImprimirIndexacionPregunta() {
		cout << "Pregunta indexada: " << pregunta << endl;
		cout << "Términos indexados en la pregunta: " << endl;
		// A continuación aparecerá un listado del contenido de "indicePregunta" donde para cada término indexado se imprimirá:	cout << termino << '\t' << InformacionTerminoPregunta;
		cout << "Información de la pregunta: " << infPregunta;
	}
	bool Devuelve(const string& word, InformacionTermino& inf);
	// Devuelve true si word está indexado, devolviendo su información almacenada "inf". En caso que no esté, devolvería "inf" vacío
	bool Devuelve(const string& word, const string& nomDoc, InfTermDoc& InfDoc);
	// Devuelve true si word está indexado y aparece en el documento de nombre nomDoc, en cuyo caso devuelve la información almacenada para word
	// en el documento. En caso que no esté, devolvería "InfDoc" vacío
	bool Existe(const string& word);
	// Devuelve true si word aparece como término indexado
	bool Borra(const string& word);
	// Devuelve true si se realiza el borrado (si word aparece como término indexado)
	bool BorraDoc(const string& nomDoc);
	// Devuelve true si nomDoc está indexado y se realiza el borrado de todos los términos del documento y del documento en los campos privados
	// "indiceDocs" e "informacionColeccionDocs"
	void VaciarIndice();
	// Borra todos los términos de indice
	bool Actualiza(const string& word, const InformacionTermino& inf);
	// Será true si word está indexado, sustituyendo la información almacenada por "inf"
	bool Inserta(const string& word, const InformacionTermino& inf);
	// Será true si se realiza la inserción (p.ej. si word no estaba previamente indexado)
	//void Inserta(const string& word, const string& nomDoc); El profesor dijo que no había que implementarlo, hay un mensaje en el campus
	// Inserta el término "word" que está en el documento "nomDoc" (frecuencia == 1)
	int NumPalIndexadas();
	// Devolverá el número de términos indexados
	string DevolverFichPalParada();
	// Devuelve el contenido del campo privado "ficheroStopWords"
	void ListarPalParada();
	// Mostrará por pantalla las palabras de parada almacenadas: una palabra por línea (salto de línea al final de cada palabra)
	void ListarTerminosPregunta();
	// Mostrará por pantalla los términos indexados de la pregunta
	int NumPalParada();
	// Devolverá el número de palabras de parada almacenadas
	string DevolverDelimitadores();
	// Devuelve los delimitadores utilizados por el tokenizador
	string DevolverDirIndice();
	// Devuelve "directorioIndice" (el directorio del disco duro donde se almacenará el índice)
	int DevolverTipoStemming();
	// Devolverá el tipo de stemming realizado en la indexación de acuerdo con el valor indicado en la variable privada "stemmer"
	bool DevolverAlmEnDisco();
	// Devolverá el valor indicado en la variable privada "almEnDisco"
	void ListarInfColeccDocs();
	// Mostrar por pantalla: cout << informacionColeccionDocs;
	void ListarTerminos();
	// Mostrar por pantalla el contenido el contenido del campo privado "índice": cout << termino << '\t' << InformacionTermino;
	bool ListarTerminos(const string& nomDoc);
	// Devuelve true si nomDoc existe en la colección y muestra por pantalla todos los términos indexados del documento con nombre "nomDoc": cout <<
	// termino << '\t' << InformacionTermino;
	void ListarDocs();
	// Mostrar por pantalla el contenido el contenido del campo privado "indiceDocs": cout << nomDoc << '\t' << InfDoc;
	bool ListarDocs(const string& nomDoc);

	bool isAlmacenarEnDisco() const {
		return almacenarEnDisco;
	}

	void setAlmacenarEnDisco(bool almacenarEnDisco) {
		this->almacenarEnDisco = almacenarEnDisco;
	}

	const string& getDirectorioIndice() const {
		return directorioIndice;
	}

	void setDirectorioIndice(const string& directorioIndice) {
		this->directorioIndice = directorioIndice;
	}

	const string& getDirectorioInfDocDisco() const {
		return directorioInfDocDisco;
	}

	void setDirectorioInfDocDisco(const string& directorioInfDocDisco) {
		this->directorioInfDocDisco = directorioInfDocDisco;
	}

	const string& getFicheroStopWords() const {
		return ficheroStopWords;
	}

	void setFicheroStopWords(const string& ficheroStopWords) {
		this->ficheroStopWords = ficheroStopWords;
	}

	int getIdContDoc() const {
		return idContDoc;
	}

	void setIdContDoc(int idContDoc) {
		this->idContDoc = idContDoc;
	}

	const unordered_map<string, InformacionTermino>& getIndice() const {
		return indice;
	}

	void setIndice(const unordered_map<string, InformacionTermino>& indice) {
		this->indice = indice;
	}

	const unordered_map<string, InfDoc>& getIndiceDocs() const {
		return indiceDocs;
	}

	void setIndiceDocs(const unordered_map<string, InfDoc>& indiceDocs) {
		this->indiceDocs = indiceDocs;
	}

	const unordered_map<string, InformacionTerminoPregunta>& getIndicePregunta() const {
		return indicePregunta;
	}

	void setIndicePregunta(
			const unordered_map<string, InformacionTerminoPregunta>& indicePregunta) {
		this->indicePregunta = indicePregunta;
	}

	const InfColeccionDocs& getInformacionColeccionDocs() const {
		return informacionColeccionDocs;
	}

	void setInformacionColeccionDocs(
			const InfColeccionDocs& informacionColeccionDocs) {
		this->informacionColeccionDocs = informacionColeccionDocs;
	}

	const InformacionPregunta& getInfPregunta() const {
		return infPregunta;
	}

	void setInfPregunta(const InformacionPregunta& infPregunta) {
		this->infPregunta = infPregunta;
	}

	const unordered_map<string, int>& getListaPalabraParada() const {
		return listaPalabraParada;
	}

	void setListaPalabraParada(
			const unordered_map<string, int>& listaPalabraParada) {
		this->listaPalabraParada = listaPalabraParada;
	}

	const string& getPregunta() const {
		return pregunta;
	}

	void setPregunta(const string& pregunta) {
		this->pregunta = pregunta;
	}

	int getStemmerType() const {
		return stemmerType;
	}

	void setStemmerType(int stemmerType) {
		this->stemmerType = stemmerType;
	}

	const unordered_set<string>& getStopWords() const {
		return stopWords;
	}

	void setStopWords(const unordered_set<string>& stopWords) {
		this->stopWords = stopWords;
	}

	const Tokenizador& getTok() const {
		return tok;
	}

	void setTok(const Tokenizador& tok) {
		this->tok = tok;
	}

	// Devuelve true si nomDoc existe en la colección y muestra por pantalla el contenido el contenido del campo privado "indiceDocs" para el
	// documento con nombre "nomDoc": cout << nomDoc << '\t' << InfDoc;

private:
	//Métodos para simular el unordered_map cuando almacenarEnDisco está a true
	void CrearDirectorioInfDocDisco();
	bool InsertarInfDocDisco(string nomdoc, InfDoc InfAInsertar);
	bool BorrarInfDocDisco(string nomdoc);
	InfDoc BuscarInfDocDisco(string nomdoc);
	void GuardarInfDocDisco();
	string ObtenerNombreInfDocDisco(string fichero);
	void ListarInfDocDisco();

	//Método para indexar las palabras de parada
	void CargarPalabrasParada(const string& nomFichero);

	//Métodos para obtener los stat de los ficheros
	void ObtenerFechaFichero(const string& rutaFichero, Date& fecha);
	int ObtenerTamanyoFichero(const string& rutaFichero);

	//Métodos para guardar y leer los ficheros en el constructor que lee una indexación guardada y en el método para guardar estas indexaciones
	void GuardarIndiceTermino();
	void GuardarIndiceDocs();
	void GuardarIndicePregunta();
	void GuardarVariables();

	void LeerIndiceTermino();
	void LeerIndiceDocs();
	void LeerIndicePregunta();
	void LeerVariables();

	unordered_map<string, int> listaPalabraParada;
	unordered_map<string, InformacionTermino> indice;
	// Índice de términos indexados accesible por el término
	unordered_map<string, InfDoc> indiceDocs;
	// Índice de documentos indexados accesible por el nombre del documento
	// Si el campo privado almacenarEnDisco == true entonces InfDoc se almacenará en disco duro
	InfColeccionDocs informacionColeccionDocs;
	// Información recogida de la colección de documentos indexada
	string pregunta;
	// Pregunta indexada actualmente. Si no hay ninguna indexada, contendría el valor ""
	unordered_map<string, InformacionTerminoPregunta> indicePregunta;
	// Índice de términos indexados en una pregunta. Se almacenará en memoria principal
	InformacionPregunta infPregunta;
	// Información recogida de la pregunta indexada. Se almacenará en memoria principal
	unordered_set<string> stopWords;
	// Palabras de parada. Se almacenará en memoria principal
	string ficheroStopWords;
	// Nombre del fichero que contiene las palabras de parada
	Tokenizador tok;
	// Tokenizador que se usará en la indexación
	string directorioIndice;
	// "directorioIndice" será el directorio del disco duro donde se almacenará el índice. En caso que contenga la cadena vacía se creará en
	// el directorio donde se ejecute el indexador
	int stemmerType;
	// 0 = no se aplica stemmer: se indexa el término tal y como aparecetokenizado
	// 1 = stemmer de Porter para español
	// 2 = stemmer de Porter para inglés
	// Para el stemmer de Porter se utilizarán los archivos stemmer.cc y stemmer.h, concretamente las funciones de nombre "stemmer"
	bool almacenarEnDisco;
	// Si es true se almacenará la parte de los índices descrita en el enunciado en disco duro. En caso contrario se almacenará to-do el índice
	// en memoria principal
	int idContDoc;
	string directorioInfDocDisco;
};

#endif /* INDEXADORHASH_H_ */
