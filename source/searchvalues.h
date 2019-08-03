#ifndef SEARCHVALUES_H
#define SEARCHVALUES_H

#include <QPair>

struct SearchValues {
	QPair<int, int> age;
	QPair<int, int> potential;
	QPair<int, int> salary;
	QPair<int, int> dmi;
	int countryid;
	int divCount;
	bool div[6];
	bool countrySet;
	bool nationalitySet;
};

#endif // SEARCHVALUES_H
