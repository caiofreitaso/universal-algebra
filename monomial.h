#ifndef MONOMIAL_H
#define MONOMIAL_H

#include "product.h"

template <class K, class T = unsigned>
struct Monomial {
	K coefficient;
	Product<T> terms;

	Monomial() : coefficient(1) { }
	Monomial(Monomial const& c) : coefficient(c.coefficient), terms(c.terms) { }
	Monomial(K c, Product<T> p) : coefficient(c), terms(p) { }
	Monomial(Product<T> p) : coefficient(1), terms(p) { }

	bool isPolylinear() const {
		for (unsigned i = 0; i < terms.size(); i++)
			for (unsigned j = i+1; j < terms.size(); j++)
				if (terms[i] == terms[j])
					return false;
		return true;
	}

	bool operator==(Monomial const& r) const {
		return coefficient == coefficient && terms == r.terms;
	}
	bool operator!=(Monomial const& r) const { return !(*this == r); }
};

template <class K, class T = unsigned>
class Polynomial : public std::vector<Monomial<K,T> > {
	public:
		void optimize() {
			for (unsigned i = 0; i < this->size(); i++)
				for (unsigned j = i+1; j < this->size(); j++)
					if ((*this)[i].terms == (*this)[j].terms) {
						(*this)[i].coefficient += (*this)[j].coefficient;
						this->erase(this->begin()+j);
						j--;
					}
		}
		bool isHomogeneous() const {
			if (this->size()) {
				unsigned size = (*this)[0].terms.size();
				for (unsigned i = 1; i < this->size(); i++)
					if ((*this)[i].terms.size() != size)
						return false;
			}
			return true;

		}
};
#endif