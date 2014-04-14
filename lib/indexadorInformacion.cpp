#include "indexadorInformacion.h"
#include "indexadorHash.h"

using namespace std;

// Date //

Date::Date() {
	anyo = mes = dia = hora = minutos = segundos = milisegundos = 0;
}

Date::Date(long int milisegundos, int segundos, int hora, int minutos, int dia,
		int mes, int anyo) {
	this->milisegundos = milisegundos;
	this->segundos = segundos;
	this->hora = hora;
	this->minutos = minutos;
	this->dia = dia;
	this->mes = mes;
	this->anyo = anyo;
}

Date::Date(Date &original) {
	this->milisegundos = original.milisegundos;
	this->segundos = original.segundos;
	this->hora = original.hora;
	this->minutos = original.minutos;
	this->dia = original.dia;
	this->mes = original.mes;
	this->anyo = original.anyo;
}

Date::~Date() {
	anyo = mes = dia = hora = minutos = segundos = milisegundos = 0;
}

Date& Date::operator=(const Date& original) {
	this->milisegundos = original.milisegundos;
	this->segundos = original.segundos;
	this->hora = original.hora;
	this->minutos = original.minutos;
	this->dia = original.dia;
	this->mes = original.mes;
	this->anyo = original.anyo;
	return *this;
}

bool Date::operator<(const Date& toCompare) const {
	if (this->anyo < toCompare.anyo) {
		return true;
	} else if (this->anyo > toCompare.anyo) {
		return false;
	} else {
		if (this->mes < toCompare.mes) {
			return true;
		} else if (this->mes > toCompare.mes) {
			return false;
		} else {
			if (this->dia < toCompare.dia) {
				return true;
			} else if (this->dia > toCompare.dia) {
				return false;
			} else {
				if (this->hora < toCompare.hora) {
					return true;
				} else if (this->hora > toCompare.hora) {
					return false;
				} else {
					if (this->minutos < toCompare.minutos) {
						return true;
					} else if (this->minutos > toCompare.minutos) {
						return false;
					} else {
						if (this->segundos < toCompare.segundos) {
							return true;
						} else if (this->segundos > toCompare.segundos) {
							return false;
						} else {
							if (this->milisegundos < toCompare.milisegundos) {
								return true;
							} else if (this->milisegundos
									> toCompare.milisegundos) {
								return false;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool Date::operator>(const Date& toCompare) const {
	if (this->anyo > toCompare.anyo) {
		return true;
	} else if (this->anyo < toCompare.anyo) {
		return false;
	} else {
		if (this->mes > toCompare.mes) {
			return true;
		} else if (this->mes < toCompare.mes) {
			return false;
		} else {
			if (this->dia > toCompare.dia) {
				return true;
			} else if (this->dia < toCompare.dia) {
				return false;
			} else {
				if (this->hora > toCompare.hora) {
					return true;
				} else if (this->hora < toCompare.hora) {
					return false;
				} else {
					if (this->minutos > toCompare.minutos) {
						return true;
					} else if (this->minutos < toCompare.minutos) {
						return false;
					} else {
						if (this->segundos > toCompare.segundos) {
							return true;
						} else if (this->segundos < toCompare.segundos) {
							return false;
						} else {
							if (this->milisegundos > toCompare.milisegundos) {
								return true;
							} else if (this->milisegundos
									< toCompare.milisegundos) {
								return false;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

ostream& operator<<(ostream& s, const Date& d) {
	if (d.dia < 10)
		s << "0";
	s << d.dia << "/";
	if (d.mes < 10)
		s << "0";
	s << d.mes << "/" << d.anyo;
	return s;
}

// InfTermDoc //

InfTermDoc::InfTermDoc(const InfTermDoc& original) {
	this->ft = original.ft;
	this->posTerm = original.posTerm;
}

InfTermDoc::InfTermDoc() {
	this->ft = 0;
	this->posTerm.clear();
}

InfTermDoc::~InfTermDoc() {
	this->ft = 0;
	this->posTerm.clear();
}

InfTermDoc& InfTermDoc::operator =(const InfTermDoc& original) {
	this->ft = original.ft;
	this->posTerm = original.posTerm;
	return *this;
}

ostream& operator <<(ostream& s, const InfTermDoc& p) {
	s << "ft: " << p.ft << "\t";
	for (list<int>::const_iterator i = p.posTerm.begin(); i != p.posTerm.end();
			i++) {
		if (i == p.posTerm.begin())
			s << *i;
		else
			s << "\t" << *i;
	}
	s << endl;
	return s;
}

// InfDoc //

InfDoc::InfDoc(const InfDoc& original) {
	this->idDoc = original.idDoc;
	this->numPal = original.numPal;
	this->numPalDiferentes = original.numPalDiferentes;
	this->numPalSinParada = original.numPalSinParada;
	this->tamBytes = original.tamBytes;
	this->fechaModificacion = original.fechaModificacion;
}

InfDoc::InfDoc() {
	this->idDoc = numPal = numPalDiferentes = numPalSinParada = tamBytes = 0;
}

InfDoc::~InfDoc() {
	this->idDoc = numPal = numPalDiferentes = numPalSinParada = tamBytes = 0;
}

InfDoc& InfDoc::operator =(const InfDoc& original) {
	this->idDoc = original.idDoc;
	this->numPal = original.numPal;
	this->numPalDiferentes = original.numPalDiferentes;
	this->numPalSinParada = original.numPalSinParada;
	this->tamBytes = original.tamBytes;
	this->fechaModificacion = original.fechaModificacion;
	return *this;
}

ostream& operator<<(ostream& s, const InfDoc& p) {
	s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal
			<< "\tnumPalSinParada: " << p.numPalSinParada
			<< "\tnumPalDiferentes: " << p.numPalDiferentes << "\ttamBytes: "
			<< p.tamBytes << "\tfechaModificacion: " << p.fechaModificacion;
	return s;
}

// InfColeccionDocs //

InfColeccionDocs::InfColeccionDocs(const InfColeccionDocs& original) {
	this->numDocs = original.numDocs;
	this->numTotalPal = original.numTotalPal;
	this->numTotalPalSinParada = original.numTotalPalSinParada;
	this->tamBytes = original.tamBytes;
}

InfColeccionDocs::InfColeccionDocs() {
	this->numDocs = numTotalPal = numTotalPalSinParada = tamBytes = 0;
}

InfColeccionDocs::~InfColeccionDocs() {
	this->numDocs = numTotalPal = numTotalPalSinParada = tamBytes = 0;
}

InfColeccionDocs& InfColeccionDocs::operator =(
		const InfColeccionDocs& original) {
	this->numDocs = original.numDocs;
	this->numTotalPal = original.numTotalPal;
	this->numTotalPalSinParada = original.numTotalPalSinParada;
	this->tamBytes = original.tamBytes;
	return *this;
}

ostream& operator<<(ostream& s, const InfColeccionDocs& p) {
	s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal
			<< "\tnumTotalPalSinParada: " << p.numTotalPalSinParada
			<< "\ttamBytes: " << p.tamBytes << endl;
	return s;
}

// InformacionTerminoPregunta //

InformacionTerminoPregunta::InformacionTerminoPregunta(
		const InformacionTerminoPregunta& original) {
	this->ft = original.ft;
	this->posTerm = original.posTerm;
}

InformacionTerminoPregunta::InformacionTerminoPregunta() {
	this->ft = 0;
	this->posTerm.clear();
}

InformacionTerminoPregunta::~InformacionTerminoPregunta() {
	this->ft = 0;
	this->posTerm.clear();
}

InformacionTerminoPregunta& InformacionTerminoPregunta::operator =(
		const InformacionTerminoPregunta& original) {
	this->ft = original.ft;
	this->posTerm = original.posTerm;
	return *this;
}

ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p) {
	s << "ft: " << p.ft << "\t";
	for (list<int>::const_iterator i = p.posTerm.begin(); i != p.posTerm.end();
			i++) {
		if (i == p.posTerm.begin())
			s << *i;
		else
			s << "\t" << *i;
	}
	s << endl;
	return s;
}

// InformacionPregunta //

InformacionPregunta::InformacionPregunta(const InformacionPregunta& original) {
	this->numTotalPal = original.numTotalPal;
	this->numTotalPalSinParada = original.numTotalPalSinParada;
}

InformacionPregunta::InformacionPregunta() {
	this->numTotalPal = numTotalPalSinParada = 0;
}

InformacionPregunta::~InformacionPregunta() {
	this->numTotalPal = numTotalPalSinParada = 0;
}

InformacionPregunta& InformacionPregunta::operator =(
		const InformacionPregunta& original) {
	this->numTotalPal = original.numTotalPal;
	this->numTotalPalSinParada = original.numTotalPalSinParada;
	return *this;
}

ostream& operator<<(ostream& s, const InformacionPregunta& p) {
	s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: "
			<< p.numTotalPalSinParada;
	s << endl;
	return s;
}

// InformacionTermino //

InformacionTermino::InformacionTermino() {
	ftc = fd = 0;
	this->l_docs.clear();
}

InformacionTermino::InformacionTermino(const int& fft, const int& ffd) {
	ftc = fft;
	fd = ffd;
	this->l_docs.clear();
}

InformacionTermino::InformacionTermino(const InformacionTermino& original) {
	if (this != &original) {
		this->fd = original.fd;
		this->ftc = original.ftc;
		this->l_docs = original.l_docs;
	}
}

InformacionTermino::~InformacionTermino() {
	ftc = fd = 0;
	this->l_docs.clear();
}

InformacionTermino& InformacionTermino::operator =(
		const InformacionTermino& original) {
	this->fd = original.fd;
	this->ftc = original.ftc;
	this->l_docs = original.l_docs;
	return *this;
}

ostream& operator<<(ostream& s, const InformacionTermino& p) {
	s << "Frecuencia total: " << p.ftc << "\tfd: " << p.fd;
	for (unordered_map<long int, InfTermDoc>::const_iterator i =
			p.l_docs.begin(); i != p.l_docs.end(); i++)
		s << "\t" << "Id.Doc: " << i->first << '\t' << i->second;
	s << endl;
	return s;
}
