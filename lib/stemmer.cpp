#include <string>
#include "stemmer.h"
//#include "porter.cc"

using namespace std;

/* ==== :INFO/STRIP.C ==== */

/*
 * AUTHOR	: Stuart J. Barr
 * DATE:	: c. September 1986, give or take a few months...
 */

/*
 * to save confusion...
 * USAGE	: define KEYWORDSIZE (6 is a good value, in practise)
 *		  define FALSE, TRUE
 *		  if you want to strip prefixes, define PREFIXES
 *		  Write a wee C function to call
 *			strip_affixes(string)
 *		  where string is a char *.
 */

#include <stdio.h>
#include <string.h>

/*#include "defs.h"*/
#define TRUE   1
#define FALSE  0
#define KEYWORDSIZE 500
#define PREFIXES 1

#define BIG_KEYWORDSIZE (KEYWORDSIZE+20)

#ifdef PREFIXES
static void strip_prefixes(char *);
#endif
static void to_lower_case(char *);
static void clean(char *);
static void strip_suffixes(char *);
static int isvalid(char);
static void step_1(char *), step_2(char *), step_3(char *), step_4(char *),
		step_5(char *);
static int contains_vowel(char *);
static int measure(char *);
static int vowel(char, char);
static int cvc(char *);
static int has_suffix(char *, char *, char *);

void strip_affixes(char *string)
//	char *string;
		{
	to_lower_case(string);
	clean(string);
#ifdef PREFIXES
	strip_prefixes(string);
#endif 
	strip_suffixes(string);
	string[KEYWORDSIZE] = '\0';
}

static void to_lower_case(char *kwd)
//	char *kwd;
		{
	int i;
	for (i = 0; i < strlen(kwd); i++)
		if ((kwd[i] >= 'A') && (kwd[i] <= 'Z'))
			kwd[i] += 'a' - 'A';
}

static void clean(char *kwd)
//	char *kwd;
		{
	int i, j, last = strlen(kwd);
	for (i = 0; i < last; i++) {
		if (isvalid(kwd[i]) != 0) {
			for (j = i; j < last - 1; j++)
				kwd[j] = kwd[j + 1];
			kwd[last - 1] = '\0';
			last--;
			i--;
		}
	}
}

static int isvalid(char l)
//	char l;
		{
	if ((l >= 'a') && (l <= 'z'))
		return (0);
	if ((l >= 'A') && (l <= 'Z'))
		return (0);
	if ((l >= '0') && (l <= '9'))
		return (0);
	return (1);
}

#ifdef PREFIXES
static void strip_prefixes(char *string)
//	char *string;
		{
	static char *prefixes[] = { (char*) "kilo", (char*) "micro",
			(char*) "milli", (char*) "intra", (char*) "ultra", (char*) "mega",
			(char*) "nano", (char*) "pico", (char*) "pseudo", 0 };
	int i;
	for (i = 0; prefixes[i] != 0; i++) {
		if (strncmp(prefixes[i], string, strlen(prefixes[i])) == 0) {
			int j;
			for (j = 0; j < strlen(string) - strlen(prefixes[i]); j++)
				string[j] = string[j + strlen(prefixes[i])];
			string[j] = '\0';
			return;
		}
	}
}
#endif 

static void strip_suffixes(char *string)
//	char *string;
		{
//	static void step_1(char *), step_2(char *), step_3(char *), step_4(char *),step_5(char *);
	step_1(string);
	step_2(string);
	step_3(string);
	step_4(string);
	step_5(string);
}

static void step_1(char *string)
//	char *string;
		{
	char stem[BIG_KEYWORDSIZE];
	if (string[strlen(string) - 1] == 's') {
		if ((has_suffix(string, (char*) "sses", stem) == TRUE)
				|| (has_suffix(string, (char*) "ies", stem) == TRUE))
			string[strlen(string) - 2] = '\0';
		else if (string[strlen(string) - 2] != 's')
			string[strlen(string) - 1] = '\0';
	}
	if (has_suffix(string, (char*) "eed", stem) == TRUE) {
		if (measure(stem) > 0)
			string[strlen(string) - 1] = '\0';
	} else {
		if (((has_suffix(string, (char*) "ed", stem) == TRUE)
				|| (has_suffix(string, (char*) "ing", stem) == TRUE))
				&& (contains_vowel(stem) == TRUE)) {
			string[strlen(stem)] = '\0';
			if ((has_suffix(string, (char*) "at", stem) == TRUE)
					|| (has_suffix(string, (char*) "bl", stem) == TRUE)
					|| (has_suffix(string, (char*) "iz", stem) == TRUE)) {
				string[strlen(string) + 1] = '\0';
				string[strlen(string)] = 'e';
			} else {
				int length = strlen(string);
				if ((string[length - 1] == string[length - 2])
						&& (string[length - 1] != 'l')
						&& (string[length - 1] != 's')
						&& (string[length - 1] != 'z'))
					string[length - 1] = '\0';
				else if (measure(string) == 1) {
					if (cvc(string) == TRUE) {
						string[strlen(string) + 1] = '\0';
						string[strlen(string)] = 'e';
					}
				}
			}
		}
	}
	if ((has_suffix(string, (char*) "y", stem) == TRUE)
			&& (contains_vowel(stem) == TRUE))
		string[strlen(string) - 1] = 'i';
//	if ( has_suffix(string,"th",stem) == TRUE)		//ING añadido para que quite 50th y deje 50
//		string[strlen(string)-2] = '\0';
}

static void step_2(char *string)
//	char *string;
		{
	static char *suffixes[][2] = { { (char*) "ational", (char*) "ate" }, {
			(char*) "tional", (char*) "tion" },
			{ (char*) "enci", (char*) "ence" },
			{ (char*) "anci", (char*) "ance" },
			{ (char*) "izer", (char*) "ize" },
			{ (char*) "iser", (char*) "ize" },
			{ (char*) "abli", (char*) "able" },
			{ (char*) "alli", (char*) "al" },
			{ (char*) "entli", (char*) "ent" }, { (char*) "eli", (char*) "e" },
			{ (char*) "ousli", (char*) "ous" }, { (char*) "ization",
					(char*) "ize" }, { (char*) "isation", (char*) "ize" }, {
					(char*) "ation", (char*) "ate" }, { (char*) "ator",
					(char*) "ate" }, { (char*) "alism", (char*) "al" }, {
					(char*) "iveness", (char*) "ive" }, { (char*) "fulness",
					(char*) "ful" }, { (char*) "ousness", (char*) "ous" }, {
					(char*) "aliti", (char*) "al" }, { (char*) "iviti",
					(char*) "ive" }, { (char*) "biliti", (char*) "ble" },
			{ 0, 0 } };
	char stem[BIG_KEYWORDSIZE];
	register int index;
	for (index = 0; suffixes[index][0] != 0; index++) {
		if (has_suffix(string, suffixes[index][0], stem) == TRUE) {
			if (measure(stem) > 0) {
				sprintf(string, "%s%s", stem, suffixes[index][1]);
				return;
			}
		}
	}
	return;
}

static void step_3(char *string)
//	char *string;
		{
	static char *suffixes[][2] = { { (char*) "icate", (char*) "ic" }, {
			(char*) "ative", (char*) "" }, { (char*) "alize", (char*) "al" }, {
			(char*) "alise", (char*) "al" }, { (char*) "iciti", (char*) "ic" },
			{ (char*) "ical", (char*) "ic" }, { (char*) "ful", (char*) "" }, {
					(char*) "ness", (char*) "" }, { 0, 0 } };
	char stem[BIG_KEYWORDSIZE];
	register int index;
	for (index = 0; suffixes[index][0] != 0; index++) {
		if (has_suffix(string, suffixes[index][0], stem) == TRUE)
			if (measure(stem) > 0) {
				sprintf(string, "%s%s", stem, suffixes[index][1]);
				return;
			}
	}
	return;
}

static void step_4(char *string)
//	char *string;
		{
	static char *suffixes[] = { (char*) "al", (char*) "ance", (char*) "ence",
			(char*) "er", (char*) "ic", (char*) "able", (char*) "ible",
			(char*) "ant", (char*) "ement", (char*) "ment", (char*) "ent",
			(char*) "sion", (char*) "tion", (char*) "ou", (char*) "ism",
			(char*) "ate", (char*) "iti", (char*) "ous", (char*) "ive",
			(char*) "ize", (char*) "ise", 0 };
	char stem[BIG_KEYWORDSIZE];
	register int index;
	for (index = 0; suffixes[index] != 0; index++) {
		if (has_suffix(string, suffixes[index], stem) == TRUE)
			if (measure(stem) > 1) {
				strcpy(string, stem);
				return;
			}
	}
	return;
}

static void step_5(char *string)
//	char *string;
		{
	if (string[strlen(string) - 1] == 'e') {
		if (measure(string) > 1)
			/* measure(string)==measure(stem) if ends in vowel */
			string[strlen(string) - 1] = '\0';
		else if (measure(string) == 1) {
			char stem[BIG_KEYWORDSIZE];
			strcpy(stem, "");
			strncat(stem, string, strlen(string) - 1);
			if (cvc(stem) == FALSE)
				string[strlen(string) - 1] = '\0';
		}
	}
	if ((string[strlen(string) - 1] == 'l')
			&& (string[strlen(string) - 2] == 'l') && (measure(string) > 1))
		string[strlen(string) - 1] = '\0';
}

static int has_suffix(char *word, char *suffix, char *stem)
//	char *word, *suffix, *stem;
		{
	char tmp[BIG_KEYWORDSIZE];
	if (strlen(word) <= strlen(suffix))
		return (FALSE);
	if ((strlen(suffix) > 1)
			&& (word[strlen(word) - 2] != suffix[strlen(suffix) - 2]))
		return (FALSE);
	strcpy(stem, "");
	strncat(stem, word, strlen(word) - strlen(suffix));
	strcpy(tmp, stem);
	strcat(tmp, suffix);
	if (strcmp(tmp, word) == 0)
		return (TRUE);
	else
		return (FALSE);
}

static int cvc(char *string)
//	char *string;
		{
	int length = strlen(string);
	if (length < 3)
		return (FALSE);
	if ((vowel(string[length - 1], string[length - 2]) == FALSE)
			&& (string[length - 1] != 'w') && (string[length - 1] != 'x')
			&& (string[length - 1] != 'y')
			&& (vowel(string[length - 2], string[length - 3]) == TRUE)
			&& (((length == 3) && (vowel(string[0], 'a') == FALSE))
					|| (vowel(string[length - 3], string[length - 4]) == FALSE)))
		return (TRUE);
	else
		return (FALSE);
}

static int vowel(char ch, char prev)
//	char ch, prev;
		{
	switch (ch) {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
		return (TRUE);
	case 'y':
		return (vowel(prev, '?') == TRUE);
	default:
		return (FALSE);
	}
	return (TRUE);
}

static int measure(char *stem)
//	char *stem;
		{
	register int i = 0, count = 0;
	int length = strlen(stem);
	while (i < length) {
		for (; i < length; i++) {
			if (i > 0) {
				if (vowel(stem[i], stem[i - 1]) == TRUE)
					break;
			} else {
				if (vowel(stem[i], 'a') == TRUE)
					break;
			}
		}
		for (i++; i < length; i++) {
			if (i > 0) {
				if (vowel(stem[i], stem[i - 1]) == FALSE)
					break;
			} else {
				if (vowel(stem[i], '?') == FALSE)
					break;
			}
		}
		if (i < length) {
			count++;
			i++;
		}
	}
	return (count);
}

static int contains_vowel(char *word)
//	char *word;
		{
	register int i;
	for (i = 0; i < strlen(word); i++)
		if (i > 0) {
			if (vowel(word[i], word[i - 1]) == TRUE)
				return (TRUE);
		} else {
			if (vowel(word[0], 'a') == TRUE)
				return (TRUE);
		}
	return (FALSE);
}

/*  Spanish stemmer tring to remove inflectional suffixes */
static char *removeSpanishAccent(char[]);

char * spanish_stemming(char word[]) {
	int len = strlen(word) - 1;

	if (len > 3) {
		removeSpanishAccent(word);
		if ((word[len] == 's') && (word[len - 1] == 'e')
				&& (word[len - 2] == 's') && (word[len - 3] == 'e')) {
			/*  corteses -> cortés  */
			word[len - 1] = '\0';
			return (word);
		}
		if ((word[len] == 's') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'c')) {
			word[len - 2] = 'z'; /*  dos veces -> una vez  */
			word[len - 1] = '\0';
			return (word);
		}
		if (word[len] == 's') { /*  ending with -os, -as  or -es */
			if (word[len - 1] == 'o' || word[len - 1] == 'a'
					|| word[len - 1] == 'e') {
				word[len - 1] = '\0'; /*  remove -os, -as  or -es */
				return (word);
			}
		}
		if (word[len] == 'o') { /*  ending with  -o  */
			word[len] = '\0';
			return (word);
		}
		if (word[len] == 'a') { /*  ending with  -a  */
			word[len] = '\0';
			return (word);
		}
		if (word[len] == 'e') { /*  ending with  -e  */
			word[len] = '\0';
			return (word);
		}
	} /* end if (len > 3) */
	return (word);
}

static char * removeSpanishAccent(char word[]) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if ((word[i] == 'à') || (word[i] == 'á') || (word[i] == 'â')
				|| (word[i] == 'ä')) {
			word[i] = 'a';
		}
		if ((word[i] == 'ò') || (word[i] == 'ó') || (word[i] == 'ô')
				|| (word[i] == 'ö')) {
			word[i] = 'o';
		}
		if ((word[i] == 'è') || (word[i] == 'é') || (word[i] == 'ê')
				|| (word[i] == 'ë')) {
			word[i] = 'e';
		}
		if ((word[i] == 'ù') || (word[i] == 'ú') || (word[i] == 'û')
				|| (word[i] == 'ü')) {
			word[i] = 'u';
		}
		if ((word[i] == 'ì') || (word[i] == 'í') || (word[i] == 'î')
				|| (word[i] == 'ï')) {
			word[i] = 'i';
		}
	}
	return (word);
}
// ***********************************FIN ESPANOL

// ********************************** PORTUGUES

static char *remove_PTsuffix(char *);
static char *normFemininPortuguese(char *);
static char *finalVowelPortuguese(char *);
static char *removeAllPTAccent(char *);

char * portuguese_stemming(char *word) {
	int len = strlen(word) - 1;

	if (len > 2) {
		remove_PTsuffix(word);
		normFemininPortuguese(word);
		finalVowelPortuguese(word);
		removeAllPTAccent(word);
	}
	return (word);
}

static char *
removeAllPTAccent(char *word) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if (word[i] == 'ä') {
			word[i] = 'a';
		}
		if (word[i] == 'â') {
			word[i] = 'a';
		}
		if (word[i] == 'à') {
			word[i] = 'a';
		}
		if (word[i] == 'á') {
			word[i] = 'a';
		}
		if (word[i] == 'ã') {
			word[i] = 'a';
		}
		if (word[i] == 'ê') {
			word[i] = 'e';
		}
		if (word[i] == 'é') {
			word[i] = 'e';
		}
		if (word[i] == 'è') {
			word[i] = 'e';
		}
		if (word[i] == 'ë') {
			word[i] = 'e';
		}
		if (word[i] == 'ï') {
			word[i] = 'i';
		}
		if (word[i] == 'î') {
			word[i] = 'i';
		}
		if (word[i] == 'ì') {
			word[i] = 'i';
		}
		if (word[i] == 'í') {
			word[i] = 'i';
		}
		if (word[i] == 'ü') {
			word[i] = 'u';
		}
		if (word[i] == 'ú') {
			word[i] = 'u';
		}
		if (word[i] == 'ù') {
			word[i] = 'u';
		}
		if (word[i] == 'û') {
			word[i] = 'u';
		}
		if (word[i] == 'ô') {
			word[i] = 'o';
		}
		if (word[i] == 'ö') {
			word[i] = 'o';
		}
		if (word[i] == 'ó') {
			word[i] = 'o';
		}
		if (word[i] == 'ò') {
			word[i] = 'o';
		}
		if (word[i] == 'õ') {
			word[i] = 'o';
		}
		if (word[i] == 'ç') {
			word[i] = 'c';
		}
	}
	return (word);
}

static char * finalVowelPortuguese(char *word) {
	int len = strlen(word) - 1;

	if (len > 3) {
		if ((word[len] == 'e') || (word[len] == 'a') || (word[len] == 'o')) {
			word[len] = '\0'; /* remove final -e or -a or -o */
			return (word);
		}
	}
	return (word);
}

/* Remove plural and feminine form of Portuguese words */

static char * remove_PTsuffix(char *word) {
	int len = strlen(word) - 1;

	if (len > 3) { /* plural in -es when sing form ends with -r, -s, -l or -z*/
		if ((word[len] == 's') && (word[len - 1] == 'e')
				&& ((word[len - 2] == 'r') || (word[len - 2] == 's')
						|| (word[len - 2] == 'z') || (word[len - 2] == 'l'))) {
			word[len - 1] = '\0'; /* doutores (plur) --> doutor (sing) */
			return (word);
		}
	} /* len > 3 */

	if (len > 2) { /* when sing form ends with -em, change -m in -n in plur */
		if ((word[len] == 's') && (word[len - 1] == 'n')) {
			word[len - 1] = 'm'; /* homens (plur) --> homem (sing) */
			word[len] = '\0';
			return (word);
		}
	} /* len > 2 */

	if (len > 3) { /* when sing form ends with -el, change -el in -eis  */
		if (((word[len] == 's') && (word[len - 1] == 'i'))
				&& ((word[len - 2] == 'e') || (word[len - 2] == 'é'))) {
			word[len - 2] = 'e'; /* papéis (plur) --> papel (sing) */
			word[len - 1] = 'l'; /* error:  faceis (plur) --> facil (sing) */
			word[len] = '\0';
			return (word);
		}
	} /* len > 3 */

	if (len > 3) { /* when sing form ends with -ais, change -ais in -al in plur */
		if ((word[len] == 's') && (word[len - 1] == 'i')
				&& (word[len - 2] == 'a')) {
			word[len - 1] = 'l'; /* normais (plur) --> normal (sing) */
			word[len] = '\0';
			return (word);
		}
	} /* len > 3 */

	if (len > 3) { /* when sing form ends with -'ois, change -ais in -al in plur */
		if ((word[len] == 's') && (word[len - 1] == 'i')
				&& (word[len - 2] == 'ó')) {
			word[len - 2] = 'o'; /* lencois (plur) --> lencol (sing) */
			word[len - 1] = 'l';
			word[len] = '\0';
			return (word);
		}
	} /* len > 3 */

	if (len > 3) { /* when sing form ends with -is, change -is in -il in plur */
		if ((word[len] == 's') && (word[len - 1] == 'i')) {
			word[len] = 'l'; /* barris (plur) --> barril (sing) */
			return (word);
		}
	} /* len > 3 */

	if (len > 2) { /* when plur form ends with -ões, change -ões in -ão  */
		if ((word[len] == 's') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'õ')) {
			word[len - 2] = 'ã'; /* botões (plur) --> botão (sing) */
			word[len - 1] = 'o';
			word[len] = '\0';
			return (word);
		} /* when plur form ends with -ães, change -ães in -ão  */
		if ((word[len] == 's') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'ã')) {
			word[len - 1] = 'o'; /* caes (plur) --> cao (sing) */
			word[len] = '\0';
			return (word);
		}
	} /* len > 2 */

	if (len > 5) { /* for adverb -mente */
		if ((word[len] == 'e') && (word[len - 1] == 't')
				&& (word[len - 2] == 'n') && (word[len - 3] == 'e')
				&& (word[len - 4] == 'm')) {
			word[len - 4] = '\0';
			return (word);
		}
	} /* len > 5 */

	if (len > 2) { /* usually plural in -s */
		if (word[len] == 's') {
			word[len] = '\0'; /* of course, invariable word, pires->pires */
			len--;
		}
	} /* len > 2 */

	return (word);
}

static char * normFemininPortuguese(char *word) {
	int len = strlen(word) - 1;

	if ((len < 3) || (word[len] != 'a')) {
		return (word);
	}
	if (len > 6) {
		/*  -inha  -> inho */
		if ((word[len - 1] == 'h') && (word[len - 2] == 'n')
				&& (word[len - 3] == 'i')) {
			word[len] = 'o';
			return (word);
		}
		/*  -iaca  -> iaco */
		if ((word[len - 1] == 'c') && (word[len - 2] == 'a')
				&& (word[len - 3] == 'i')) {
			word[len] = 'o';
			return (word);
		}
		/*  -eira  -> eiro */
		if ((word[len - 1] == 'r') && (word[len - 2] == 'i')
				&& (word[len - 3] == 'e')) {
			word[len] = 'o';
			return (word);
		}
	} /* len > 6 */

	if (len > 5) {
		/*  -ona  -> ão */
		if ((word[len - 1] == 'n') && (word[len - 2] == 'o')) {
			word[len - 2] = 'ã';
			word[len - 1] = 'o';
			word[len] = '\0';
			return (word);
		}
		/*  -ora  -> or */
		if ((word[len - 1] == 'r') && (word[len - 2] == 'o')) {
			word[len] = '\0';
			return (word);
		}
		/*  -osa  -> oso */
		if ((word[len - 1] == 's') && (word[len - 2] == 'o')) {
			word[len] = 'o';
			return (word);
		}
		/*  -esa  -> ês */
		if ((word[len - 1] == 's') && (word[len - 2] == 'e')) {
			word[len - 2] = 'ê';
			word[len] = '\0';
			return (word);
		}
		/*  -ica  -> ico */
		if ((word[len - 1] == 'c') && (word[len - 2] == 'i')) {
			word[len] = 'o';
			return (word);
		}
		/*  -ida  -> ido */
		if ((word[len - 1] == 'd') && (word[len - 2] == 'i')) {
			word[len] = 'o';
			return (word);
		}
		/*  -ada  -> ido */
		if ((word[len - 1] == 'd') && (word[len - 2] == 'a')) {
			word[len] = 'o';
			return (word);
		}
		/*  -iva  -> ivo */
		if ((word[len - 1] == 'v') && (word[len - 2] == 'i')) {
			word[len] = 'o';
			return (word);
		}
		/*  -ama  -> imo */
		if ((word[len - 1] == 'm') && (word[len - 2] == 'a')) {
			word[len] = 'o';
			return (word);
		}
		/*  -na  -> no */
		if (word[len - 1] == 'n') {
			word[len] = 'o';
			return (word);
		}
	} /* len > 5 */

	return (word);
}

// ***********************************FIN PORTUGUES

/* Remove plural and feminine form of French words */

static char * remove_french_plural(char word[]) {
	int len = strlen(word) - 1;

	if (len > 4) {
		if (word[len] == 'x') {
			if (word[len - 1] == 'u' && word[len - 2] == 'a') {
				word[len - 1] = 'l';
			}
			word[len] = '\0';
			return (word);
		} else {
			if (word[len] == 's') {
				word[len] = '\0';
				len--;
			}
			if (word[len] == 'r') {
				word[len] = '\0';
				len--;
			}
			if (word[len] == 'e') {
				word[len] = '\0';
				len--;
			}
			if (word[len] == 'é') {
				word[len] = '\0';
				len--;
			}
			if (word[len] == word[len - 1])
				word[len] = '\0';
		} /* end else */
	} /* end if (len > 4) */
	return (word);
}
//***************Fin frances

// Inicio ALEMAN
//static char *remove_german_plural();
//static char *removeAllAccent ();

static char * removeAllAccent(char word[]) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if (word[i] == 'ä') {
			word[i] = 'a';
		}
		if (word[i] == 'ö') {
			word[i] = 'o';
		}
		if (word[i] == 'ü') {
			word[i] = 'u';
		}
	}
	return (word);
}

static char * remove_german_plural(char word[]) {
	int len = strlen(word) - 1;

	if (len > 3) {
		removeAllAccent(word);
		if (len > 5) {
			if ((word[len] == 'n') && (word[len - 1] == 'e')
					&& (word[len - 2] == 'n')) {
				word[len - 2] = '\0'; /*  ending with -nen  */
				return (word);
			}
		}
		if (len > 4) {
			if ((word[len] == 'n') && (word[len - 1] == 'e')) { /*  ending with -en  */
				word[len - 1] = '\0';
				return (word);
			}
			if ((word[len] == 'e') && (word[len - 1] == 's')) { /*  ending with -se  */
				word[len - 1] = '\0';
				return (word);
			}
			if ((word[len] == 's') && (word[len - 1] == 'e')) { /*  ending with -es  */
				word[len - 1] = '\0';
				return (word);
			}
			if ((word[len] == 'r') && (word[len - 1] == 'e')) { /*  ending with -er  */
				word[len - 1] = '\0';
				return (word);
			}
		} /* end if len > 4 */
		if (word[len] == 'n') { /*  ending with -n  */
			word[len] = '\0';
			return (word);
		}
		if (word[len] == 's') { /*  ending with -s  */
			word[len] = '\0';
			return (word);
		}
		if (word[len] == 'r') { /*  ending with -r  */
			word[len] = '\0';
			return (word);
		}
		if (word[len] == 'e') { /*  ending with -r  */
			word[len] = '\0';
			return (word);
		}
	} /* end if (len > 4) */
	return (word);
}
//*************FIN ALEMAN
/*  Italian stemmer tring to remove inflectional suffixes */

//static char *removeItalianAccent();

static char * removeItalianAccent(char word[]) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if ((word[i] == 'à') || (word[i] == 'á') || (word[i] == 'â')
				|| (word[i] == 'ä')) {
			word[i] = 'a';
		}
		if ((word[i] == 'ò') || (word[i] == 'ó') || (word[i] == 'ô')
				|| (word[i] == 'ö')) {
			word[i] = 'o';
		}
		if ((word[i] == 'è') || (word[i] == 'é') || (word[i] == 'ê')
				|| (word[i] == 'ë')) {
			word[i] = 'e';
		}
		if ((word[i] == 'ù') || (word[i] == 'ú') || (word[i] == 'û')
				|| (word[i] == 'ü')) {
			word[i] = 'u';
		}
		if ((word[i] == 'ì') || (word[i] == 'í') || (word[i] == 'î')
				|| (word[i] == 'ï')) {
			word[i] = 'i';
		}
	}
	return (word);
}

//***********************Inicio funcion principal
static char * italian_stemming(char word[]) {
	int len = strlen(word) - 1;

	if (len > 4) {
		removeItalianAccent(word);
		if (word[len] == 'e') { /*  ending with -ie or -he  */
			if (word[len - 1] == 'i' || word[len - 1] == 'h') {
				word[len - 1] = '\0';
				return (word);
			}
			word[len] = '\0'; /*  ending with -e  */
			return (word);
		}
		if (word[len] == 'i') { /*  ending with -hi or -ii */
			if ((word[len - 1] == 'h') || (word[len - 1] == 'i')) {
				word[len - 1] = '\0';
				return (word);
			}
			word[len] = '\0'; /*  ending with -i  */
			return (word);
		}
		if (word[len] == 'a') { /*  ending with -ia  */
			if (word[len - 1] == 'i') {
				word[len - 1] = '\0';
				return (word);
			}
			word[len] = '\0'; /*  ending with -a  */
			return (word);
		}
		if (word[len] == 'o') { /*  ending with -io  */
			if (word[len - 1] == 'i') {
				word[len - 1] = '\0';
				return (word);
			}
			word[len] = '\0'; /*  ending with -o  */
			return (word);
		}

	} /* end if (len > 4) */
	return (word);
}
/*  French stemmer tring to remove inflectional and derivational suffixes */

static char * removeAllFEAccent(char *word) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if (word[i] == 'â') {
			word[i] = 'a';
		}
		if (word[i] == 'à') {
			word[i] = 'a';
		}
		if (word[i] == 'á') {
			word[i] = 'a';
		}
		if (word[i] == 'ê') {
			word[i] = 'e';
		}
		if (word[i] == 'é') {
			word[i] = 'e';
		}
		if (word[i] == 'è') {
			word[i] = 'e';
		}
		if (word[i] == 'î') {
			word[i] = 'i';
		}
		if (word[i] == 'ù') {
			word[i] = 'u';
		}
		if (word[i] == 'û') {
			word[i] = 'u';
		}
		if (word[i] == 'ô') {
			word[i] = 'o';
		}
		if (word[i] == 'ç') {
			word[i] = 'c';
		}
	}
	return (word);
}
static char * removeDoublet(char *word) {
	int len = strlen(word) - 1;
	int i, position;
	char currentChar;

	if (len > 3) {
		currentChar = word[0];
		position = 1;
		while (word[position]) {
			if (currentChar == word[position]) {
				i = position - 1;
				while (word[i] != '\0') {
					word[i] = word[i + 1];
					i++;
				}
			} /* end if */
			else {
				currentChar = word[position];
				position++;
			}
		} /* end while */
	} /* end if len */
	return (word);
}

static char *
normfrenchword(char * word) {
	int len = strlen(word) - 1;

	if (len > 3) {
		removeAllFEAccent(word);
		removeDoublet(word);
		len = strlen(word) - 1;
	}

	if (len > 3) {
		if (word[len] == 'e' && word[len - 1] == 'i') {
			word[len - 1] = '\0';
			len = len - 2;
		}
	}
	if (len > 3) {
		if (word[len] == 'r') {
			word[len] = '\0';
			len--;
		}
		if (word[len] == 'e') {
			word[len] = '\0';
			len--;
		}
		/*    if (word[len]=='é')  */
		if (word[len] == 'e') {
			word[len] = '\0';
			len--;
		}
		if (word[len] == word[len - 1])
			word[len] = '\0';
	}
	return (word);
}

char * french_stemming(char *word) {
	int len = strlen(word) - 1;

	removeSpanishAccent(word);
	if (len > 4) {
		if (word[len] == 'x') {
			if (word[len - 1] == 'u' && word[len - 2] == 'a'
					&& word[len - 3] != 'e') {
				word[len - 1] = 'l'; /*  chevaux -> cheval  */
			} /*  error :  travaux -> traval but not travail  */
			word[len] = '\0'; /*  anneaux -> anneau,  neveux -> neveu  */
			len--; /*  error :  aulx -> aul but not ail (rare)  */
		}
	} /*  error :  yeux -> yeu but not oeil (rare)  */
	if (len > 2) {
		if (word[len] == 'x') {
			word[len] = '\0'; /*  peaux -> peau,  poux -> pou  */
			len--; /*  error :  affreux -> affreu */
		}
	}

	if (len > 2 && word[len] == 's') { /*  remove final --s --> -- */
		word[len] = '\0';
		len--;
	}

	if (len > 8) { /* --issement  -->   --ir */
		if (word[len] == 't' && word[len - 1] == 'n' && word[len - 2] == 'e'
				&& word[len - 3] == 'm' && word[len - 4] == 'e'
				&& word[len - 5] == 's' && word[len - 6] == 's'
				&& word[len - 7] == 'i') {
			word[len - 6] = 'r'; /* investissement --> investir */
			word[len - 5] = '\0';
			return (normfrenchword(word));
		}
	}

	if (len > 7) { /* ---issant  -->   ---ir */
		if (word[len] == 't' && word[len - 1] == 'n' && word[len - 2] == 'a'
				&& word[len - 3] == 's' && word[len - 4] == 's'
				&& word[len - 5] == 'i') {
			word[len - 4] = 'r'; /* assourdissant --> assourdir */
			word[len - 3] = '\0';
			return (normfrenchword(word));
		}
	}

	if (len > 5) { /* --ement  -->   --e */
		if (word[len] == 't' && word[len - 1] == 'n' && word[len - 2] == 'e'
				&& word[len - 3] == 'm' && word[len - 4] == 'e') {
			word[len - 3] = '\0'; /* pratiquement --> pratique */
			if (word[len - 5] == 'v' && word[len - 6] == 'i') {
				word[len - 5] = 'f'; /* administrativement --> administratif */
				word[len - 4] = '\0';
			}
			return (normfrenchword(word));
		}
	}

	if (len > 10) { /* ---ficatrice  -->   --fier */
		if (word[len] == 'e' && word[len - 1] == 'c' && word[len - 2] == 'i'
				&& word[len - 3] == 'r' && word[len - 4] == 't'
				&& word[len - 5] == 'a' && word[len - 6] == 'c'
				&& word[len - 7] == 'i' && word[len - 8] == 'f') {
			word[len - 6] = 'e';
			word[len - 5] = 'r';
			word[len - 4] = '\0'; /* justificatrice --> justifier */
			return (normfrenchword(word));
		}
	}

	if (len > 9) { /* ---ficateur -->   --fier */
		if (word[len] == 'r' && word[len - 1] == 'u' && word[len - 2] == 'e'
				&& word[len - 3] == 't' && word[len - 4] == 'a'
				&& word[len - 5] == 'c' && word[len - 6] == 'i'
				&& word[len - 7] == 'f') {
			word[len - 5] = 'e';
			word[len - 4] = 'r';
			word[len - 3] = '\0'; /* justificateur --> justifier */
			return (normfrenchword(word));
		}
	}

	if (len > 8) { /* ---catrice  -->   --quer */
		if (word[len] == 'e' && word[len - 1] == 'c' && word[len - 2] == 'i'
				&& word[len - 3] == 'r' && word[len - 4] == 't'
				&& word[len - 5] == 'a' && word[len - 6] == 'c') {
			word[len - 6] = 'q';
			word[len - 5] = 'u';
			word[len - 4] = 'e';
			word[len - 3] = 'r';
			word[len - 2] = '\0'; /* educatrice--> eduquer */
			return (normfrenchword(word));
		}
	}

	if (len > 7) { /* ---cateur -->   --quer */
		if (word[len] == 'r' && word[len - 1] == 'u' && word[len - 2] == 'e'
				&& word[len - 3] == 't' && word[len - 4] == 'a'
				&& word[len - 5] == 'c') {
			word[len - 5] = 'q';
			word[len - 4] = 'u';
			word[len - 3] = 'e';
			word[len - 2] = 'r';
			word[len - 1] = '\0'; /* communicateur--> communiquer */
			return (normfrenchword(word));
		}
	}

	if (len > 7) { /* ---atrice  -->   --er */
		if (word[len] == 'e' && word[len - 1] == 'c' && word[len - 2] == 'i'
				&& word[len - 3] == 'r' && word[len - 4] == 't'
				&& word[len - 5] == 'a') {
			word[len - 5] = 'e';
			word[len - 4] = 'r';
			word[len - 3] = '\0'; /* accompagnatrice--> accompagner */
			return (normfrenchword(word));
		}
	}

	if (len > 6) { /* ---ateur  -->   --er */
		if (word[len] == 'r' && word[len - 1] == 'u' && word[len - 2] == 'e'
				&& word[len - 3] == 't' && word[len - 4] == 'a') {
			word[len - 4] = 'e';
			word[len - 3] = 'r';
			word[len - 2] = '\0'; /* administrateur--> administrer */
			return (normfrenchword(word));
		}
	}

	if (len > 5) { /* --trice  -->   --teur */
		if (word[len] == 'e' && word[len - 1] == 'c' && word[len - 2] == 'i'
				&& word[len - 3] == 'r' && word[len - 4] == 't') {
			word[len - 3] = 'e';
			word[len - 2] = 'u';
			word[len - 1] = 'r'; /* productrice --> producteur */
			word[len] = '\0'; /* matrice --> mateur ? */
			len--;
		}
	}

	if (len > 4) { /* --ième  -->   -- */
		if (word[len] == 'e' && word[len - 1] == 'm' && word[len - 2] == 'è'
				&& word[len - 3] == 'i') {
			word[len - 3] = '\0';
			return (normfrenchword(word));
		}
	}

	if (len > 6) { /* ---teuse  -->   ---ter */
		if (word[len] == 'e' && word[len - 1] == 's' && word[len - 2] == 'u'
				&& word[len - 3] == 'e' && word[len - 4] == 't') {
			word[len - 2] = 'r';
			word[len - 1] = '\0'; /* acheteuse --> acheter */
			return (normfrenchword(word));
		}
	}

	if (len > 5) { /* ---teur  -->   ---ter */
		if (word[len] == 'r' && word[len - 1] == 'u' && word[len - 2] == 'e'
				&& word[len - 3] == 't') {
			word[len - 1] = 'r';
			word[len] = '\0'; /* planteur --> planter */
			return (normfrenchword(word));
		}
	}

	if (len > 4) { /* --euse  -->   --eu- */
		if (word[len] == 'e' && word[len - 1] == 's' && word[len - 2] == 'u'
				&& word[len - 3] == 'e') {
			word[len - 1] = '\0'; /* poreuse --> poreu-,  plieuse --> plieu- */
			return (normfrenchword(word));
		}
	}

	if (len > 7) { /* ------ère  -->   ------er */
		if (word[len] == 'e' && word[len - 1] == 'r' && word[len - 2] == 'è') {
			word[len - 2] = 'e';
			word[len - 1] = 'r';
			word[len] = '\0'; /* bijoutière --> bijoutier,  caissière -> caissier */
			return (normfrenchword(word));
		}
	}

	if (len > 6) { /* -----ive  -->   -----if */
		if (word[len] == 'e' && word[len - 1] == 'v' && word[len - 2] == 'i') {
			word[len - 1] = 'f'; /* but not convive */
			word[len] = '\0'; /* abrasive --> abrasif */
			return (normfrenchword(word));
		}
	}

	if (len > 3) { /* folle or molle  -->   fou or mou */
		if (word[len] == 'e' && word[len - 1] == 'l' && word[len - 2] == 'l'
				&& word[len - 3] == 'o'
				&& (word[len - 4] == 'f' || word[len - 4] == 'm')) {
			word[len - 2] = 'u';
			word[len - 1] = '\0'; /* folle --> fou */
			return (normfrenchword(word));
		}
	}

	if (len > 8) { /* ----nnelle  -->   ----n */
		if (word[len] == 'e' && word[len - 1] == 'l' && word[len - 2] == 'l'
				&& word[len - 3] == 'e' && word[len - 4] == 'n'
				&& word[len - 5] == 'n') {
			word[len - 4] = '\0'; /* personnelle --> person */
			return (normfrenchword(word));
		}
	}

	if (len > 8) { /* ----nnel  -->   ----n */
		if (word[len] == 'l' && word[len - 1] == 'e' && word[len - 2] == 'n'
				&& word[len - 3] == 'n') {
			word[len - 2] = '\0'; /* personnel --> person */
			return (normfrenchword(word));
		}
	}

	if (len > 3) { /* --ète  -->  et */
		if (word[len] == 'e' && word[len - 1] == 't' && word[len - 2] == 'è') {
			word[len - 2] = 'e';
			word[len] = '\0'; /* complète --> complet */
			len--;
		}
	}

	if (len > 7) { /* -----ique  -->   */
		if (word[len] == 'e' && word[len - 1] == 'u' && word[len - 2] == 'q'
				&& word[len - 3] == 'i') {
			word[len - 3] = '\0'; /* aromatique --> aromat */
			len = len - 4;
		}
	}

	if (len > 7) { /* -----esse -->    */
		if (word[len] == 'e' && word[len - 1] == 's' && word[len - 2] == 's'
				&& word[len - 3] == 'e') {
			word[len - 2] = '\0'; /* faiblesse --> faible */
			return (normfrenchword(word));
		}
	}

	if (len > 6) { /* ---inage -->    */
		if (word[len] == 'e' && word[len - 1] == 'g' && word[len - 2] == 'a'
				&& word[len - 3] == 'n' && word[len - 4] == 'i') {
			word[len - 2] = '\0'; /* patinage --> patin */
			return (normfrenchword(word));
		}
	}

	if (len > 8) { /* ---isation -->   - */
		if (word[len] == 'n' && word[len - 1] == 'o' && word[len - 2] == 'i'
				&& word[len - 3] == 't' && word[len - 4] == 'a'
				&& word[len - 5] == 's' && word[len - 6] == 'i') {
			word[len - 6] = '\0'; /* sonorisation --> sonor */
			if (len > 11 && word[len - 7] == 'l' && word[len - 8] == 'a'
					&& word[len - 9] == 'u')
				word[len - 8] = 'e'; /* ritualisation --> rituel */
			return (normfrenchword(word));
		}
	}

	if (len > 8) { /* ---isateur -->   - */
		if (word[len] == 'r' && word[len - 1] == 'u' && word[len - 2] == 'e'
				&& word[len - 3] == 't' && word[len - 4] == 'a'
				&& word[len - 5] == 's' && word[len - 6] == 'i') {
			word[len - 6] = '\0'; /* colonisateur --> colon */
			return (normfrenchword(word));
		}
	}

	if (len > 7) { /* ----ation -->   - */
		if (word[len] == 'n' && word[len - 1] == 'o' && word[len - 2] == 'i'
				&& word[len - 3] == 't' && word[len - 4] == 'a') {
			word[len - 4] = '\0'; /* nomination --> nomin */
			return (normfrenchword(word));
		}
	}

	if (len > 7) { /* ----ition -->   - */
		if (word[len] == 'n' && word[len - 1] == 'o' && word[len - 2] == 'i'
				&& word[len - 3] == 't' && word[len - 4] == 'i') {
			word[len - 4] = '\0'; /* disposition --> dispos */
			return (normfrenchword(word));
		}
	}

	/* various other suffix */
	return (normfrenchword(word));
}

/*  German stemmer tring to remove inflectional suffixes */

static int STEnding(char aLetter) {
	if (aLetter == 'b' || aLetter == 'd' || aLetter == 'f' || aLetter == 'g'
			|| aLetter == 'h' || aLetter == 'k' || aLetter == 'l'
			|| aLetter == 'm' || aLetter == 'n' || aLetter == 't')
		return (1);
	return (0);
}
static char * removeGermanAccent(char *word) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if ((word[i] == 'ä') || (word[i] == 'à') || (word[i] == 'á')
				|| (word[i] == 'â')) {
			word[i] = 'a';
		}
		if ((word[i] == 'ö') || (word[i] == 'ò') || (word[i] == 'ó')
				|| (word[i] == 'ô')) {
			word[i] = 'o';
		}
		if ((word[i] == 'ï') || (word[i] == 'ì') || (word[i] == 'í')
				|| (word[i] == 'î')) {
			word[i] = 'i';
		}
		if ((word[i] == 'ü') || (word[i] == 'ù') || (word[i] == 'ú')
				|| (word[i] == 'û')) {
			word[i] = 'u';
		}
	}
	return (word);
}

static char *remove_Step1(char *word) {
	int len = strlen(word) - 1;

	if (len > 4) {
		if (word[len] == 'n' && word[len - 1] == 'r' && word[len - 2] == 'e') {
			word[len - 2] = '\0'; /*  ending with -ern ->   */
			return (word);
		}
	}
	if (len > 3) {
		if (word[len] == 'm' && word[len - 1] == 'e') {
			word[len - 1] = '\0'; /*  ending with -em ->  */
			return (word);
		}
		if (word[len] == 'n' && word[len - 1] == 'e') {
			word[len - 1] = '\0'; /*  ending with -en ->  */
			return (word);
		}
		if (word[len] == 'r' && word[len - 1] == 'e') {
			word[len - 1] = '\0'; /*  ending with -er ->  */
			return (word);
		}
		if (word[len] == 's' && word[len - 1] == 'e') {
			word[len - 1] = '\0'; /*  ending with -es ->  */
			return (word);
		}
	}
	if (len > 2) {
		if (word[len] == 'e') {
			word[len] = '\0'; /*  ending with -e ->  */
			return (word);
		}
		if (word[len] == 's' && STEnding(word[len - 1])) {
			word[len] = '\0'; /*  ending with -s ->  */
			return (word);
		}
	}
	return (word);
}

static char *remove_Step2(char *word) {
	int len = strlen(word) - 1;

	if (len > 4) {
		if (word[len] == 't' && word[len - 1] == 's' && word[len - 2] == 'e') {
			word[len - 2] = '\0'; /*  ending with -est ->   */
			return (word);
		}
	}
	if (len > 3) {
		if (word[len] == 'r' && word[len - 1] == 'e') {
			word[len - 1] = '\0'; /*  ending with -er ->  */
			return (word);
		}
		if (word[len] == 'n' && word[len - 1] == 'e') {
			word[len - 1] = '\0'; /*  ending with -en ->  */
			return (word);
		}
		if (word[len] == 't' && word[len - 1] == 's'
				&& STEnding(word[len - 2])) {
			word[len - 1] = '\0'; /*  ending with -st ->  */
			return (word);
		}
	}
	return (word);
}

char * german_stemming(char *word) {
	removeGermanAccent(word);
	remove_Step1(word);
	remove_Step2(word);
	return (word);
}

/*  
 int main()

 {
 string cad;
 int numero;
 cout <<"\n---> in word:  ";
 cin >> cad;
 //cin >> numero;
 stemmer(cad,1);
 cout << "\t\t\t"<<cad<<"\n";

 return 0;
 }

 */

/*  Finnish stemmer tring to remove inflectional suffixes */

#define IsVowel(c) ('a'==(c)||'e'==(c)||'i'==(c)||'o'==(c)||'u'==(c)||\
                    'y'==(c))

static char * removeDoubleKPT(char *word) {
	int len = strlen(word) - 1;
	int i, position;
	char currentChar;

	if (len > 3) {
		currentChar = word[0];
		position = 1;
		while (word[position]) { /*  remove double kk pp tt  */
			if ((currentChar == word[position])
					&& ((currentChar == 'k') || (currentChar == 'p')
							|| (currentChar == 't'))) {
				i = position - 1;
				while (word[i] != '\0') {
					word[i] = word[i + 1];
					i++;
				}
			} /* end if */
			else {
				currentChar = word[position];
				position++;
			}
		} /* end while */
	} /* end if len */
	return (word);
}

static char * removeFinnishAccent(char *word) {
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if (word[i] == 'ä') {
			word[i] = 'a';
		}
		if (word[i] == 'å') {
			word[i] = 'a';
		}
		if (word[i] == 'ö') {
			word[i] = 'o';
		}
	}
	return (word);
}

static char * norm_finnish(char *word) {
	int len = strlen(word) - 1;

	if (len > 4) { /* -hde  -> -ksi  */
		if ((word[len] == 'e') && (word[len - 1] == 'd')
				&& (word[len - 2] == 'h')) {
			word[len - 2] = 'k';
			word[len - 1] = 's';
			word[len] = 'i';
		}
	} /* end if len > 4 */
	if (len > 3) { /* -ei  -> -  */
		if ((word[len] == 'i') && (word[len - 1] == 'e')) {
			word[len - 1] = '\0';
			return (word);
		}
		if ((word[len] == 't') && (word[len - 1] == 'a')) {
			word[len - 1] = '\0';
			return (word);
		}
	} /* end if len > 3 */
	if (len > 2) { /* plural    -t  or -(aeiouy)i */
		if ((word[len] == 't') || (word[len] == 's') || (word[len] == 'j')
				|| (word[len] == 'e') || (word[len] == 'a')) {
			word[len] = '\0';
		} else {
			/*      if ((word[len]=='i') && (IsVowel(word[len-1]))) { */
			if ((word[len] == 'i')) {
				word[len] = '\0';
			}
		}
	} /* end if (len > 2) */
	return (word);
}

static char * norm2_finnish(char *word) {
	int len = strlen(word) - 1;

	if (len > 7) { /* -e, -o,  -u */
		if ((word[len] == 'e') || (word[len] == 'o') || (word[len] == 'u')) {
			word[len] = '\0';
			len--;
		}
	}
	if (len > 3) { /* plural    -i  */
		if (word[len] == 'i') {
			word[len] = '\0';
		}
		removeDoubleKPT(word);
	} /* end if (len > 3) */
	return (word);
}

static char * finnishStep1(char *word) {
	int len = strlen(word) - 1;

	if (len > 7) {
		/*    -kin  */
		if ((word[len] == 'n') && (word[len - 1] == 'i')
				&& (word[len - 2] == 'k')) {
			word[len - 2] = '\0';
			return (finnishStep1(word));
		}
		/*    -ko  */
		if ((word[len] == 'o') && (word[len - 1] == 'k')) {
			word[len - 1] = '\0';
			return (finnishStep1(word));
		}
	} /* end if (len > 7) */
	if (len > 10) {
		/*    -dellinen  for adjective  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'n') && (word[len - 3] == 'i')
				&& (word[len - 4] == 'l') && (word[len - 5] == 'l')
				&& (word[len - 6] == 'e') && (word[len - 7] == 'd')) {
			word[len - 7] = '\0';
			return (word);
		}
		/*    -dellisuus  for adverb  */
		if ((word[len] == 's') && (word[len - 1] == 'u')
				&& (word[len - 2] == 'u') && (word[len - 3] == 's')
				&& (word[len - 4] == 'i') && (word[len - 5] == 'l')
				&& (word[len - 6] == 'l') && (word[len - 7] == 'e')
				&& (word[len - 8] == 'd')) {
			word[len - 8] = '\0';
			return (word);
		}
	} /* end if (len > 10) */
	return (word);
}

static char *finnishStep2(char *word) {
	int len = strlen(word) - 1;

	if (len > 4) {
		/*    -lla  */
		if ((word[len] == 'a') && (word[len - 1] == 'l')
				&& (word[len - 2] == 'l')) {
			word[len - 2] = '\0';
			return (word);
		}
		/*    -tse  */
		if ((word[len] == 'e') && (word[len - 1] == 's')
				&& (word[len - 2] == 't')) {
			word[len - 2] = '\0';
			return (word);
		}
		/*    -sti  */
		if ((word[len] == 'i') && (word[len - 1] == 't')
				&& (word[len - 2] == 's')) {
			word[len - 2] = '\0';
			return (word);
		}
		/*    -ni  */
		if ((word[len] == 'i') && (word[len - 1] == 'n')) {
			word[len - 1] = '\0';
			return (word);
		}
		/*    -a  if -aa  */
		if ((word[len] == 'a') && (word[len - 1] == 'a')) {
			word[len] = '\0';
			return (word);
		}
	} /* end if (len > 4) */
	return (word);
}

static char * finnishStep3(char *word) {
	int len = strlen(word) - 1;

	if (len > 7) {
		/* genetive -nnen  -s  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'n') && (word[len - 3] == 'n')) {
			word[len - 3] = 's';
			word[len - 2] = '\0';
			return (word);
		}
		/* essive -ntena  -s  */
		if ((word[len] == 'a') && (word[len - 1] == 'n')
				&& (word[len - 2] == 'e') && (word[len - 3] == 't')
				&& (word[len - 4] == 'n')) {
			word[len - 4] = 's';
			word[len - 3] = '\0';
			return (word);
		}
		/*  -tten  -s  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 't') && (word[len - 3] == 't')) {
			word[len - 3] = '\0';
			return (word);
		}
		/* genitive plural   -eiden  -s  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'd') && (word[len - 3] == 'i')
				&& (word[len - 4] == 'e')) {
			word[len - 4] = '\0';
			return (word);
		}
	}
	if (len > 5) {
		/* komitatiivi plural   -neen  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'e') && (word[len - 3] == 'n')) {
			word[len - 3] = '\0';
			return (word);
		}
		/* illatiivi   -siin,  etc.  */
		if ((word[len] == 'n') && (word[len - 1] == 'i')
				&& (word[len - 2] == 'i') && (word[len - 3] == 'n')) {
			word[len - 3] = '\0';
			return (word);
		}
		/* illatiivi   -seen,  etc.  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'e') && (word[len - 3] == 's')) {
			word[len - 3] = '\0';
			return (word);
		}
		/* illatiivi   -h?n,  etc.  */
		if ((word[len] == 'n') && (IsVowel(word[len-1]))
				&& (word[len - 2] == 'h')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* genitive plural   -teen,  etc.  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'e') && (word[len - 3] == 't')) {
			word[len - 3] = '\0';
			return (word);
		}
		/* genitive plural   -den  -s  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'd')) {
			word[len - 2] = 's';
			word[len - 1] = '\0';
			return (word);
		}
		/* genitive plural   -ksen  -s  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 's') && (word[len - 3] == 'k')) {
			word[len - 3] = 's';
			word[len - 2] = '\0';
			return (word);
		}
		/* komitatiivi plural   -neni  */
		if ((word[len] == 'n') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'n') && (word[len - 3] == 'i')) {
			word[len - 3] = '\0';
			return (word);
		}
		/* inessiivi   -ssa  */
		if ((word[len] == 'a') && (word[len - 1] == 's')
				&& (word[len - 2] == 's')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* elatiivi   -sta  */
		if ((word[len] == 'a') && (word[len - 1] == 't')
				&& (word[len - 2] == 's')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* adessiivi   -lla  */
		if ((word[len] == 'a') && (word[len - 1] == 'l')
				&& (word[len - 2] == 'l')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* ablatiivi   -lta  */
		if ((word[len] == 'a') && (word[len - 1] == 't')
				&& (word[len - 2] == 'l')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* abessiivi   -tta  */
		if ((word[len] == 'a') && (word[len - 1] == 't')
				&& (word[len - 2] == 't')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* translatiivi   -ksi  */
		if ((word[len] == 'i') && (word[len - 1] == 's')
				&& (word[len - 2] == 'k')) {
			word[len - 2] = '\0';
			return (word);
		}
		/* allatiivi   -lle  */
		if ((word[len] == 'e') && (word[len - 1] == 'l')
				&& (word[len - 2] == 'l')) {
			word[len - 2] = '\0';
			return (word);
		}
	} /* end if (len > 5) */
	if (len > 4) {
		/* essiivi   -na  */
		if ((word[len] == 'a') && (word[len - 1] == 'n')) {
			word[len - 1] = '\0';
			return (word);
		}
		/* komitatiivi   -ne-  */
		if ((word[len] == 'e') && (word[len - 1] == 'n')) {
			word[len - 1] = '\0';
			return (word);
		}
		if ((word[len] == 'i') && (word[len - 1] == 'e')
				&& (word[len - 2] == 'n')) {
			word[len - 2] = '\0';
			return (word);
		}
	} /* end if (len > 4) */
	if (len > 3) {
		/* partitiivi   -(t,j)a  */
		if ((word[len] == 'a')
				&& ((word[len - 1] == 't') || (word[len - 1] == 'j'))) {
			word[len - 1] = '\0';
			return (word);
		}
		if (word[len] == 'a') {
			word[len] = '\0';
			return (word);
		}
		/* illatiivi   -an, -en, -on, -in, -un, -yn, etc.  */
		if ((word[len] == 'n')
				&& ((word[len - 1] == 'a') || (word[len - 1] == 'e')
						|| (word[len - 1] == 'o') || (word[len - 1] == 'i')
						|| (word[len - 1] == 'u') || (word[len - 1] == 'y'))) {
			word[len - 1] = '\0';
			return (word);
		}
		/* genetiivi or instruktiivi   -n  */
		if (word[len] == 'n') {
			word[len] = '\0';
			return (word);
		}
	} /* end if (len > 3) */
	return (word);
}

char * finnish_stemming(char *word) {
	int len = strlen(word) - 1;

	if (len > 2) {
		removeFinnishAccent(word);
		finnishStep1(word);
		finnishStep2(word);
		finnishStep3(word);
		norm_finnish(word);
		norm2_finnish(word);
	}
	return (word);
}
/*  Russian stemmer tring to remove inflectional suffixes from nouns and adjectives */

static char *removeAllRUAccent(char *word) /* remove accents */
{
	int len = strlen(word) - 1;
	int i;

	for (i = len; i >= 0; i--) {
		if ((word[i] == 'à') || (word[i] == 'á') || (word[i] == 'â')
				|| (word[i] == 'ä')) {
			word[i] = 'a';
		}
		if ((word[i] == 'ò') || (word[i] == 'ó') || (word[i] == 'ô')
				|| (word[i] == 'ö')) {
			word[i] = 'o';
		}
		if ((word[i] == 'è') || (word[i] == 'é') || (word[i] == 'ê')) {
			word[i] = 'e';
		}
		if ((word[i] == 'ù') || (word[i] == 'ú') || (word[i] == 'û')
				|| (word[i] == 'ü')) {
			word[i] = 'u';
		}
		if ((word[i] == 'ì') || (word[i] == 'î') || (word[i] == 'ï')) {
			word[i] = 'i';
		}
	}
	return (word);
}

static char *normRussian(char *word) {
	int len = strlen(word) - 1;

	if (len > 1) { /* -'  */
		if ((word[len] == 'n') && (word[len - 1] == 'n')) {
			word[len] = '\0';
			len--;
		}
		if (word[len] == '\'') {
			word[len] = '\0';
			len--;
		}
	} /* end if len > 1 */
	return (word);
}

/* Our russian stemmer */

char *russian_stemming(char *word) {
	int len = strlen(word) - 1;

	removeAllRUAccent(word);
	if (len > 7) {
		/* -iiakh  */
		if ((word[len - 4] == 'i') && (word[len - 3] == 'i')
				&& (word[len - 2] == 'a') && (word[len - 1] == 'k')
				&& (word[len] == 'h')) {
			word[len - 4] = '\0';
			return (normRussian(word));
		}
		/* -iiami  */
		if ((word[len - 4] == 'i') && (word[len - 3] == 'i')
				&& (word[len - 2] == 'a') && (word[len - 1] == 'm')
				&& (word[len] == 'i')) {
			word[len - 4] = '\0';
			return (normRussian(word));
		}
	} /* end if len > 7 */

	if (len > 6) {
		/* -iaia  */
		if ((word[len - 3] == 'i') && (word[len - 2] == 'a')
				&& (word[len - 1] == 'i') && (word[len] == 'a')) {
			word[len - 3] = '\0';
			return (normRussian(word));
		}
		/* -iakh  */
		if ((word[len - 3] == 'i') && (word[len - 2] == 'a')
				&& (word[len - 1] == 'k') && (word[len] == 'h')) {
			word[len - 3] = '\0';
			return (normRussian(word));
		}
		/* -iami  */
		if ((word[len - 3] == 'i') && (word[len - 2] == 'a')
				&& (word[len - 1] == 'm') && (word[len] == 'i')) {
			word[len - 3] = '\0';
			return (normRussian(word));
		}
		/* -iiam  */
		if ((word[len - 3] == 'i') && (word[len - 2] == 'i')
				&& (word[len - 1] == 'a') && (word[len] == 'm')) {
			word[len - 3] = '\0';
			return (normRussian(word));
		}
		/* -iuiu  */
		if ((word[len - 3] == 'i') && (word[len - 2] == 'u')
				&& (word[len - 1] == 'i') && (word[len] == 'u')) {
			word[len - 3] = '\0';
			return (normRussian(word));
		}
	} /* end if len > 6 */

	if (len > 5) {
		/* -ami  */
		if ((word[len - 2] == 'a') && (word[len - 1] == 'm')
				&& (word[len] == 'i')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -aia  */
		if ((word[len - 2] == 'a') && (word[len - 1] == 'i')
				&& (word[len] == 'a')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -akh  */
		if ((word[len - 2] == 'a') && (word[len - 1] == 'k')
				&& (word[len] == 'h')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ego  */
		if ((word[len - 2] == 'e') && (word[len - 1] == 'g')
				&& (word[len] == 'o')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -'iu  */
		if ((word[len - 2] == '\'') && (word[len - 1] == 'i')
				&& (word[len] == 'u')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -'eh  */
		if ((word[len - 2] == '\'') && (word[len - 1] == 'e')
				&& (word[len] == 'h')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -eiu  */
		if ((word[len - 2] == 'e') && (word[len - 1] == 'i')
				&& (word[len] == 'u')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -uiu  */
		if ((word[len - 2] == 'u') && (word[len - 1] == 'i')
				&& (word[len] == 'u')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ikh  */
		if ((word[len - 2] == 'i') && (word[len - 1] == 'k')
				&& (word[len] == 'h')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -imi  */
		if ((word[len - 2] == 'i') && (word[len - 1] == 'm')
				&& (word[len] == 'i')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -emu  */
		if ((word[len - 2] == 'e') && (word[len - 1] == 'm')
				&& (word[len] == 'u')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -'ia  */
		if ((word[len - 2] == '\'') && (word[len - 1] == 'i')
				&& (word[len] == 'a')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -iam */
		if ((word[len - 2] == 'i') && (word[len - 1] == 'a')
				&& (word[len] == 'm')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ie{ìm} */
		if ((word[len - 2] == 'i') && (word[len - 1] == 'e')
				&& ((word[len] == 'ì') || (word[len] == 'm'))) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ii{au} */
		if ((word[len - 2] == 'i') && (word[len - 1] == 'i')
				&& ((word[len] == 'a') || (word[len] == 'u'))) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -'i{ou} */
		if ((word[len - 2] == '\'') && (word[len - 1] == 'i')
				&& ((word[len] == 'o') || (word[len] == 'u'))) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -oiu */
		if ((word[len - 2] == 'o') && (word[len - 1] == 'i')
				&& (word[len] == 'u')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ogo */
		if ((word[len - 2] == 'o') && (word[len - 1] == 'g')
				&& (word[len] == 'o')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ymi */
		if ((word[len - 2] == 'y') && (word[len - 1] == 'm')
				&& (word[len] == 'i')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -omu */
		if ((word[len - 2] == 'o') && (word[len - 1] == 'm')
				&& (word[len] == 'u')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
		/* -ykh */
		if ((word[len - 2] == 'y') && (word[len - 1] == 'k')
				&& (word[len] == 'h')) {
			word[len - 2] = '\0';
			return (normRussian(word));
		}
	} /* end if len > 5 */

	if (len > 4) {
		/* -'e  */
		if ((word[len - 1] == '\'') && (word[len] == 'e')) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -go  */
		if ((word[len - 1] == 'g') && (word[len] == 'o')) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -am  */
		if ((word[len - 1] == 'a') && (word[len] == 'm')) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -mu  */
		if ((word[len - 1] == 'm') && (word[len] == 'u')) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -o{vmeì}  */
		if ((word[len - 1] == 'o')
				&& ((word[len] == 'v') || (word[len] == 'm')
						|| (word[len] == 'e') || (word[len] == 'ì'))) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -e{eiìmv}  */
		if ((word[len - 1] == 'e')
				&& ((word[len] == 'e') || (word[len] == 'ì')
						|| (word[len] == 'i') || (word[len] == 'm')
						|| (word[len] == 'v'))) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -y{eìm}  */
		if ((word[len - 1] == 'y')
				&& ((word[len] == 'e') || (word[len] == 'ì')
						|| (word[len] == 'm'))) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
		/* -i{aeiìmu}  */
		if ((word[len - 1] == 'i')
				&& ((word[len] == 'a') || (word[len] == 'e')
						|| (word[len] == 'i') || (word[len] == 'ì')
						|| (word[len] == 'm') || (word[len] == 'u'))) {
			word[len - 1] = '\0';
			return (normRussian(word));
		}
	} /* end if len > 4 */

	if (len > 3) { /* -{ae'iìmouy}  */
		if ((word[len] == 'a') || (word[len] == 'e') || (word[len] == '\'')
				|| (word[len] == 'i') || (word[len] == 'ì')
				|| (word[len] == 'm') || (word[len] == 'o')
				|| (word[len] == 'u') || (word[len] == 'y')) {
			word[len] = '\0';
			return (normRussian(word));
		}
	} /* end if len > 3 */

	return (normRussian(word));

}

void stemmer(string & cad, int numero) {
	char cadena[1000];

	strcpy(cadena, cad.c_str());

	switch (numero) {
	case 1: //espanol
		cad = spanish_stemming(cadena);
		break;
	case 2: //ingleis
		strip_affixes(cadena);
//           cout << "STEMMER " << numero << ": pal: " << cad << " stem: " << cadena << endl;
		cad = cadena;
		break;
		/*  case 3: //frances
		 //cad = remove_french_plural(cadena);
		 cad = french_stemming(cadena);
		 break;
		 case 4: //aleman
		 //cad = remove_german_plural(cadena);
		 cad = german_stemming(cadena);
		 break;
		 case 5: //italiano
		 cad = italian_stemming(cadena);
		 break;
		 case 6: //fines
		 cad = finnish_stemming(cadena);
		 break;
		 case 7: //rusos
		 cad = russian_stemming(cadena);
		 break;
		 case 8: //portugues
		 cad = portuguese_stemming(cadena);
		 break;*/
	default:
		break;
	}
}

void stemmer(string & cad, int numero, string & cadF) {
	char cadena[1000];

	strcpy(cadena, cad.c_str());

	switch (numero) {
	case 1: //espanol
		cadF = spanish_stemming(cadena);
		break;
	case 2: //ingleis
		strip_affixes(cadena);
//           cout << "STEMMER " << numero << ": pal: " << cad << " stem: " << cadena << endl;
		cadF = cadena;
		break;
		/*  case 3: //frances
		 //cad = remove_french_plural(cadena);
		 cad = french_stemming(cadena);
		 break;
		 case 4: //aleman
		 //cad = remove_german_plural(cadena);
		 cad = german_stemming(cadena);
		 break;
		 case 5: //italiano
		 cad = italian_stemming(cadena);
		 break;
		 case 6: //fines
		 cad = finnish_stemming(cadena);
		 break;
		 case 7: //rusos
		 cad = russian_stemming(cadena);
		 break;
		 case 8: //portugues
		 cad = portuguese_stemming(cadena);
		 break;*/
	default:
		break;
	}
}

