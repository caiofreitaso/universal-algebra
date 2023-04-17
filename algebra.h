#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "monomial.h"
#include "matrix.h"
#include <time.h>
#include <omp.h>

template<class K, class T>
class Algebra;

template<class K, class T>
struct Relation {
	Polynomial<K,T> monomials;
};

template<class K>
struct Identity : public Relation<K,unsigned> {
	template<class T>
	#ifdef _MSC_VER
	void createSingle(Algebra<K,T>& algebra, std::vector<Product<T*> const*>& re, SparseMatrix<K>& matrix, unsigned size, unsigned sum = 0, unsigned index = 0) const {
	#else
	void createSingle(Algebra<K,T>& algebra, Product<T*> const* re[], SparseMatrix<K>& matrix, unsigned size, unsigned sum = 0, unsigned index = 0) const {
	#endif
		if (index < this->monomials[0].terms.terms()) {
			if (index < this->monomials[0].terms.terms() - 1)
				for (unsigned i = 1; i <= (size-sum) - ((this->monomials[0].terms.terms()-1) - index); i++)
					for (unsigned k = algebra.start(i); k < algebra.start(i+1); k++) {
						re[index] = &algebra[k].element;
						createSingle(algebra,re,matrix,size,sum+i,index+1);
					}
			else
				for (unsigned k = algebra.start(size - sum); k < algebra.start(size - sum + 1); k++) {
					re[index] = &algebra[k].element;
					createSingle(algebra,re,matrix,size,sum,index+1);
				}
		} else {
			std::vector<Product<T*> > v;

			for (unsigned i = 0; i < this->monomials.size(); i++)
				#ifdef _MSC_VER
				v.push_back(replace(this->monomials[i].terms,re.data(),0));
				#else
				v.push_back(replace(this->monomials[i].terms,re,0));
				#endif

			//std::vector<K> row(algebra.length(size),0);
			MatrixRow<K> row;

			for (unsigned i = 0; i < this->monomials.size(); i++)
				for (unsigned j = algebra.start(size); j < algebra.start(size+1); j++)
					if (v[i] == algebra[j].element) {
						row.set(j - algebra.start(size),row.get(j - algebra.start(size)) + this->monomials[i].coefficient);
						break;
					}

			unsigned misses = 0;
			K tmp;
			unsigned pivot;
			unsigned rows;
			#pragma omp critical
			{
				rows = matrix.rows();
			}
			if (rows)
				while (misses != rows) {
					if (!row.row.size())
						break;

					pivot = row.pivot().index;
					tmp = row.pivot().value;
					for (unsigned i = 0; i < matrix.columns(); i++)
						row.set(i,row.get(i) / tmp);

					misses = 0;
					for (unsigned i = 0; i < rows; i++) {
						unsigned j = 0;
						
						#pragma omp critical
						{
							j = matrix[i].pivot().index;
						}
					
						if (j == pivot) {
							tmp = row.get(j);

							#pragma omp critical
							{
								for(unsigned k = 0; k < matrix.columns(); k++)
									row.set(k,row.get(k) - tmp * matrix[i].get(k));
							}
							
							if (!row.row.size())
								return;
							i = 0;
							misses = 0;
							pivot = row.pivot().index;
						} else
							misses++;
					}
				}
			else {
				if (!row.row.size())
					return;
				tmp = row.pivot().value;
				for (unsigned i = 0; i < matrix.columns(); i++)
					row.set(i,row.get(i) / tmp);
			}

			if (row.row.size()) {
				#pragma omp critical
				{
					matrix.addRow(row);
				}
			}
		}
	}

	public:
		template<class T>
		void generate(Algebra<K,T>& algebra, SparseMatrix<K>& matrix, unsigned size) const {
			if (size < this->monomials[0].terms.terms())
				return;

			#ifdef _MSC_VER
			std::vector<Product<T*> const*> replacements(this->monomials[0].terms.terms(),0);
			#else
			Product<T*> const* replacements[this->monomials[0].terms.terms()];
			#endif

			if (algebra.length(size) > 500) {
				int i, k;
				unsigned max = size - (this->monomials[0].terms.terms()-1);
				std::cout << "Threads: " << max << "\n";
				#pragma omp parallel for private(replacements,k) schedule(guided) num_threads(max)
				for (i = 1; i <= size - (this->monomials[0].terms.terms()-1); i++) {
					#ifdef _MSC_VER
					replacements.reserve(this->monomials[0].terms.terms());
					#endif

					for (k = algebra.start(i); k < algebra.start(i+1); k++) {
						replacements[0] = &algebra[k].element;

						createSingle(algebra,replacements,matrix,size,i,1);
					}
				}
			} else
				createSingle(algebra,replacements,matrix,size);
		}

};

template<class K>
class Ideal : public std::vector<Identity<K> > {};

template<class K>
struct Term {
	K coefficient;
	unsigned index;

	Term(K c, unsigned i) : coefficient(c),index(i) { }
};

template<class K, class T>
struct Equivalence {
	Product<T*> element;
	std::vector<Term<K> > expression;

	Equivalence() {}
	Equivalence(Product<T*> e) : element(e) { }
};

template<class K, class T>
class Algebra {
	std::vector<T> _generators;
	std::vector<unsigned> _nillocal;
	unsigned _nil;
	unsigned _max;

	Ideal<K> _ideal;
	std::vector<Relation<K,T*> > _relations;

	std::vector<Equivalence<K,T> > _set;
	std::vector<unsigned> _setlengths;

	static const unsigned _combinatory[20];

	public:
		Algebra() : _max(0) { }
		Algebra(unsigned nil) : _nil(nil), _max(0) { }

		void setNilpotency(unsigned n) { _nil = n; }

		void addGenerator(T g, unsigned n) {
			_generators.push_back(g);
			_nillocal.push_back(n);
			_max += n;
		}

		void addIdentity(Identity<K> i) { _ideal.push_back(i); }
		void addRelation(Relation<K,T*> r) { _relations.push_back(r); }

		unsigned nilpotency() const { return _nil; }
		unsigned nilpotency(unsigned i) const { return _nillocal[i]; }
		unsigned maxLength() const {
			if (_max < _nil)
				return _max;
			return _nil;
		}

		T& operator()(unsigned i) { return _generators[i]; }
		T const& operator()(unsigned i) const { return _generators[i]; }

		unsigned generators() const { return _generators.size(); }

		Identity<K>& identity(unsigned i) { return _ideal[i]; }
		Identity<K> const& identity(unsigned i) const { return _ideal[i]; }

		Relation<K,T*>& relation(unsigned i) { return _relations[i]; }
		Relation<K,T*> const& relation(unsigned i) const { return _relations[i]; }

		unsigned identities() const { return _ideal.size(); }
		unsigned relations() const { return _relations.size(); }

		unsigned size() const { return _set.size(); }
		Equivalence<K,T> const& operator[](unsigned i) { return _set[i]; }

		unsigned start(unsigned i) const { return _setlengths[i-1]; }
		unsigned length(unsigned i) const { return _setlengths[i] - _setlengths[i-1]; }

		void generate() {
			clock_t totalTime = clock();

			unsigned end = 0;
			unsigned total = 0;
			for (unsigned i = 0, k = 1; i < maxLength(); i++) {
				k *= generators();
				total += _combinatory[i] * k;
			}

			_set.reserve(total);
			_setlengths.reserve(maxLength());

			std::vector<std::vector<unsigned> > count;

			for (unsigned i = 0; i < generators(); i++) {
				_set.push_back(Product<T*>(&_generators[i]));
				count.push_back(std::vector<unsigned>(generators(),0));
				count[count.size()-1][i]++;
			}

			_setlengths.push_back(end);
			end = _set.size();
			_setlengths.push_back(end);
			

			clock_t a;
			for (unsigned length = 2; length <= maxLength(); length++) {
				std::cout << "Building elements of length: " << length << ".\n";
				a = clock();

				for (unsigned k = 1; k <= length/2; k++) {
					unsigned max = length - k;
					if (k != max)
						for (unsigned i = _setlengths[k-1]; i < _setlengths[k]; i++)
							for (unsigned j = _setlengths[max-1]; j < _setlengths[max]; j++) {
								total = 0;

								bool exists = true;
								for (unsigned p = 0; p < generators(); p++)
									if (count[i][p] + count[j][p] > _nillocal[p]) {
										exists = false;
										break;
									} else
										total += count[i][p] + count[j][p];
								if (total > _nil)
									exists = false;

								if (exists) {
									_set.push_back(Product<T*>(_set[i].element,_set[j].element));
									count.push_back(count[i]);
									for (unsigned p = 0; p < generators(); p++)
										count[count.size()-1][p] += count[j][p];
									if (i != j) {
										_set.push_back(Product<T*>(_set[j].element,_set[i].element));
										count.push_back(count[i]);
										for (unsigned p = 0; p < generators(); p++)
											count[count.size()-1][p] += count[j][p];
									}
								}
							}
					else
						for (unsigned i = _setlengths[k-1]; i < _setlengths[k]; i++)
							for (unsigned j = _setlengths[max-1]; j < _setlengths[max]; j++) {
								total = 0;

								bool exists = true;
								for (unsigned p = 0; p < generators(); p++)
									if (count[i][p] + count[j][p] > _nillocal[p]) {
										exists = false;
										break;
									} else
										total += count[i][p] + count[j][p];
								if (total > _nil)
									exists = false;

								if (exists) {
									_set.push_back(Product<T*>(_set[i].element,_set[j].element));
									count.push_back(count[i]);
									for (unsigned p = 0; p < generators(); p++)
										count[count.size()-1][p] += count[j][p];
								}
							}
				}
				a = clock() - a;
				std::cout << "\t\t" << (_set.size()-end) << " @ " << ((double)a)/CLOCKS_PER_SEC << "s.\n\tCreating ideal matrix.\n";
				a = clock();

				end = _set.size();
				_setlengths.push_back(end);

				
				
				SparseMatrix<K> matrix(this->length(length));
				for(unsigned i = 0; i < _ideal.size(); i++)
					_ideal[i].generate(*this,matrix,length);
				
				a = clock() - a;
				std::cout << "\t\tM["<<matrix.rows()<<","<<matrix.columns()<<"] @ " << ((double)a)/CLOCKS_PER_SEC << "s.\n\tGauss-Jordan elimination.\n";
				a = clock();

				matrix = matrix.gaussian();

				
				a = clock() - a;
				std::cout << "\t\tM["<<matrix.rows()<<","<<matrix.columns()<<"] @ " << ((double)a)/CLOCKS_PER_SEC << "s.\n\tDeleting null elements.\n";
				a = clock();
				unsigned deleted = 0;
				for (unsigned i = 0; i < matrix.rows(); i++) {
					unsigned pivot = matrix[i].pivot().index;
					if (pivot < matrix.columns()) {
						bool null = matrix[i].row.size() == 1;

						if (null) {
							_set.erase(_set.begin()+start(length)+pivot-deleted);
							deleted++;
							for (unsigned k = start(length); k < start(length+1); k++)
								for (unsigned j = 0; j < _set[k].expression.size(); j++)
									if (_set[k].expression[j].index > pivot + start(length))
										_set[k].expression[j].index--;
						} else 
							for (typename MatrixRow<K>::iterator it = (matrix[i].row.begin()+1); it != matrix[i].row.end(); it++)
								_set[start(length)+pivot-deleted].expression.push_back(Term<K>(-it->value, start(length)+it->index-deleted));
					}
				}

				end = _set.size();
				_setlengths[length] = end;

				a = clock() - a;
				std::cout << "\t\t" << deleted << " @" << ((double)a)/CLOCKS_PER_SEC << "s.\n";
			}

			if (_relations.size()) {
				std::cout << "Building final matrix.\n";
				a = clock();
				SparseMatrix<K> finalMatrix(_set.size());
				for (unsigned i = 0; i < _set.size(); i++) {
					MatrixRow<K> row;
					for (unsigned j = 0; j < _set[i].expression.size(); j++)
						row.set(_set[i].expression[j].index, _set[i].expression[j].coefficient);
					finalMatrix.addRow(row);
				}
				for (unsigned i = 0; i < _relations.size(); i++) {
					MatrixRow<K> row;
					for (unsigned j = 0; j < _relations[i].monomials.size(); j++)
						for (unsigned k = start(_relations[i].monomials[j].terms.terms()); k < start(_relations[i].monomials[j].terms.terms() + 1); k++)
							if (_relations[i].monomials[j].terms == _set[k].element) {
								row.set(k, _relations[i].monomials[j].coefficient);
								break;
							}
					finalMatrix.addRow(row);
				}

				a = clock() - a;
				std::cout << "\t\tM["<<finalMatrix.rows()<<","<<finalMatrix.columns()<<"] @ " << ((double)a)/CLOCKS_PER_SEC << "s.\n\tGauss-Jordan elimination.\n";
				a = clock();

				finalMatrix = finalMatrix.gaussian();

				a = clock() - a;
				std::cout << "\t\tM["<<finalMatrix.rows()<<","<<finalMatrix.columns()<<"] @ " << ((double)a)/CLOCKS_PER_SEC << "s.\n\tDeleting null elements.\n";
				a = clock();

				unsigned deleted = 0;
				for (unsigned i = 0; i < finalMatrix.rows(); i++) {
					unsigned pivot = finalMatrix[i].pivot().index;
					if (pivot < finalMatrix.columns()) {
						bool null = finalMatrix[i].row.size() == 1;

						if (null) {
							_set.erase(_set.begin()+pivot-deleted);
							deleted++;
							for (unsigned k = 0; k < _set.size(); k++)
								for (unsigned j = 0; j < _set[k].expression.size(); j++)
									if (_set[k].expression[j].index > pivot)
										_set[k].expression[j].index--;
						} else {
							_set[pivot-deleted].expression.clear();
							for (typename MatrixRow<K>::iterator it = (finalMatrix[i].row.begin()+1); it != finalMatrix[i].row.end(); it++)
								_set[pivot-deleted].expression.push_back(Term<K>(-it->value, it->index-deleted));
						}
					}
				}
				a = clock() - a;
				std::cout << "\t\t" << deleted << " @" << ((double)a)/CLOCKS_PER_SEC << "s.\n";
			}

			totalTime = clock() - totalTime;
			std::cout << "Linear Algebra created.\n " << _set.size() << " elements @" << ((double)totalTime)/CLOCKS_PER_SEC << "s.\n";
		}

		std::vector<Term<K> > evaluate(Polynomial<K,T*> expression) {
			std::vector<Term<K> > ret;

			for (unsigned i = 0; i < expression.size(); i++) {
				unsigned index = _set.size();
				for (unsigned j = start(expression[i].terms.terms()); j < start(expression[i].terms.terms()+1); j++)
					if (_set[j].element == expression[i].terms) {
						index = j;
						break;
					}

				if (index < _set.size()) {
					if (_set[index].expression.size()) {
						std::vector<unsigned> notfound;

						for (unsigned j = 0; j < _set[index].expression.size(); j++) {
							unsigned k;
							for (k = 0; k < ret.size(); k++)
								if (ret[k].index == _set[index].expression[j].index) {
									ret[k].coefficient += expression[i].coefficient * _set[index].expression[j].coefficient;
									break;
								}
							if (k == ret.size())
								notfound.push_back(j);
						}
						for (unsigned j = 0; j < notfound.size(); j++)
							ret.push_back(Term<K>(expression[i].coefficient * _set[index].expression[notfound[j]].coefficient, _set[index].expression[notfound[j]].index));
					} else
						ret.push_back(Term<K>(expression[i].coefficient, index));
				}
			}

			return ret;
		}
};

template<class K, class T>
const unsigned Algebra<K,T>::_combinatory[20] = {1, 1, 2, 5, 14, 42, 132, 429, 1430, 4862, 16796, 58786, 208012, 742900, 2674440, 9694845, 35357670, 129644790, 477638700, 1767263190};

#endif