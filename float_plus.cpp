#include "float_plus.h"
#include<math.h>
#include <iomanip>
#include<iostream>
using namespace std;

//---------------------------------------------------------------------------------//
//																				   //
//  Librairie float_plus par GUIRONNET Yannis, tutor� par GROSSO Vincent		   //
// 																				   //
//  Un float_plus est un flottant cod� sur 32 bits, situ� entre 0 et 1			   //
//  cod� sur le m�me principe que la norme IEEE754, et in�vitablement plus pr�cis  //
//  car un float habituel se situe entre des bornes bien plus larges			   //
//  Ce float_plus poss�de 8 bits d'exposant, pour un exposant entre 0 et -255	   //
//  et 24 bits de mantisse.														   //
// 																				   //
//  Valeur minimum d'un float_plus  : 0											   //
//  Valeur maximum d'un float_plus  : 1											   //
//  Plus petite valeur d�normalis�e : 1.727233814e-77							   //
//  Plus grande valeur d�normalis�e : 0.9999999702								   //
//																				   //
//---------------------------------------------------------------------------------//


// Cette fonction permet d'afficher grossi�rement la valeur de notre float+.
// On stock cette valeur dans un double, qui est plus pr�cis que notre float+.
double Float_plus::get_double(void)
{
	double valeur;
	// Le double �tant plus grand que le float+, la valeur ne sera pas erron�e
	if (bits == 0)
	{
		valeur = 0;
	}
	else if (bits >= 0xFF000000) {
		valeur = 1;
	}
	else {
		valeur = 1;
		for (unsigned int i = 0; i < 24; i++)
		{
			if (bits&(1 << i))
			{
				valeur += (2 / pow(2, 25 - i));
			}
			// La fonction est simple, on ajoute 2^-(position) chaque fois qu'on trouve un '1'
			// � une certaine position dans la mantisse
		}
		int exp = ((bits >> 24) - 255);
		valeur *= (1 / pow(2, (-exp)));
	}
	return valeur;
}


//On utilise la fonction get_double pour afficher la valeur
void Float_plus::Affiche_float(void)
{
	cout << "La valeur approximative du flottant est : " << setprecision(25) << get_double() << endl;
}

// Fonction qui permet d'afficher la mantisse et l'exposant du double_plus, au cas ou la valeur
// est hors des limites d'un double normal.
// Affiche sous la forme : Valeur du double plus : 1.mantisse * 2^(exposant)
void Float_plus::Affiche_mantisse(void)
{
	streamsize prec = cout.precision();
	double d = 1;
	uint32_t temp = 1;
	uint32_t exp = (255 - (bits >> 24)) & 0xFF;
	if (bits != 0)
	{
		for (unsigned int i = 0; i < 24; i++)
		{
			if (bits&(temp))
			{
				d += (2 / pow(2, 25 - i));

			}
			temp *= 2;
		}

		cout << "Valeur du float_plus : " << setprecision(25) << d  << " * 2^(-" << exp << ")" <<setprecision(prec) << endl;
	}
	else cout << "Valeur du float_plus : 0" << endl;
}


// Fonction simple qui affiche les bits du float+, sous forme d'un uint32_t;
void Float_plus::Affiche_binaire(void)
{
	cout << bits << endl;
}


// Fonction simple qui affiche le float associ� � la pr�cision;
void Float_plus::Affiche_precision(void)
{
	cout << "La precision du float est de plus ou moins " << precision << "." << endl;
}

// Constructeur du float+ de base. Sa valaur sera de 0 par d�faut
Float_plus::Float_plus()
{

}


// Constructeur � partir d'un uint32_t. Permet d'initialiser facilement un float+ de notre choix, 
// lorsqu'il se situe hors des limites d'un float normal.
Float_plus::Float_plus(unsigned int b)
{
	bits = b;
	int exp = ((bits >> 24) - 255);
	precision = pow(2, -24) * pow(2, exp);
}

// Constructeur � partir d'un float. Permet de choisir facilement la valeur de notre float+
// lorsqu'on veut une valeur pas trop grande ou petite. 
Float_plus::Float_plus(float f)
{
	// Si le float est situ� hors des bornes [0,1], on indique qu'il y a une erreur, 
	// et la valeur de float est alors soit 0 soit 1.
	if (f < 0)
	{
		cout << "Creation du float+ impossible, valeur hors limite." << endl;
		bits = 0;
	}
	else if (f>1)
	{
		cout << "Creation du float+ impossible, valeur hors limite." << endl;
		bits = 0xFFFFFFFF;
	}
	else if (f == 0) bits = 0; // Cas o� on a 0
	else
	{
		// Sinon on extrait la mantisse et l'exposant du float.
		// Pour ce faire, on cr�e un unsigned int qui correspond au float.
		unsigned int* intf = reinterpret_cast<unsigned int*>(&f); 
		// La fonction reinterpret_cast nous permet ceci

		// Comme l'exposant d'un float se situe entre -126 et 127
		// et que le notre se situe entre -255 et 0, on y ajoute 128
		int exp = (*intf >> 23)+128;
		// On prend les 24 bits de mantisse du double, et on les mets dans le float+
		bits = ((*intf & 0x007FFFFF) << 1);
		// Puis on ajoute notre nouveau exposant.
		bits += (exp << 24);


		int exp_temp = ((bits >> 24) - 255);
		precision = pow(2, -24) * pow(2, exp_temp);

	
	}
}

//////// Op�rateur d'addition +
//
// Tout les op�rateurs sont cod�s sur le principe de la norme IEEE754 bien �videment.
//
// Pour l'addition, on prend d'abord la diff�rence d'exposant, et on prend nos mantisses, normalis�es, c'est � dire 1.(mantisse)
// Ensuite on d�cale la mantisse correspondant au plus petit exposant, de la diff�rence des exposant.
// Si exp1 = -3 et mant1 = 100100
//    exp2 = -5 et mant2 = 110110
// Alors on prend exp1 - exp2 = 2, et donc on veut mant2 =>> 2, donc mant2 = 001101.
// 
// On peut ensuite additionner nos mantisses, qui sera notre mantisse finale. L'exposant correspond au plus gros exposant
// Si jamais, on normalise la mantisse si elle n'est pas de forme 1.mantisse, tout en modifiant l'exposant


Float_plus Float_plus::operator+(const Float_plus & fp)
{
	Float_plus res;
	int mant1 = (bits & 0x00FFFFFF) + 0x01000000;
	int mant2 = (fp.bits & 0x00FFFFFF) + 0x01000000;
	int exp1 = ((bits >> 24) - 255);
	int exp2 = ((fp.bits >> 24) - 255);
	int temp;


	// Si les exposant sont �gaux, on peut directement additioner les mantisses
	if (exp1 - exp2 == 0) {

		// Si la mantisse n'est pas aux normes, on doit la d�caler d'un sur la droite
		// et ajouter 1 � l'exposant
		if (mant1 + mant2 > 0x01FFFFFF)
		{
			if (exp2)
			{
				res.bits = (((mant1 + mant2) >> 1) & 0x00FFFFFF);
				res.bits += ((fp.bits & 0xFF000000) + 0x01000000);
			}
			// Si jamais l'exposant �tait �gal � 0, alors notre r�sultat vaut 1.
			else
			{
				res.bits = 0xFFFFFFFF;
			}

		}
		// Sinon on ajoute notre nouvelle mantisse, avec l'exposant actuel. 
		// (normalement ce cas n'arrive pas, car si les exposant sont �gaux
		// la somme des mantisses ne sera jamais normalis�e.
		else
		{
			res.bits = ((mant1 + mant2) & 0x00FFFFFF);
			res.bits += (fp.bits & 0xFF000000);
		}
	}

	// Si exp1 < exp2, on fait comme indiqu�, et on d�cale mant1 de la diff�rence des exposants vers la droite
	else if (exp1 - exp2 < 0) {
		temp = mant1 >> (exp2 - exp1);

		// Encore une fois, si la somme n'est pas normalis�e, on la normalise, 
		// et on ajoute 1 � l'exposant final
		if (temp + mant2 > 0x01FFFFFF)
		{
			if (exp2)
			{
				res.bits = (((temp + mant2) >> 1) & 0x00FFFFFF);
				res.bits += ((fp.bits & 0xFF000000) + 0x01000000);
			}
			else
			{
				res.bits = 0xFFFFFFFF;
			}
		}
		else
		{
			res.bits = ((temp + mant2) & 0x00FFFFFF);
			res.bits += (fp.bits & 0xFF000000);
		}
	}
	// Dernier cas, si exp1 > exp2, c'est mant2 qu'on d�cale vers la droite cette fois.
	else if (exp1 - exp2 > 0) {
		temp = mant2 >> exp1 - exp2;
		if (temp + mant1 > 0x01FFFFFF)
		{
			res.bits = ((temp + mant1 >> 1) & 0x00FFFFFF);
			res.bits += ((this->bits & 0xFF000000) + 0x01000000);
		}
		else
		{
			if (exp1)
			{
				res.bits = ((temp + mant1) & 0x00FFFFFF);
				res.bits += (this->bits & 0xFF000000);
			}
			else
			{
				res.bits = 0xFFFFFFFF;
			}
		}

	}
	res.precision = precision + fp.precision;
	return res;
}

//////// Op�rateur de soustraction -
//
// La soustraction fonctionne tr�s similairement � l'addition.
// On prend encore la diff�rence d'exposant, et on d�cale la mantisse dont l'exposant est le plus petit 
// vers la droite, d'un nombre correspondant � la diff�rence d'exposant.
// Puis on fait �videmment la diff�rence de mantisse, qui donne notre mantisse finale si elle est d�j� normalis�e.
//
// Cette fois ci, la normalisation se passe diff�rement. La mantisse ne se trouveras probablement pas sous la forme 1.(mantisse), mais plutot
// 0.0001(mantisse). On veut alors d�caler vers la gauche jusqu'� obtenir une forme normalis�e.
// Chaque fois qu'on d�caleras vers la gauche, l'exposant diminueras d'1.


Float_plus Float_plus::operator-(const Float_plus & fp)
{
	Float_plus res;
	int exp = 0;
	int exp1 = ((bits >> 24) - 255);
	int exp2 = ((fp.bits >> 24) - 255);
	int mant1 = (bits & 0x00FFFFFF) + 0x01000000;
	int mant2 = (fp.bits & 0x00FFFFFF) + 0x01000000;
	int temp = (mant1 - mant2);
	
	// Dans le cas ou les exposant sous �gaux, on veut que mant1>mant2, sinon le r�sultat n�gatif.
	if (exp1 - exp2 == 0) {
		if (mant1 > mant2)
		{
			temp = temp & 0x00FFFFFF;

			// Comme d�j� indiqu�, tant que la diff�rence des mantisses n'est pas de la forme 1.(mantisse), on la d�cale vers la gauche
			// et fait exp+=1. Comme on prend -exp plus tard, c'est comme ci on diminuait notre exposant d'1 � chaque it�rations.
			while(temp < (0x01000000))
			{
				temp = temp << 1;
				exp -= 1;
				
			}
			cout << exp << endl;
			res.bits = temp&0x00FFFFFF;
			res.bits += ((((fp.bits & 0xFF000000)>>24) + exp)<<24);
			
			
		}
		// Si jamais mant1<mant2, le r�sultat est n�gatif mais on retourne 0.
		else
		{
			res.bits = 0;
		}
	}
	// Dans le cas ou exp1<exp2, le r�sultat est aussi n�gatif, on retourne donc 0, peut importe les mantisses.
	else if (exp1 - exp2 < 0) {

		res.bits = 0;
	}
	//Dans le cas ou exp1>exp2, le r�sultat est forc�ment positif.
	else if (exp1 - exp2 > 0) {
		temp = mant2 >> exp1 - exp2;
		temp = mant1 - temp;
		// Dans ce cas-l� la normalisation est moins compliqu�e, et on proc�de comme pour l'addition.
		// Soit la mantisse est deja normalis�e, soit on doit la d�caler que d'1 vers la gauche.
		if (temp > 0x01000000)
		{
			res.bits = temp & 0x00FFFFFF;
			res.bits += (this->bits & 0xFF000000);
		}
		else
		{
			res.bits = temp << 1;
			res.bits += ((this->bits & 0xFF000000) - 0x02000000);
		}

	}
	res.precision = precision + fp.precision;
	return res;
}

//////// Op�rateur dde multiplication * 
//
// La multiplication parait, aux premiers abords compliqu�s, mais en fin de compte elle est relativement facile.
// On s'occupe d'abord des cas ou l'un des produits vaut 0 ou 1.
// Si jamais l'un des deux vaut 0, le r�sulta vaut bien �videment 0.
// Si l'un des deux vaut 1, le r�sultat vaut celui qui ne vaut pas 1.
//
// Cette fois ci, notre exposant final correspond � la somme des exposants.
// Peu importe la valeur de ces exposants, on fait la multiplication des mantisses, qu'on stocke dans un
// uint64_t, car les mantisse �tant sur 1+24 bits, le produit de ces mantisses sur trouve sur plus de 51 bits.
//
// On d�cale ensuite ce produit de mantisse de 24 vers la droite, puis on regarde s'il faut le normaliser ou non.


Float_plus Float_plus::operator*(const Float_plus & fp)
{
	Float_plus res;
	int exp1 = ((bits >> 24) - 255);
	int exp2 = ((fp.bits >> 24) - 255);
	int exp = (exp1 + exp2)-1, i=0;
	uint64_t n1 = (bits & 0x00FFFFFF) + 0x01000000;
	uint64_t n2 = (fp.bits & 0x00FFFFFF) + 0x01000000;
	uint64_t temp, temp2;
	Float_plus fp_temp = fp;

	// Si la somme des exposant est trop petite, on arrondit le r�sultat � 0.
	if (exp < -255)
	{
		res.bits = 0;
		res.precision = 0;
	}
	// Cas ou l'un des deux multipli� vaut 0.
	else if (this->bits == 0 || fp.bits == 0)
	{
		res.bits = 0;
		res.precision = 0;
	}
	// Cas ou this vaut 1.
	else if (this->bits == 0xFFFFFFFF)
	{
		res.bits = fp.bits;
		res.precision = fp.precision;
	}
	// Cas ou fp vaut 1.
	else if (fp.bits == 0xFFFFFFFF)
	{
		res.bits = this->bits;
		res.precision = precision;
	}
	// Si on peut faire le calcul :
	else
	{
		// Notre nouvelle mantisse temp est stock�e dans un uint64_t.
		temp = (n1*n2)>>24;

		if ((n1*n2) & 0x800000) temp += 1;
		// Cette ligne permet d'arrondir la valeur de la multiplication
		// En effet, en fesant un d�calage vers la droite de 24, on supprime 24 valeurs
		// Afin de ne pas perdre en pr�cision, on regarde si le dernier bit qu'on supprime vaut '1',
		// Si ce bit vaut bien '1', on arrondit en ajoutant 1 � temp.
		


		if (temp < 0x1FFFFFF) // Cas deja normalis�
		{
			res.bits = temp & 0x00FFFFFF;
			res.bits += exp << 24;
		}
		else // Cas a normaliser
		{
			res.bits = (temp >> 1) & 0x00FFFFFF;
			res.bits += (exp+1) << 24; // Si on normalise, on ajoute 1 � l'exposant
		}
		res.precision = ((precision / get_double()) + (fp.precision / fp_temp.get_double()))*res.get_double();
	}
	
	return res;
}

//////// Op�rateur de division / 
//
// La division est aussi quant � elle relativement simple
// On s'occupe, comme la mutliplication, des valeurs hors limites, donc
// Si on divise pas 0, on retourne une erreur
// Si on divise 0, on retourne 0
// Si on divise par 1, on obtient la valeur de d�part
// Si on divise 1, on obtient un nombre trop grand, donc on met 1.
//
// Cette fois-ci, notre exposant correspond � la diff�rence des exposants
// Pour la division, on veut diviser les mantisses. Hors contrairement � l'addition, soustraction et multiplication,
// diviser 2 mantisses pose des probl�mes d'arrondi. On fait alors correpondre notre 1.(mantisse) � des float, qui se
// situeront forc�ment entre 1 et 1.9999999...
// Ainsi on peut prendre la mantisse obtenue, et la placer dans notre r�sultat.
//
// On fait bien �videment attention � ce que le float r�sultant soit aussi compris entre 1 et 1.9999... sinon, on doit 
// normaliser. 


Float_plus Float_plus::operator/(const Float_plus & fp)
{
	Float_plus res;
	int exp = (((bits >> 24) - 255) - (((fp.bits) >> 24) - 255));

	// Cas o� la diff�rence des exposant est sup�rieur � 0, donc o� le r�sultat est trop grand
	if (exp > 0)
	{
		cout << "Erreur, r�sultat de division trop grand"<<endl;
		res.bits = 0xFFFFFFFF;
	}
	// Cas o� on divise par 0
	else if (fp.bits == 0)
	{
		cout << "Erreur, division par 0" << endl;
		res.bits = 0xFFFFFFFF;
	}
	// Cas o� on divise 0
	else if (bits == 0)
	{
		res.bits = 0;
	}
	// Cas o� on divise par 1
	else if (fp.bits == 0xFFFFFFFF)
	{
		res.bits = res.bits;
	}
	// Cas o� on divise 1, donc o� le r�sultat est trop grand
	else if (bits == 0xFFFFFFFF)
	{
		cout << "Erreur, r�sultat de division trop grand" << endl;
		res.bits = 1;
	}
	// Sinon
	else
	{
		uint32_t temp2 = ((fp.bits)&0x00FFFFFF)>>1;
		temp2 += 0x3F800000;
		uint32_t temp1 = ((bits) & 0x00FFFFFF)>>1;
		temp1 += 0x3F800000;
		// On a cr��, sur temp1 et temp2, des float "artificiels" � partir de nos float+, 
		// afin d'avoir des floats compris entre 1 et 1.9999...
		// Puis on cr�e des float � partir de nos uint32_t
		float* f1 = reinterpret_cast<float*>(&temp1);
		float* f2 = reinterpret_cast<float*>(&temp2);
		float f = *f1 / *f2;
		// Une fois que la division est faite, on recr�e un int � partir des bits du float
		unsigned int* intf = reinterpret_cast<unsigned int*>(&f);
		if (f >= 1) // Cas d�j� normalis�
		{
			res.bits = (*intf & 0x007FFFFF) << 1;
			res.bits += (255+exp) << 24;
		}
		else // Cas a normaliser
		{
			res.bits = (*intf & 0x007FFFFF) << 1;
			res.bits += (254+exp) << 24;
		}
	}
	return res;
}


// Op�rateur =(float), qui revient au constructeur � partir d'un float
Float_plus Float_plus::operator=(const float & fp)
{
	return Float_plus(fp);
}


//-------------------------------------------------------------------
// Liste de op�rateurs de comparaison
//
// Toutes les comparaisons sont extremement simples a effectuer.
// C'est du � la mani�re dont un float et un double sont cod�s
// 000000...00 est la plus petite valeur d'un double+ et 111111...11 est la plus grande
// La valeur qui suit 00000..00 est 00000..01, puis 000000..10, etc...
// Ainsi lorsqu'un double+ est plus garnd qu'un autre double+, alors sa valeur, qu'on stocke dans un int
// est forc�ment plus grande, et cela fonctionne pour toutes les comparaisons. Il suffit ainsi
// de retourner la comparaison des bits, pour obtenir la comparaison d'un float+
//
//-------------------------------------------------------------------------

/////////// Comparaison d'�galit� avec un int     ==

bool Float_plus::operator==(const unsigned int & fp) const
{
	if (bits == fp)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'in�galit� avec un int     !=

bool Float_plus::operator!=(const unsigned int & fp) const
{
	if (bits != fp)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'�galit� avec un Float_plus    ==


bool Float_plus::operator==(const Float_plus & fp) const
{
	if (bits == fp.bits)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'in�galit� avec un Float_plus    !=

bool Float_plus::operator!=(const Float_plus & fp) const
{
	if (bits != fp.bits)
	{
		return true;
	}
	else return false;
}

///////////  Comparaison inf�rieur ou �gal avec un Float_plus   <=

bool Float_plus::operator<=(const Float_plus & fp) const
{
	if (bits <= fp.bits) return true;
	else return false;
}

///////////  Comparaison inf�rieur avec un Float_plus   <

bool Float_plus::operator<(const Float_plus & fp) const
{
	if (bits < fp.bits) return true;
	else return false;
}

///////////  Comparaison sup�rieur ou �gal avec un Float_plus   >=

bool Float_plus::operator>=(const Float_plus & fp) const
{
	if (bits >= fp.bits) return true;
	else return false;
}

///////////  Comparaison sup�rieur ou �gal avec un Float_plus   >

bool Float_plus::operator>(const Float_plus & fp) const
{
	if (bits > fp.bits) return true;
	else return false;
}

/////////////

uint32_t Float_plus::get_bits()
{
	return bits;
}

//////////// Fonction qui permet d'�lever un float_plus � une certaine puissance, d�finie
///////////  par un entier non sign�

Float_plus pow(Float_plus fp, unsigned int i) 
{
	Float_plus res = fp.get_bits();
	if (i == 0) {
		// SI jamais on jait puissance 0, autant retouner 1 directement
		Float_plus res = 0b11111111111111111111111111111111;
		return res;
	}
	else {
		for (unsigned int j = 1; j < i; j++)
		{
			//On utilise juste l'op�rateur *, le nombre de fois sp�cifi�
			fp = res * fp;
		}
		return fp;
	}
}















//---------------------------------------------------------------------------------//
//																				   //
//  Un double_plus fonction similairement � un float_plus, mais sur 64 bits		   //
// 																				   //
//  Un double_plus est un double cod� sur 64 bits, situ� entre 0 et 1			   //
//  cod� sur le m�me principe que la norme IEEE754, et in�vitablement plus pr�cis  //
//  car un double habituel se situe entre des bornes bien plus larges			   //
//  Ce double_plus poss�de 12 bits d'exposant, pour un exposant entre 0 et -4095   //
//  et 52 bits de mantisse.														   //
// 																				   //
//  Valeur minimum d'un float_plus  : 0											   //
//  Valeur maximum d'un float_plus  : 1											   //
//  Plus petite valeur d�normalis�e : 1.91499549e-1233						       //
//  Plus grande valeur d�normalis�e : 0.9999999999999998889776975				   //
//																				   //
//---------------------------------------------------------------------------------//


// Fonction simple qui affiche les bits du double, sous forme d'un uint64_t;
void Double_plus::Affiche_binaire(void)
{
	cout << bits << endl;
}

// Fonction simple qui affiche le double associ� � la pr�cision;
void Double_plus::Affiche_precision(void)
{
	cout << "La precision du double est de plus ou moins " << precision << "." << endl;
}

// Fonction qui permet d'afficher la mantisse et l'exposant du double_plus, au cas ou la valeur
// est hors des limites d'un double normal.
// Affiche sous la forme : Valeur du double plus : 1.mantisse * 2^(exposant)
void Double_plus::Affiche_mantisse(void)
{
	streamsize prec = cout.precision();
	double d=1;
	uint64_t temp = 1;
	uint64_t exp = (4095-(bits >> 52))&0xFFF;
	if (bits != 0)
	{
		for (unsigned int i = 0; i < 52; i++)
		{
			if (bits&(temp))
			{
				d += (2 / pow(2, 53 - i));
				// Chaque bits de la mantisse correspond � 2^-(sa position)
				// On ajoute donc cette valeur � un double chaque fois que ce bit vaut 1.
			}
			temp *= 2;
		}

		cout << "Valeur du double plus : " << setprecision(25) << d << " * 2^(-" << exp << ")" <<setprecision(prec)<< endl;
	}
	else cout << "Valeur du double plus : 0" << endl;
	// On affiche directement 0 si la valeur de l'uint64 vaut 0.
}

// Comme l'uint64_t bits est en private, on cr�e une fonction pour y acc�der.
uint64_t Double_plus::get_bits(void)
{
	return bits;
}


// Cette fonction permet d'afficher grossi�rement la valeur de notre double_plus.
// Comme un double est bien moins pr�cis que notre double_plus, la valeur a de fortes
// chances d'�tre erron�e.
double Double_plus::get_double(void)
{
	unsigned long long temp = 1;
	double valeur;
	if (bits == 0)
	{
		valeur = 0;
	}
	else if (bits >= 0xFFF0000000000000) {
		valeur = 1;
	}
	else {
		valeur = 1;
		for (unsigned int i = 0; i < 52; i++)
		{
			if (bits&(temp))
			{
				valeur += (2 / pow(2, 53 - i));

			}
			temp *= 2;

		}
		int exp = ((bits >> 52) - 4095);
		valeur *= (1 / pow(2, (-exp)));
		// La m�thode de calcul revient � stocker 1.mantisse*2(exposant) dans un double, puis l'afficher
	}
	return valeur;
}

// On r�utilise get_double pour faire une fonction affiche_float
void Double_plus::Affiche_float(void)
{
	cout << "La valeur approximative du Double_plus est : " << setprecision(25) << get_double() << endl;
}


/////// Tout les constructeurs :

// Constructeur de base. La valeur du double est 0 par d�faut
Double_plus::Double_plus()
{
}


// Constructeur � partir d'un uint64_t. Permet d'initialiser facilement un double+ de notre choix, 
// lorsqu'il se situe hors des limites d'un double
Double_plus::Double_plus(uint64_t b)
{
	bits = b;
	long long exp = ((bits >> 52) - 4095);
	precision = pow(2, -52) * pow(2, exp);
}

// Constructeur � partir d'un int. Revient � la m�me chose qu'un uint64_t
Double_plus::Double_plus(int b)
{
	bits = b;
	long long exp = ((bits >> 52) - 4095);
	precision = pow(2, -52) * pow(2, exp);
}

// Constructeur � partir d'un double. Permet de choisir facilement la valeur de notre double+
// lorsqu'on veut une valeur pas trop grande ou petite. 
Double_plus::Double_plus(double f)
{

	// Si jamais le double ne se trouve pas entre 0 et 1, on a une erreur.
	if (f < 0)
	{
		cout << "Creation du double+ impossible, valeur hors limite." << endl;
		bits = 0;
	}
	else if (f > 1)
	{
		cout << "Creation du double+ impossible, valeur hors limite." << endl;
		bits = 0xFFFFFFFFFFFFFFFF;
	}
	else if (f == 0)
	{
		bits = 0;

	}
	else
	{
		// Sinon on extrait la mantisse et l'exposant du double.
		// Pour ce faire, on cr�e un long long int qui correspond au double.
		unsigned long long* intf = reinterpret_cast<unsigned long long*>(&f);

		// Comme l'exposant d'un double se situe entre -1022 et 1023
		// et que le notre se situe entre -4095 et 0, on y ajoute 3072
		unsigned long long exp = ((*intf >> 52) + 3072);

		// On prend les 52 bits de mantisse du double, et on les mets dans le double+
		bits = (*intf & 0xFFFFFFFFFFFFF);
		// Puis on ajoute notre nouveau exposant.
		bits += (exp << 52);		
	}
	long long exp = ((bits >> 52) - 4095);
	precision = pow(2, -52) * pow(2, exp);
}



//////// PARTIE CONCERNANT LES OPERATEURS
/////// /Fonctionne exactement sur le m�me principe que le float_plus, mais sur 64 bits.

//////// Op�rateur de construction =

Double_plus Double_plus::operator=(const double & fp)
{
	return Double_plus(fp);
}

//////// Op�rateur d'addition +
//
//
// Fonctionne sur le m�me principe sur pour le float+, voir commentaires ci-dessus
//
//

Double_plus Double_plus::operator+(const Double_plus & fp)
{
	Double_plus res;
	uint64_t temp;
	int exp1 = ((bits >> 52) - 4095);
	int exp2 = ((fp.bits >> 52) - 4095);
	uint64_t mant1 = (bits & 0x000FFFFFFFFFFFFF) + 0x0010000000000000;
	uint64_t mant2 = (fp.bits & 0x000FFFFFFFFFFFFF) + 0x0010000000000000;
	if (!exp1 || !exp2) res.bits = 0xFFFFFFFFFFFFFFFF;
	else if (exp1 == exp2) 
	{
		if (exp1)
		{
			res.bits = ((mant1 + mant2 >> 1) & 0x000FFFFFFFFFFFFF);
			cout << (mant1 + mant2 >> 1) << endl;
			res.bits += ((fp.bits & 0xFFF0000000000000) + 0x0010000000000000);
		}
		else
		{
			res.bits = 0xFFFFFFFFFFFFFFFF;
		}

	}
	else if (exp1 < exp2) {
		temp = mant1 >> (exp2 - exp1);
		if (temp + mant2 > 0x001FFFFFFFFFFFFF)
		{
			if (exp2)
			{
				res.bits = ((temp + exp2 >> 1) & 0x000FFFFFFFFFFFFF);
				res.bits += ((fp.bits & 0xFFF0000000000000) + 0x0010000000000000);
			}
			else
			{
				res.bits = 0xFFFFFFFFFFFFFFFF;
			}
		}
		else
		{
			res.bits = ((temp + mant2) & 0x000FFFFFFFFFFFFF);
			res.bits += (fp.bits & 0xFFF0000000000000);
		}
	}
	else if (exp1 > exp2) {
		temp = mant2 >> (exp1 - exp2);
		if (temp + mant1 > 0x001FFFFFFFFFFFFF)
		{
			res.bits = ((temp + mant1 >> 1) & 0x000FFFFFFFFFFFFF);
			res.bits += ((this->bits & 0xFFF0000000000000) + 0x0010000000000000);
		}
		else
		{
			if (exp1)
			{
				res.bits = ((temp + mant1) & 0x000FFFFFFFFFFFFF);
				res.bits += (this->bits & 0xFFF0000000000000);
			}
			else
			{
				res.bits = 0xFFFFFFFFFFFFFFFF;
			}
		}

	}
	res.precision = precision + fp.precision;
	return res;
}

//////// Op�rateur de soustraction -
//
//
// Fonctionne sur le m�me principe sur pour le float+, voir commentaires ci-dessus
//
//

Double_plus Double_plus::operator-(const Double_plus & fp)
{
	Double_plus res;
	int exp1 = ((bits >> 52) - 4095);
	int exp2 = ((fp.bits >> 52) - 4095);
	uint64_t mant1 = (bits & 0x000FFFFFFFFFFFFF) + 0x0010000000000000;
	uint64_t mant2 = (fp.bits & 0x000FFFFFFFFFFFFF) + 0x0010000000000000;
	uint64_t temp = (mant1 - mant2);
	int exp = 0;

	if (exp1 == exp2) {
		if (mant1 > mant2)
		{
			temp = temp & 0x000FFFFFFFFFFFFF;
			while (temp < (0x0010000000000000))
			{
				temp = temp << 1;
				exp += 1;
			}
			res.bits = temp & 0x000FFFFFFFFFFFFF;
			res.bits += ((((fp.bits & 0xFFF0000000000000) >> 52) - exp) << 52);


		}
		else
		{
			res.bits = 0;
		}
	}
	else if (exp1 < exp2) {

		res.bits = 0;
	}
	else if (exp1 > exp2) {
		temp = mant2 >> exp1 - exp2;
		temp = mant1 - temp;
		if (temp > 0x0010000000000000)
		{
			res.bits = temp & 0x000FFFFFFFFFFFFF;
			res.bits += (this->bits & 0xFFF0000000000000);
		}
		else
		{
			res.bits = temp << 1;
			res.bits += ((this->bits & 0xFFF0000000000000) - 0x0020000000000000);
		}

	}
	res.precision = precision + fp.precision;
	return res;
}

//////// Op�rateur de multplication *
//
// L'op�rateur de multiplication est le suel op�rateur qui fonctionne assez diff�rement pour le double+
// L'id�e g�n�rale est la m�me, mais multiplier 2 mantisses de 52+1 bits n�cessite d'avoir acc�s �
// un int de 128 bits, ce qui n�cessite une autre librairie. Je suis parti du principe que ce n'�tait pas
// absolument n�cessaire. J'ai donc fait la multiplication de 2 ints de 53 bits tout en restant dans un uint64_t
// La m�thode ne fonctionne que pour des mantisses, mais le r�sultat est bon.
//

Double_plus Double_plus::operator*(const Double_plus & fp)
{

	// L'op�ration bit � bit >> est limit� � 32 bits, on la remplace donc par *pow(2,52) ici

	Double_plus res;
	long int exp1 = ((bits >> 52) - 4095);
	long int exp2 = ((fp.bits >> 52) - 4095);
	long int exp = (exp1 + exp2)-1, i = 0;
	Double_plus fp_temp = fp;



	if (exp <= -4095)
	{
		res.bits = 0;
		res.precision = 0;
	}
	else if (this->bits == 0 || fp.bits == 0)
	{
		res.bits = 0;
		res.precision = 0;
	}
	else if (this->bits == 0xFFFFFFFFFFFFFFFF)
	{
		res.bits = fp.bits;
		res.precision = fp.precision;
	}
	else if (fp.bits == 0xFFFFFFFFFFFFFFFF)
	{
		res.bits = this->bits;
		res.precision = precision;
	}
	else
	{

		//// Partie tr�s important et qui a �t� compliqu� � faire.
		//// Comme la multiplication de 2 mantisses de 52+1 bits doit �tre stock�
		//// dans un int de AU MOINS 106 bits, et que je n'ai pas su avoir acc�s �
		//// des bits aussi longs sans devoir emprunter dans des librairies sur le net,
		//// j'ai cr�� une boucle qui fait la multiplication des 2 mantisses, tout en gardant
		//// le calcul dans un int de 64 bits.

		uint64_t calcul = 0;
		uint64_t deux = 1;
		uint64_t mant1 = (bits & 0x000FFFFFFFFFFFFF) + 0x0010000000000000;
		uint64_t mant2 = (fp.bits & 0x000FFFFFFFFFFFFF) + 0x0010000000000000;
		uint64_t expplus = 4096 + exp;

		for (unsigned int i = 0; i < 52; i++)
		{
			calcul = calcul >> 1;
			if (mant2&deux) calcul += mant1;
			deux *= 2;
		}
		calcul += (mant1 << 1);
		if (calcul & 1) calcul += 1;
		calcul = calcul >> 1;

		//////////////////////////////////////
		//
		// Ce calcul nous donne la nouvelle mantisse sur 52+1 bits.
		// Si cette mantisse se trouve � 52+2 bits, on normalise en augmentant l'exposant
		//
		///////////////////////////////////
		if (calcul < 0x1FFFFFFFFFFFFF)
		{
			res.bits = (calcul) & 0x000FFFFFFFFFFFFF;
			expplus = (expplus)*pow(2, 52);
			expplus = expplus & 0xFFF0000000000000;
			res.bits += expplus;
		}
		
		else
		{
			res.bits = (calcul >> 1) & 0x000FFFFFFFFFFFFF;
			expplus = (expplus + 1)*pow(2, 52);
			expplus = expplus & 0xFFF0000000000000;
			res.bits += expplus;
			
		}

		res.precision = ((precision / get_double()) + (fp.precision / fp_temp.get_double()))*res.get_double();
	}
	return res;
}

Double_plus pow(Double_plus fp, unsigned int i)
{
	Double_plus res = fp.get_bits();
	if (i == 0) {
		Double_plus res = 0xFFFFFFFFFFFFFFFF;
		return res;
	}
	else {
		for (unsigned int j = 1; j < i; j++)
		{
			fp = res * fp;
		}
		return fp;
	}
}

//////// Op�rateur de division / 
//
//
// Fonctionne sur le m�me principe sur pour le float+, voir commentaires ci-dessus
//
//

Double_plus Double_plus::operator/(const Double_plus & fp)
{
	Double_plus res;
	int64_t exp = (((bits >> 52) - 4095) - (((fp.bits) >> 52) - 4095));
	Double_plus fp_temp = fp;

	if (fp.bits == 0)
	{
		cout << "Erreur, division par 0" << endl;
		res.bits = 0xFFFFFFFFFFFFFFFF;
	}
	else if (exp > 0)
	{
		cout << "Erreur, resultat de division trop grand" << endl;
		res.bits = 0xFFFFFFFFFFFFFFFF;
	}
	else if (bits == 0)
	{
		res.bits = 0;
	}
	else if (fp.bits == 0xFFFFFFFFFFFFFFFF)
	{
		res.bits = res.bits;
	}
	else if (bits == 0xFFFFFFFFFFFFFFFF)
	{
		cout << "Erreur, resultat de division trop grand" << endl;
		res.bits = 0xFFFFFFFFFFFFFFFF;
	}

	else
	{
		uint64_t temp2 = ((fp.bits) & 0x000FFFFFFFFFFFFF);
		temp2 += 0x3FF0000000000000;
		uint64_t temp1 = ((bits) & 0x000FFFFFFFFFFFFF);
		temp1 += 0x3FF0000000000000;
		double* f1 = reinterpret_cast<double*>(&temp1);
		double* f2 = reinterpret_cast<double*>(&temp2);
		double f = *f1 / *f2;
		unsigned long long* intf = reinterpret_cast<unsigned long long*>(&f);
		if (f >= 1)
		{
			res.bits = (*intf & 0x000FFFFFFFFFFFFF);
			res.bits += (4095 + exp) << 52;
		}
		else
		{
			res.bits = (*intf & 0x000FFFFFFFFFFFFF);
			res.bits += (4094 + exp) << 52;
		}
		
	}
	res.precision = ((precision / get_double()) + (fp.precision / fp_temp.get_double()))*res.get_double();
	return res;
}


//-------------------------------------------------------------------
// Liste de op�rateurs de comparaison
//
// Toutes les comparaisons sont extremement simples a effectuer.
// C'est du � la mani�re dont un float et un double sont cod�s
// 000000...00 est la plus petite valeur d'un double+ et 111111...11 est la plus grande
// La valeur qui suit 00000..00 est 00000..01, puis 000000..10, etc...
// Ainsi lorsqu'un double+ est plus garnd qu'un autre double+, alors sa valeur, qu'on stocke dans un int
// est forc�ment plus grande, et cela fonctionne pour toutes les comparaisons. Il suffit ainsi
// de retourner la comparaison des bits, pour obtenir la comparaison d'un double+
//
//-------------------------------------------------------------------------


/////////// Comparaison d'�galit� avec un int     ==

bool Double_plus::operator==(const unsigned int & fp) const
{
	if (bits == fp)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'�galit� avec un Double_plus    ==

bool Double_plus::operator==(const Double_plus & fp) const
{
	if (bits == fp.bits)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'�galit� avec un double    ==

bool Double_plus::operator==(const double & d) const
{
	Double_plus fp = d;
	if (bits == fp.bits)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'in�galit� avec un int     !=

bool Double_plus::operator!=(const unsigned int & fp) const
{
	if (bits != fp)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'in�galit� avec un Double_plus    !=

bool Double_plus::operator!=(const Double_plus & fp) const
{
	if (bits != fp.bits)
	{
		return true;
	}
	else return false;
}

/////////// Comparaison d'in�galit� avec un double   !=

bool Double_plus::operator!=(const double & d) const
{
	Double_plus fp = d;
	if (bits != fp.bits)
	{
		return true;
	}
	else return false;
}


///////////  Comparaison inf�rieur ou �gal avec un Double_plus   <=

bool Double_plus::operator<=(const Double_plus & fp) const
{
	if (bits <= fp.bits) return true;
	else return false;
}

///////////  Comparaison inf�rieur ou �gal avec un double   <=

bool Double_plus::operator<=(const double & d) const
{
	Double_plus fp = d;
	if (bits <= fp.bits) return true;
	else return false;
}

///////////  Comparaison inf�rieur avec un Double_plus   <

bool Double_plus::operator<(const Double_plus & fp) const
{
	if (bits < fp.bits) return true;
	else return false;
}

///////////  Comparaison inf�rieur ou �gal avec un double   <

bool Double_plus::operator<(const double & d) const
{
	Double_plus fp = d;
	if (bits < fp.bits) return true;
	else return false;
}

///////////  Comparaison sup�rieur ou �gal avec un Double_plus   >=

bool Double_plus::operator>=(const Double_plus & fp) const
{
	if (bits >= fp.bits) return true;
	else return false;
}

///////////  Comparaison inf�rieur ou �gal avec un double   >=

bool Double_plus::operator>=(const double & d) const
{
	Double_plus fp = d;
	if (bits >= fp.bits) return true;
	else return false;
}

///////////  Comparaison sup�rieur ou �gal avec un Double_plus   >

bool Double_plus::operator>(const Double_plus & fp) const
{
	if (bits > fp.bits) return true;
	else return false;
}

///////////  Comparaison inf�rieur ou �gal avec un double   >

bool Double_plus::operator>(const double & d) const
{
	Double_plus fp = d;
	if (bits > fp.bits) return true;
	else return false;
}