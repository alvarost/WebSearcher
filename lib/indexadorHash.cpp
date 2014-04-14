#include "indexadorHash.h"

#include <dirent.h>
#include <vector>
#include <algorithm>

IndexadorHash::IndexadorHash() {
	this->pregunta = "";
	this->almacenarEnDisco = false;
	this->stemmerType = 0;
	this->idContDoc = 0;
	this->directorioIndice = "";
	this->ficheroStopWords = "";
	this->CargarPalabrasParada(this->ficheroStopWords);
	this->tok = Tokenizador("");
	this->directorioInfDocDisco = "ficherosDisco";
	CrearDirectorioInfDocDisco();
}

IndexadorHash::IndexadorHash(const string& fichStopWords,
		const string& delimitadores, const string& dirIndice,
		const int& tipoStemmer, const bool& almEnDisco) {
	this->stemmerType = tipoStemmer;
	this->almacenarEnDisco = almEnDisco;
	this->idContDoc = 0;
	this->directorioInfDocDisco = "ficherosDisco";
	CrearDirectorioInfDocDisco();
	this->directorioIndice = dirIndice;
	string dirACrear = directorioIndice;
	if (dirACrear.substr(0, 2) == "./")
		dirACrear.erase(0, 2);
	DIR *dir;
	string temp;
	if ((dir = opendir(dirACrear.c_str())) == NULL) {
		temp = "mkdir " + dirACrear;
		system(temp.c_str());
	}
	closedir(dir);
	this->ficheroStopWords = fichStopWords;
	this->tok = Tokenizador(delimitadores);
	this->pregunta = "";
	this->CargarPalabrasParada(fichStopWords);
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion) {
	this->directorioInfDocDisco = "ficherosDisco";
	CrearDirectorioInfDocDisco();
	this->directorioIndice = directorioIndexacion;

	LeerIndiceTermino();
	LeerIndiceDocs();
	LeerIndicePregunta();
	LeerVariables();
}

IndexadorHash::~IndexadorHash() {
	string tmp;
	tmp = "rm -r " + directorioInfDocDisco;
	system(tmp.c_str());

	tmp = "rm data.tmp";
	ifstream iv;
	iv.open("data.tmp");
	if (iv)
		system(tmp.c_str());
	iv.close();

	tmp = "rm data2.tmp";
	iv.open("data2.tmp");
	if (iv)
		system(tmp.c_str());
	iv.close();
}

IndexadorHash& IndexadorHash::operator =(const IndexadorHash& original) {
	this->directorioInfDocDisco = "ficherosDisco";
	CrearDirectorioInfDocDisco();
	this->almacenarEnDisco = original.almacenarEnDisco;
	this->pregunta = original.pregunta;
	this->stemmerType = original.stemmerType;
	this->stopWords = original.stopWords;
	this->tok = original.tok;
	this->idContDoc = original.idContDoc;
	this->indice = original.indice;
	this->indiceDocs = original.indiceDocs;
	this->directorioIndice = original.directorioIndice;
	this->ficheroStopWords = original.ficheroStopWords;
	this->indicePregunta = original.indicePregunta;
	this->infPregunta = original.infPregunta;
	this->informacionColeccionDocs = original.informacionColeccionDocs;
	return *this;
}

bool IndexadorHash::InicializarParametros(const string& fichStopWords,
		const string& delimitadores, const string& dirIndice,
		const int& tipoStemmer, const bool& almEnDisco) {
	if (directorioIndice == "" && stemmerType == 0 && almacenarEnDisco == false
			&& tok.getDelimiters() == "" && ficheroStopWords == "") {
		this->pregunta = "";
		this->idContDoc = 0;
		this->ficheroStopWords = fichStopWords;
		this->directorioIndice = dirIndice;
		this->stemmerType = tipoStemmer;
		this->tok = Tokenizador(delimitadores);
		this->almacenarEnDisco = almEnDisco;
		this->directorioInfDocDisco = "ficherosDisco";
		CrearDirectorioInfDocDisco();
		this->CargarPalabrasParada(fichStopWords);
		return true;
	} else
		return false;
}

bool IndexadorHash::Indexar(const string& ficheroDocumentos) {
	ifstream iv;
	int palabrasDocumento = 0;
	int idDocumentoAInsertar = 0;
	string nomFichero;
	string termino;
	list<string> tokens;
	iv.open(ficheroDocumentos.c_str());
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << ficheroDocumentos << endl;
	else {
		while (!iv.eof()) {
			InfDoc doc;
			palabrasDocumento = 0;
			getline(iv, nomFichero);
			if (nomFichero.length() != 0) {
				Date fecha;
				ObtenerFechaFichero(nomFichero, fecha);
				bool check = false;
				if (almacenarEnDisco == false) {
					if (indiceDocs.find(nomFichero) != indiceDocs.end()) {
						if (indiceDocs[nomFichero].getFechaModificacion() < fecha) {
							idDocumentoAInsertar = indiceDocs.find(nomFichero)->second.getIdDoc();
							BorraDoc(nomFichero);
						}
					}
					if (indiceDocs.find(nomFichero) == indiceDocs.end())
						check = true;
					else
						check = false;
				} else {
					if (BuscarInfDocDisco(nomFichero).getNumPal() != 0) {
						if (BuscarInfDocDisco(nomFichero).getFechaModificacion() < fecha) {
							idDocumentoAInsertar = BuscarInfDocDisco(nomFichero).getIdDoc();
							BorrarInfDocDisco(nomFichero);
						}
					}
					if (BuscarInfDocDisco(nomFichero).getNumPal() == 0)
						check = true;
					else
						check = false;
				}
				if (check) {
					idContDoc++;
					if (idDocumentoAInsertar == 0)
						idDocumentoAInsertar = idContDoc;
					tokens.clear();
					tok.TokenizarFichero(nomFichero, tokens);
					ifstream iv2;
					list<string>::iterator itS;
					for (itS = tokens.begin(); itS != tokens.end(); itS++) {
						termino = (*itS);
						unordered_map<string, int>::const_iterator got1 =
								listaPalabraParada.find(termino);
						doc.setNumPal(doc.getNumPal() + 1);
						palabrasDocumento++;
						if (got1 == listaPalabraParada.end()) {
							doc.setNumPalSinParada(
									doc.getNumPalSinParada() + 1);
							stemmer(termino, this->stemmerType);
							unordered_map<string, InformacionTermino>::const_iterator got2 =
									indice.find(termino);
							if (got2 == indice.end()) {
								InformacionTermino infPalabra;
								infPalabra.setFd(1);
								infPalabra.setFtc(1);
								InfTermDoc itd;
								itd.setFt(1);
								list<int> posDoc;
								posDoc.push_back(palabrasDocumento - 1);
								itd.setPosTerm(posDoc);
								unordered_map<long int, InfTermDoc> listaDocs;
								listaDocs[idDocumentoAInsertar] = itd;
								infPalabra.setDocs(listaDocs);
								indice[termino] = infPalabra;
								doc.setNumPalDiferentes(
										doc.getNumPalDiferentes() + 1);
							} else {
								indice[termino].setFtc(
										indice[termino].getFtc() + 1);
								unordered_map<long int, InfTermDoc>::const_iterator got3 =
										indice[termino].getDocs().find(
												idDocumentoAInsertar);
								if (got3 == indice[termino].getDocs().end()) {
									indice[termino].setFd(
											indice[termino].getFd() + 1);
									doc.setNumPalDiferentes(
											doc.getNumPalDiferentes() + 1);
									InfTermDoc informacionNuevoDocumento;
									informacionNuevoDocumento.setFt(1);
									list<int> listPos;
									listPos.push_back(palabrasDocumento - 1);
									informacionNuevoDocumento.setPosTerm(
											listPos);
									unordered_map<long int, InfTermDoc> listDocs =
											indice[termino].getDocs();
									listDocs[idDocumentoAInsertar] = informacionNuevoDocumento;
									indice[termino].setDocs(listDocs);
								} else {
									unordered_map<long int, InfTermDoc> infoTermList =
											indice[termino].getDocs();
									InfTermDoc infoTerm =
											infoTermList[idDocumentoAInsertar];
									infoTerm.setFt(infoTerm.getFt() + 1);
									list<int> listPos = infoTerm.getPosTerm();
									listPos.push_back(palabrasDocumento - 1);
									infoTerm.setPosTerm(listPos);
									infoTermList[idDocumentoAInsertar] =
											infoTerm;
									indice[termino].setDocs(infoTermList);
								}
							}
						}
					}

					doc.setFechaModificacion(fecha);
					doc.setIdDoc(idDocumentoAInsertar);
					doc.setTamBytes(ObtenerTamanyoFichero(nomFichero));
					if (almacenarEnDisco == false)
						indiceDocs[nomFichero] = doc;
					else
						InsertarInfDocDisco(nomFichero, doc);
					informacionColeccionDocs.setNumDocs(
							informacionColeccionDocs.getNumDocs() + 1);
					informacionColeccionDocs.setNumTotalPal(
							informacionColeccionDocs.getNumTotalPal()
									+ doc.getNumPal());
					informacionColeccionDocs.setNumTotalPalSinParada(
							informacionColeccionDocs.getNumTotalPalSinParada()
									+ doc.getNumPalSinParada());
					informacionColeccionDocs.setTamBytes(
							informacionColeccionDocs.getTamBytes()
									+ doc.getTamBytes());
					idDocumentoAInsertar = 0;
				}

			}
		}
	}
	iv.close();
	return true;
}

bool IndexadorHash::IndexarDirectorio(const string& dirAIndexar) {
	vector<string> fileNamesToSort;
	string refactorDirAIndexar = dirAIndexar;
	DIR *dir;
	ofstream fo;
	string tmp = "";
	fo.open("data2.tmp", ios::out);
	struct dirent *ent;
	if (dirAIndexar.substr(0, 2) == "./")
		refactorDirAIndexar.erase(0, 2);
	if ((dir = opendir(refactorDirAIndexar.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			tmp = ent->d_name;
			if ((tmp != ".") && (tmp != ".."))
				fileNamesToSort.push_back(ent->d_name);
		}

		closedir(dir);
		sort(fileNamesToSort.begin(), fileNamesToSort.end());
		for (unsigned int i = 0; i < fileNamesToSort.size(); i++)
			fo << refactorDirAIndexar << "/" << fileNamesToSort[i] << endl;
		fo.close();
		Indexar("data2.tmp");
	} else {
		cerr << "Error" << endl;
		return false;
	}
	return true;
}

bool IndexadorHash::GuardarIndexacion() {
	this->GuardarIndiceTermino();
	if (almacenarEnDisco == false)
		this->GuardarIndiceDocs();
	else
		this->GuardarInfDocDisco();
	this->GuardarIndicePregunta();
	this->GuardarVariables();
	return true;
}

bool IndexadorHash::IndexarPregunta(const string& preg) {
	this->indicePregunta.clear();
	this->infPregunta.~InformacionPregunta();
	pregunta = preg;
	list<string> tokens;
	tok.Tokenizar(pregunta, tokens);
	list<string>::iterator stringIt;
	for (stringIt = tokens.begin(); stringIt != tokens.end(); stringIt++) {
		string termino = (*stringIt);
		this->infPregunta.setNumTotalPal(infPregunta.getNumTotalPal() + 1);
		unordered_map<string, int>::const_iterator mapIt =
				listaPalabraParada.find(termino);
		if (mapIt == listaPalabraParada.end()) {
			this->infPregunta.setNumTotalPalSinParada(
					infPregunta.getNumTotalPalSinParada() + 1);
			stemmer(termino, this->stemmerType);
			if (indicePregunta.find(termino) == indicePregunta.end()) {
				InformacionTerminoPregunta ITPIt;
				ITPIt.setFt(1);
				list<int> pos;
				pos.push_back(this->infPregunta.getNumTotalPal() - 1);
				ITPIt.setPosTerm(pos);
				this->indicePregunta[termino] =	ITPIt;
			} else {
				InformacionTerminoPregunta itp = indicePregunta[termino];
				indicePregunta[termino].setFt(
						indicePregunta[termino].getFt() + 1);
				list<int> pos = indicePregunta[termino].getPosTerm();
				pos.push_back(this->infPregunta.getNumTotalPal() - 1);
				indicePregunta[termino].setPosTerm(pos);
			}
		}
	}
	return true;
}

bool IndexadorHash::DevuelvePregunta(string& preg) {
	if (pregunta != "") {
		preg = pregunta;
		return true;
	}
	return false;
}

bool IndexadorHash::DevuelvePregunta(const string& word,
		InformacionTerminoPregunta& inf) {
	unordered_map<string, InformacionTerminoPregunta>::const_iterator temp =
			indicePregunta.find(word);
	if (temp != indicePregunta.end()) {
		inf = temp->second;
		return true;
	}
	inf = InformacionTerminoPregunta();
	return false;
}

bool IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) {
	string preg = "";
	if (DevuelvePregunta(preg)) {
		inf = infPregunta;
		return true;
	}
	inf = InformacionPregunta();
	return false;
}

bool IndexadorHash::Devuelve(const string& word, InformacionTermino& inf) {
	unordered_map<string, InformacionTermino>::const_iterator temp =
			indice.find(word);
	if (temp != indice.end()) {
		inf = temp->second;
		return true;
	}
	inf = InformacionTermino();
	return false;
}

bool IndexadorHash::Devuelve(const string& word, const string& nomDoc,
		InfTermDoc& InfoDoc) {
	string refactorNomDoc = nomDoc;
	InformacionTermino inf = InformacionTermino();
	if (refactorNomDoc.substr(0, 2) == "./")
		refactorNomDoc.erase(0, 2);

	if (Devuelve(word, inf)) {
		if (almacenarEnDisco == false) {
			unordered_map<string, InfDoc>::const_iterator itdoc =
					indiceDocs.find(refactorNomDoc);
			if (itdoc != indiceDocs.end()) {
				unordered_map<long int, InfTermDoc>::const_iterator temp =
						inf.getDocs().find(itdoc->second.getIdDoc());
				if (temp != inf.getDocs().end()) {
					InfoDoc = temp->second;
					return true;
				}
			}
		} else {
			InfDoc itdoc = BuscarInfDocDisco(refactorNomDoc);
			if (itdoc.getIdDoc() != 0) {
				unordered_map<long int, InfTermDoc>::const_iterator temp =
						inf.getDocs().find(itdoc.getIdDoc());
				if (temp != inf.getDocs().end()) {
					InfoDoc = temp->second;
					return true;
				}
			}
		}
	}
	InfoDoc = InfTermDoc();
	return false;
}

bool IndexadorHash::Existe(const string& word) {
	return (indice.find(word) != indice.end());
}

bool IndexadorHash::Borra(const string& word) {
	if (Existe(word)) {
		indice.erase(word);
		return true;
	}
	return false;
}

bool IndexadorHash::BorraDoc(const string& nomDoc) {
	if (almacenarEnDisco == false) {
		unordered_map<string, InfDoc>::const_iterator documentIt =
				indiceDocs.find(nomDoc);
		if (documentIt != indiceDocs.end()) {
			InfDoc documentInfo = documentIt->second;
			for (unordered_map<string, InformacionTermino>::const_iterator mapIterator =
					indice.begin(); mapIterator != indice.end();
					mapIterator++) {
				InformacionTermino terminoInfo = mapIterator->second;
				string termino = mapIterator->first;
				unordered_map<long int, InfTermDoc>::const_iterator infTermDocIt =
						terminoInfo.getDocs().find(documentInfo.getIdDoc());

				if (infTermDocIt != terminoInfo.getDocs().end()) {
					int ftcToSet = terminoInfo.getFtc()
							- infTermDocIt->second.getFt();
					int fdToSet = terminoInfo.getFd();
					fdToSet--;
					terminoInfo.setFtc(ftcToSet);
					terminoInfo.setFd(fdToSet);
					unordered_map<long int, InfTermDoc> docs =
							terminoInfo.getDocs();
					docs.erase(documentInfo.getIdDoc());
					terminoInfo.setDocs(docs);
					Borra(termino);
					if (!docs.empty())
						indice[termino] = terminoInfo;
				}
			}
			informacionColeccionDocs.setNumDocs(
					informacionColeccionDocs.getNumDocs() - 1);
			informacionColeccionDocs.setNumTotalPal(
					informacionColeccionDocs.getNumTotalPal()
							- documentInfo.getNumPal());
			informacionColeccionDocs.setNumTotalPalSinParada(
					informacionColeccionDocs.getNumTotalPalSinParada()
							- documentInfo.getNumPalSinParada());
			informacionColeccionDocs.setTamBytes(
					informacionColeccionDocs.getTamBytes()
							- documentInfo.getTamBytes());
			indiceDocs.erase(nomDoc);
			return true;
		}
	} else
		return BorrarInfDocDisco(nomDoc);
	return false;
}

void IndexadorHash::VaciarIndice() {
	indice.clear();
}

bool IndexadorHash::Actualiza(const string& word,
		const InformacionTermino& inf) {
	if (Existe(word)) {
		indice[word] = inf;
		return true;
	} else {
		cerr << "ERROR: En actualiza no existe la palabra " << word << endl;
		return false;
	}
}

bool IndexadorHash::Inserta(const string& word, const InformacionTermino& inf) {
	if (Existe(word))
		return false;
	else {
		indice[word] = inf;
		return true;
	}
}

int IndexadorHash::NumPalIndexadas() {
	return indice.size();
}

string IndexadorHash::DevolverFichPalParada() {
	return ficheroStopWords;
}

void IndexadorHash::ListarPalParada() {
	ifstream iv;
	string cadena;
	iv.open(DevolverFichPalParada().c_str());
	if (!iv)
		cerr << "ERROR: No existe el archivo de las palabras de parada" << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena.length() != 0)
				cout << cadena << endl;
		}
	}
}

void IndexadorHash::ListarTerminosPregunta() {
	for (unordered_map<string, InformacionTerminoPregunta>::iterator i =
			indicePregunta.begin(); i != indicePregunta.end(); i++) {
		cout << i->first << '\t' << i->second << endl;
	}
}

int IndexadorHash::NumPalParada() {
	ifstream iv;
	string cadena;
	int numPalabraParada = 0;
	iv.open(DevolverFichPalParada().c_str(), ios::in);
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << "data.tmp" << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena.length() != 0)
				numPalabraParada++;
		}
	}
	return numPalabraParada;
}

string IndexadorHash::DevolverDelimitadores() {
	return tok.getDelimiters();
}

string IndexadorHash::DevolverDirIndice() {
	return directorioIndice;
}

int IndexadorHash::DevolverTipoStemming() {
	return stemmerType;
}

bool IndexadorHash::DevolverAlmEnDisco() {
	return almacenarEnDisco;
}

void IndexadorHash::ListarInfColeccDocs() {
	cout << informacionColeccionDocs;
}

void IndexadorHash::ListarTerminos() {
	for (unordered_map<string, InformacionTermino>::iterator i = indice.begin();
			i != indice.end(); i++)
		cout << i->first << '\t' << i->second << endl;
}

bool IndexadorHash::ListarTerminos(const string& nomDoc) {
	if (indice.find(nomDoc) != indice.end()) {
		InformacionTermino temp = indice.find(nomDoc)->second;
		cout << temp << endl;
		return true;
	}
	return false;
}

void IndexadorHash::ListarDocs() {
	if (almacenarEnDisco == false)
		for (unordered_map<string, InfDoc>::iterator i = indiceDocs.begin();
				i != indiceDocs.end(); i++)
			cout << i->first << '\t' << i->second << endl;
	else
		ListarInfDocDisco();
}

bool IndexadorHash::ListarDocs(const string& nomDoc) {
	if (almacenarEnDisco == false) {
		if (indiceDocs.find(nomDoc) != indiceDocs.end()) {
			InfDoc temp = indiceDocs.find(nomDoc)->second;
			cout << temp << endl;
			return true;
		}
	} else {
		InfDoc temp = BuscarInfDocDisco(nomDoc);
		if (temp.getNumPal() != 0) {
			cout << temp << endl;
			return true;
		} else
			return false;
	}
	return false;
}

void IndexadorHash::CrearDirectorioInfDocDisco() {
	DIR *dir;
	string tmp;
	if ((dir = opendir(directorioInfDocDisco.c_str())) == NULL) {
		tmp = "mkdir " + directorioInfDocDisco;
		system(tmp.c_str());
	}
	closedir(dir);
}

bool IndexadorHash::InsertarInfDocDisco(string nomdoc, InfDoc InfAInsertar) {
	string previousNomdoc = nomdoc;
	ofstream of;
	string toreplace = "/";
	string replaceby = "-";
	size_t pos = 0;
	while ((pos = nomdoc.find(toreplace, pos)) != std::string::npos) {
		nomdoc.replace(pos, toreplace.length(), replaceby);
		pos += replaceby.length();
	}
	of.open((directorioInfDocDisco + "/" + nomdoc + ".infd").c_str(), ios::out);
	if (!of)
		cerr << "Error para abrir los ficheros a escribir" << endl;
	of << previousNomdoc << endl;

	of << InfAInsertar.getIdDoc() << "\t" << InfAInsertar.getNumPal() << "\t"
			<< InfAInsertar.getNumPalSinParada() << "\t"
			<< InfAInsertar.getNumPalDiferentes() << "\t"
			<< InfAInsertar.getTamBytes() << "\t"
			<< InfAInsertar.getFechaModificacion().getHora() << ":"
			<< InfAInsertar.getFechaModificacion().getMinutos() << ":"
			<< InfAInsertar.getFechaModificacion().getSegundos() << ":"
			<< InfAInsertar.getFechaModificacion().getMilisegundos() << ";"
			<< InfAInsertar.getFechaModificacion().getDia() << "/"
			<< InfAInsertar.getFechaModificacion().getMes() << "/"
			<< InfAInsertar.getFechaModificacion().getAnyo() << endl;
	of.close();

	return true;
}

bool IndexadorHash::BorrarInfDocDisco(string nomdoc) {
	InfDoc documentInfo = BuscarInfDocDisco(nomdoc);
	if (documentInfo.getNumPal() != 0) {
		for (unordered_map<string, InformacionTermino>::const_iterator informacionTerminoIt =
				indice.begin(); informacionTerminoIt != indice.end();
				informacionTerminoIt++) {
			InformacionTermino terminoInfo = informacionTerminoIt->second;
			string termino = informacionTerminoIt->first;
			unordered_map<long int, InfTermDoc>::const_iterator infTermDocIt =
					terminoInfo.getDocs().find(documentInfo.getIdDoc());
			if (infTermDocIt != terminoInfo.getDocs().end()) {
				int ftcToSet = terminoInfo.getFtc()
						- infTermDocIt->second.getFt();
				int fdToSet = terminoInfo.getFd();
				fdToSet--;
				terminoInfo.setFtc(ftcToSet);
				terminoInfo.setFd(fdToSet);
				unordered_map<long int, InfTermDoc> docs =
						terminoInfo.getDocs();
				docs.erase(documentInfo.getIdDoc());
				terminoInfo.setDocs(docs);
				Borra(termino);
				if (!docs.empty())
					indice[termino] =terminoInfo;
			}
		}
		informacionColeccionDocs.setNumDocs(
				informacionColeccionDocs.getNumDocs() - 1);
		informacionColeccionDocs.setNumTotalPal(
				informacionColeccionDocs.getNumTotalPal()
						- documentInfo.getNumPal());
		informacionColeccionDocs.setNumTotalPalSinParada(
				informacionColeccionDocs.getNumTotalPalSinParada()
						- documentInfo.getNumPalSinParada());
		informacionColeccionDocs.setTamBytes(
				informacionColeccionDocs.getTamBytes()
						- documentInfo.getTamBytes());

		string toreplace = "/";
		string replaceby = "-";
		size_t pos = 0;
		while ((pos = nomdoc.find(toreplace, pos)) != std::string::npos) {
			nomdoc.replace(pos, toreplace.length(), replaceby);
			pos += replaceby.length();
		}
		string temp = "rm " + directorioInfDocDisco + "/" + nomdoc + ".infd";
		if (!system(temp.c_str()))
			return true;
		else
			return false;
	}
	return false;
}

InfDoc IndexadorHash::BuscarInfDocDisco(string nomdoc) {
	ifstream iv;
	string lectura;
	string toreplace = "/";
	string replaceby = "-";
	size_t position = 0;
	while ((position = nomdoc.find(toreplace, position)) != std::string::npos) {
		nomdoc.replace(position, toreplace.length(), replaceby);
		position += replaceby.length();
	}
	iv.open((directorioInfDocDisco + "/" + nomdoc + ".infd").c_str());
	if (!iv)
		return InfDoc();
	else {
		string indiceDoc = "";
		getline(iv, indiceDoc);
		int iddoc, numpal, numpalsinpar, numpaldif, tambytes, fhora, fminutos,
				fsegundos, fmilisegundos, fdia, fmes, fanyo = 0;
		getline(iv, lectura);
		sscanf(lectura.c_str(), "%d\t%d\t%d\t%d\t%d\t%d:%d:%d:%d;%d/%d/%d",
				&iddoc, &numpal, &numpalsinpar, &numpaldif, &tambytes, &fhora,
				&fminutos, &fsegundos, &fmilisegundos, &fdia, &fmes, &fanyo);
		InfDoc doc;
		doc.setIdDoc((long int) iddoc);
		doc.setNumPal(numpal);
		doc.setNumPalSinParada(numpalsinpar);
		doc.setNumPalDiferentes(numpaldif);
		doc.setTamBytes(tambytes);

		Date fecha;
		fecha.setHora(fhora);
		fecha.setMinutos(fminutos);
		fecha.setSegundos(fsegundos);
		fecha.setMilisegundos(fmilisegundos);
		fecha.setDia(fdia);
		fecha.setMes(fmes);
		fecha.setAnyo(fanyo);

		doc.setFechaModificacion(fecha);
		return doc;
	}
}

void IndexadorHash::GuardarInfDocDisco() {
	ofstream of;
	of.open((directorioIndice + "/" + "documentos.tmp").c_str(), ios::out);
	vector<string> fileNames;
	DIR *dir;
	string temp = "";
	struct dirent *ent;
	if ((dir = opendir(directorioInfDocDisco.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			temp = ent->d_name;
			if ((temp != ".") && (temp != ".."))
				fileNames.push_back(ent->d_name);
		}

		closedir(dir);
		sort(fileNames.begin(), fileNames.end());
		InfDoc informacionAMostrar;
		for (unsigned int i = 0; i < fileNames.size(); i++) {
			informacionAMostrar = BuscarInfDocDisco(fileNames[i]);
			of << ObtenerNombreInfDocDisco(fileNames[i]) << endl;
			of << informacionAMostrar.getIdDoc() << "\t"
					<< informacionAMostrar.getNumPal() << "\t"
					<< informacionAMostrar.getNumPalSinParada() << "\t"
					<< informacionAMostrar.getNumPalDiferentes() << "\t"
					<< informacionAMostrar.getTamBytes() << "\t"
					<< informacionAMostrar.getFechaModificacion() << endl;
		}
	} else {
		cerr << "Error" << endl;
	}
	of.close();

}

string IndexadorHash::ObtenerNombreInfDocDisco(string fichero) {
	string lectura;
	ifstream iv;
	iv.open((directorioInfDocDisco + "/" + fichero).c_str());
	if (!iv)
		cerr << "Error para abrir el fichero" << endl;
	else {
		getline(iv, lectura);
		return lectura;
	}
	return "";

}

void IndexadorHash::ListarInfDocDisco() {
	vector<string> vector;
	DIR *dir;
	string temp = "";
	struct dirent *ent;
	if ((dir = opendir(directorioInfDocDisco.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			temp = ent->d_name;
			if ((temp != ".") && (temp != ".."))
				vector.push_back(ent->d_name);
		}
		closedir(dir);
		sort(vector.begin(), vector.end());
		for (unsigned int i = 0; i < vector.size(); i++) {
			cout << ObtenerNombreInfDocDisco(vector[i]) << '\t'
					<< BuscarInfDocDisco(ObtenerNombreInfDocDisco(vector[i]))
					<< endl;
		}
	} else {
		cerr << "Error" << endl;
	}
}

void IndexadorHash::CargarPalabrasParada(const string& nomFichero) {
	ifstream iv;
	string cadena;

	iv.open(nomFichero.c_str());
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << nomFichero << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena.length() != 0)
				listaPalabraParada[cadena] = 1;
		}
	}
}

void IndexadorHash::ObtenerFechaFichero(const string& rutaFichero,
		Date& fecha) {
	string temp = "stat -c %y " + rutaFichero + " > data.tmp";
	system(temp.c_str());
	ifstream iv;
	string cadena;
	iv.open("data.tmp");
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << "data.tmp" << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena != "") {
				fecha.setAnyo(atoi(cadena.substr(0, 4).c_str()));
				fecha.setMes(atoi(cadena.substr(5, 7).c_str()));
				fecha.setDia(atoi(cadena.substr(8, 10).c_str()));
				fecha.setHora(atoi(cadena.substr(11, 13).c_str()));
				fecha.setMinutos(atoi(cadena.substr(14, 16).c_str()));
				fecha.setSegundos(atoi(cadena.substr(17, 19).c_str()));
				fecha.setMilisegundos(atoi(cadena.substr(20, 29).c_str()));
			}
		}
	}
	iv.close();
}

int IndexadorHash::ObtenerTamanyoFichero(const string& rutaFichero) {
	string temp = "stat -c %s " + rutaFichero + " > data.tmp";
	int result = 0;
	system(temp.c_str());
	ifstream iv;
	string cadena;
	iv.open("data.tmp");
	if (!iv)
		cerr << "ERROR: No existe el archivo: " << "data.tmp" << endl;
	else {
		while (!iv.eof()) {
			cadena = "";
			getline(iv, cadena);
			if (cadena != "") {
				result = (atoi(cadena.c_str()));
			}
		}
	}
	iv.close();
	return result;
}

void IndexadorHash::GuardarIndiceTermino() {
	ofstream of;
	of.open((directorioIndice + "/" + "terminos.tmp").c_str(), ios::out);
	unordered_map<string, InformacionTermino>::iterator it;
	for (it = indice.begin(); it != indice.end(); it++) {
		of << it->first << endl;
		of << it->second.getFd() << "\t" << it->second.getFtc() << endl;
		unordered_map<long int, InfTermDoc> docs = it->second.getDocs();
		unordered_map<long int, InfTermDoc>::iterator itDocs;
		of << docs.size() << "\t" << endl;
		for (itDocs = docs.begin(); itDocs != docs.end(); itDocs++) {
			of << itDocs->first << "\t" << itDocs->second.getFt() << "\t";
			list<int> posiciones = itDocs->second.getPosTerm();
			list<int>::iterator posIt;
			of << posiciones.size() << endl;
			for (posIt = posiciones.begin(); posIt != posiciones.end(); posIt++)
				of << *posIt << endl;
		}
	}
	of.close();
}

void IndexadorHash::GuardarIndiceDocs() {
	ofstream of;
	of.open((directorioIndice + "/" + "documentos.tmp").c_str(), ios::out);
	unordered_map<string, InfDoc>::iterator itd;
	for (itd = indiceDocs.begin(); itd != indiceDocs.end(); itd++) {
		of << itd->first << endl;
		of << itd->second.getIdDoc() << "\t" << itd->second.getNumPal() << "\t"
				<< itd->second.getNumPalSinParada() << "\t"
				<< itd->second.getNumPalDiferentes() << "\t"
				<< itd->second.getTamBytes() << "\t"
				<< itd->second.getFechaModificacion() << endl;
	}
	of.close();
}

void IndexadorHash::GuardarIndicePregunta() {
	ofstream of;
	of.open((directorioIndice + "/" + "terminospregunta.tmp").c_str(),
			ios::out);
	unordered_map<string, InformacionTerminoPregunta>::iterator it;
	for (it = indicePregunta.begin(); it != indicePregunta.end(); it++) {
		of << it->first << endl;
		of << it->second.getFt() << endl;
		list<int> posiciones = it->second.getPosTerm();
		list<int>::iterator posIt;
		of << posiciones.size() << endl;
		for (posIt = posiciones.begin(); posIt != posiciones.end(); posIt++)
			of << *posIt << endl;
	}
}

void IndexadorHash::GuardarVariables() {
	ofstream of;
	of.open((directorioIndice + "/" + "parametros.tmp").c_str(), ios::out);
	of << informacionColeccionDocs.getNumDocs() << "\t"
			<< informacionColeccionDocs.getNumTotalPal() << "\t"
			<< informacionColeccionDocs.getNumTotalPalSinParada() << "\t"
			<< informacionColeccionDocs.getTamBytes() << endl;
	of << pregunta << endl;
	of << infPregunta.getNumTotalPal() << "\t"
			<< infPregunta.getNumTotalPalSinParada() << endl;
	of << ficheroStopWords << endl;
	of << tok.getDelimiters() << endl;
	of << directorioIndice << endl;
	of << stemmerType << endl;
	of << almacenarEnDisco << endl;
	of << idContDoc;
	of.close();
}

void IndexadorHash::LeerIndiceTermino() {
	ifstream ifs;
	string lectura;
	ifs.open((directorioIndice + "/" + "terminos.tmp").c_str(), ios::in);
	while (!ifs.eof()) {
		string cadena = "";
		getline(ifs, cadena);
		if (cadena != "") {
			int tempftc = 0;
			int tempfd = 0;
			unordered_map<long int, InfTermDoc> templdocs;
			getline(ifs, lectura);
			sscanf(lectura.c_str(), "%d\t%d", &tempfd, &tempftc);
			int numDocs = 0;
			getline(ifs, lectura);
			numDocs = atoi(lectura.c_str());
			for (int i = 0; i < numDocs; i++) {
				int indiceDoc = 0;
				int ftDoc = 0;
				int possizeDoc = 0;
				list<int> posTermDoc;
				getline(ifs, lectura);
				sscanf(lectura.c_str(), "%d\t%d\t%d", &indiceDoc, &ftDoc,
						&possizeDoc);
				for (int c = 0; c < possizeDoc; c++) {
					getline(ifs, lectura);
					posTermDoc.push_back(atoi(lectura.c_str()));
				}
				InfTermDoc ifd;
				ifd.setFt(ftDoc);
				ifd.setPosTerm(posTermDoc);
				templdocs.insert(make_pair((long int) indiceDoc, ifd));
			}
			InformacionTermino infoter;
			infoter.setFd(tempfd);
			infoter.setFtc(tempftc);
			infoter.setDocs(templdocs);
			indice.insert(make_pair(cadena, infoter));
		}
	}
}

void IndexadorHash::LeerIndiceDocs() {
	ifstream ifs;
	string lectura;
	ifs.open((directorioIndice + "/" + "documentos.tmp").c_str(), ios::in);
	while (!ifs.eof()) {
		string indiceDoc = "";
		getline(ifs, indiceDoc);
		if (indiceDoc != "") {
			int iddoc = 0;
			int numpal = 0;
			int numpalsinpar = 0;
			int numpaldif = 0;
			int tambytes = 0;
			int fdia = 0;
			int fmes = 0;
			int fanyo = 0;
			getline(ifs, lectura);
			sscanf(lectura.c_str(), "%d\t%d\t%d\t%d\t%d\t%d/%d/%d", &iddoc,
					&numpal, &numpalsinpar, &numpaldif, &tambytes, &fdia, &fmes,
					&fanyo);
			InfDoc doc;
			doc.setIdDoc((long int) iddoc);
			doc.setNumPal(numpal);
			doc.setNumPalSinParada(numpalsinpar);
			doc.setNumPalDiferentes(numpaldif);
			doc.setTamBytes(tambytes);
			Date fecha;
			fecha.setDia(fdia);
			fecha.setMes(fmes);
			fecha.setAnyo(fanyo);
			doc.setFechaModificacion(fecha);
			if (almacenarEnDisco == false)
				indiceDocs.insert(make_pair(indiceDoc, doc));
			else
				InsertarInfDocDisco(indiceDoc, doc);
		}
	}
}

void IndexadorHash::LeerIndicePregunta() {
	ifstream ifs;
	string lectura;
	ifs.open((directorioIndice + "/" + "terminospregunta.tmp").c_str(),
			ios::in);
	while (!ifs.eof()) {
		string indicePre = "";
		getline(ifs, indicePre);
		if (indicePre != "") {
			getline(ifs, lectura);
			int ftpreg = atoi(lectura.c_str());
			getline(ifs, lectura);
			int numPos = atoi(lectura.c_str());
			list<int> posTermPreg;
			for (int i = 0; i < numPos; i++) {
				getline(ifs, lectura);
				posTermPreg.push_back(atoi(lectura.c_str()));
			}
			InformacionTerminoPregunta infterpre;
			infterpre.setFt(ftpreg);
			infterpre.setPosTerm(posTermPreg);
			indicePregunta.insert(make_pair(indicePre, infterpre));
		}
	}
}

void IndexadorHash::LeerVariables() {
	ifstream ifs;
	string lectura;
	ifs.open((directorioIndice + "/" + "parametros.tmp").c_str(), ios::in);
	int icdnumdocs, icdnumtotalpal, icdnumtotalpalsinpar, icdtambytes;
	getline(ifs, lectura);
	sscanf(lectura.c_str(), "%d\t%d\t%d\t%d", &icdnumdocs, &icdnumtotalpal,
			&icdnumtotalpalsinpar, &icdtambytes);
	informacionColeccionDocs.setNumDocs(icdnumdocs);
	informacionColeccionDocs.setNumTotalPal(icdnumtotalpal);
	informacionColeccionDocs.setNumTotalPalSinParada(
			(long int) icdnumtotalpalsinpar);
	informacionColeccionDocs.setTamBytes(icdtambytes);
	getline(ifs, lectura);
	pregunta = lectura;
	int ipnumtotalpal, ipnumtotalpalsinpar;
	getline(ifs, lectura);
	sscanf(lectura.c_str(), "%d\t%d", &ipnumtotalpal, &ipnumtotalpalsinpar);
	infPregunta.setNumTotalPal(ipnumtotalpal);
	infPregunta.setNumTotalPalSinParada(ipnumtotalpalsinpar);
	getline(ifs, lectura);
	ficheroStopWords = lectura;
	getline(ifs, lectura);
	tok.setDelimiters(lectura);
	getline(ifs, lectura);
	directorioIndice = lectura;
	getline(ifs, lectura);
	stemmerType = atoi(lectura.c_str());
	getline(ifs, lectura);
	almacenarEnDisco = atoi(lectura.c_str());
	getline(ifs, lectura);
	idContDoc = atoi(lectura.c_str());
	ifs.close();
}
