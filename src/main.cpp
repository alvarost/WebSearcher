#include <iostream> 
#include <string>
#include <list> 
#include "buscador.h"

using namespace std;

int
main(void)
{
	IndexadorHash b("./StopWordsIngles.txt", ". ,:", "./indicePrueba", 0, false);

	b.IndexarDirectorio("./CorpusTime/Documentos");
	b.GuardarIndexacion();

	Buscador a("./indicePrueba", 1);
	string preg;

	if(a.Buscar("./CorpusTime/Preguntas",15,1,3))
		a.ImprimirResultadoBusqueda(15);

}
