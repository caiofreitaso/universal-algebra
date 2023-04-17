#include "algebra.h"
#include "rational.h"
#include <time.h>

/*struct Test : Element<std::string,std::string> {
	Test(std::string v) : Element<std::string,std::string>(v) { }
	Test operator*(Test t) const {
		Test ret("(");
		ret.value += value;
		ret.value += t.value;
		ret.value += ")";

		return ret;
	}
	Test operator*(std::string t) const {
		Test ret("(");
		ret.value += value;
		ret.value += t;
		ret.value += ")";

		return ret;
	}
	friend std::stringstream& operator<< (std::stringstream& out, Test const& t);
};

std::stringstream& operator<< (std::stringstream& out, Test const& t) {
	out << t.value;
	return out;
}*/

int main() {
	Monomial<Rational> a1, a2, b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12, s2;
	a1.terms = Product<unsigned>(0,1);
	s2.terms = a2.terms = Product<unsigned>(1,0);
	s2.coefficient = -1;

	b1.terms = Product<unsigned>(Product<unsigned>(Product<unsigned>(0,1),2),3);
	b2.terms = Product<unsigned>(Product<unsigned>(Product<unsigned>(0,3),2),1);
	b3.terms = Product<unsigned>(Product<unsigned>(Product<unsigned>(1,2),0),3);
	b4.terms = Product<unsigned>(Product<unsigned>(Product<unsigned>(3,2),0),1);
	b5.terms = Product<unsigned>(Product<unsigned>(Product<unsigned>(2,0),1),3);
	b6.terms = Product<unsigned>(Product<unsigned>(Product<unsigned>(2,0),3),1);

	b7.terms = Product<unsigned>(Product<unsigned>(0,3),Product<unsigned>(1,2));
	b8.terms = Product<unsigned>(Product<unsigned>(0,1),Product<unsigned>(3,2));
	b9.terms = Product<unsigned>(Product<unsigned>(1,Product<unsigned>(3,2)),0);
	b10.terms = Product<unsigned>(Product<unsigned>(3,Product<unsigned>(1,2)),0);
	b11.terms = Product<unsigned>(Product<unsigned>(1,2),Product<unsigned>(0,3));
	b12.terms = Product<unsigned>(Product<unsigned>(3,2),Product<unsigned>(0,1));
	b7.coefficient = b8.coefficient = b9.coefficient = b10.coefficient =
	b11.coefficient = b12.coefficient = -1;

	Identity<Rational> id, id1, id2;
	Relation<Rational,std::string*> r;
	id.monomials.push_back(a1);
	id.monomials.push_back(a2);
	id1.monomials.push_back(b1);
	id1.monomials.push_back(b2);
	id1.monomials.push_back(b3);
	id1.monomials.push_back(b4);
	id1.monomials.push_back(b5);
	id1.monomials.push_back(b6);
	id1.monomials.push_back(b7);
	id1.monomials.push_back(b8);
	id1.monomials.push_back(b9);
	id1.monomials.push_back(b10);
	id1.monomials.push_back(b11);
	id1.monomials.push_back(b12);

	id2.monomials.push_back(Product<unsigned>(0,Product<unsigned>(1,2)));
	id2.monomials.push_back(Product<unsigned>(1,Product<unsigned>(2,0)));
	id2.monomials.push_back(Product<unsigned>(2,Product<unsigned>(0,1)));

	Algebra<Rational,std::string> myal(7);
	myal.addGenerator("a",7);
	myal.addGenerator("b",7);
	myal.addGenerator("c",7);
	myal.addGenerator("d",7);
	myal.addGenerator("e",7);
	myal.addIdentity(id);
	myal.addIdentity(id1);
	
	r.monomials.push_back(Product<std::string*>(Product<std::string*>(&myal(0),&myal(1)),&myal(2)));
	r.monomials.push_back(Product<std::string*>(Product<std::string*>(&myal(1),&myal(2)),&myal(0)));
	r.monomials.push_back(Product<std::string*>(Product<std::string*>(&myal(2),&myal(0)),&myal(1)));
	r.monomials.push_back(Monomial<Rational,std::string*>(-1,Product<std::string*>(&myal(3),&myal(4))));
	myal.addRelation(r);
	
	std::cout << "IDENTITIES\n";
	for(unsigned k = 0; k < myal.identities(); k++) {
		for(unsigned i = 0; i < myal.identity(k).monomials.size(); i++) {
			if (i > 0 && !myal.identity(k).monomials[i].coefficient.negative)
				std::cout << "+ ";
			if (myal.identity(k).monomials[i].coefficient.numerator != 1)
				std::cout << myal.identity(k).monomials[i].coefficient;
			else if (myal.identity(k).monomials[i].coefficient.negative)
				std::cout << "-";
			std::cout << myal.identity(k).monomials[i].terms.toString('x') << " ";
		}
		std::cout << "= 0\n";
	}
	std::cout << "\nRELATIONS\n";
	for(unsigned k = 0; k < myal.relations(); k++) {
		for(unsigned i = 0; i < myal.relation(k).monomials.size(); i++) {
			if (i > 0 && !myal.relation(k).monomials[i].coefficient.negative)
				std::cout << "+ ";
			if (myal.relation(k).monomials[i].coefficient.numerator != 1)
				std::cout << myal.relation(k).monomials[i].coefficient;
			else if (myal.relation(k).monomials[i].coefficient.negative)
				std::cout << "- ";
			std::cout << myal.relation(k).monomials[i].terms.toString() << " ";
		}
		std::cout << "= 0\n";
	}

	std::cout << "\n--- ALGEBRA ---\n";

	clock_t clk = clock();
	myal.generate();
	clk = clock() - clk;

	std::cout << "TIME: " << ((double)clk)/CLOCKS_PER_SEC << "\n";

	for (unsigned i = 0; i < myal.size(); i++) {
		std::cout << "a_{" << i << "}:\t" << myal[i].element.toString();
		if (myal[i].expression.size()) {
			std::cout << " =";
			if (myal[i].expression[0].coefficient == 1)
				std::cout << " a_{" << myal[i].expression[0].index << "}";
			else if (myal[i].expression[0].coefficient == -1)
				std::cout << " -a_{" << myal[i].expression[0].index << "}";
			else
				std::cout << " " << myal[i].expression[0].coefficient << "a_{" << myal[i].expression[0].index << "}";
			for (unsigned j = 1; j < myal[i].expression.size(); j++)
				if (myal[i].expression[j].coefficient == 1)
					std::cout << " + a_{" << myal[i].expression[j].index << "}";
				else if (myal[i].expression[j].coefficient == -1)
					std::cout << " - a_{" << myal[i].expression[j].index << "}";
				else if (myal[i].expression[j].coefficient > 0)
					std::cout << " + " << myal[i].expression[j].coefficient << "a_{" << myal[i].expression[j].index << "}";
				else
					std::cout << " " << myal[i].expression[j].coefficient << "a_{" << myal[i].expression[j].index << "}";
		}
		std::cout << "\n";
	}

	Polynomial<Rational,std::string*> value;
	Product<std::string*> v0(&myal(3),&myal(4));
	Product<std::string*> v1(&myal(1),&myal(2));
	Product<std::string*> v2(v0,v1);
	Product<std::string*> v3(v0,&myal(2));
	Product<std::string*> v4(Product<std::string*>(&myal(4),&myal(2)),&myal(3));
	Product<std::string*> v5(Product<std::string*>(&myal(2),&myal(3)),&myal(4));
	//value.push_back(v2);
	value.push_back(Product<std::string*>(v2,v3));
	value.push_back(Product<std::string*>(v2,v4));
	value.push_back(Product<std::string*>(v2,v5));
	value.push_back(Monomial<Rational,std::string*>(3,Product<std::string*>(Product<std::string*>(v3,v1),v0)));
	value.push_back(Monomial<Rational,std::string*>(3,Product<std::string*>(Product<std::string*>(v4,v1),v0)));
	value.push_back(Monomial<Rational,std::string*>(3,Product<std::string*>(Product<std::string*>(v5,v1),v0)));

	/*Polynomial<Rational,std::string*> poly1;
	Monomial<Rational,std::string*> mono1,mono2;
	mono1.coefficient = 2;
	mono1.terms = Product<std::string*>(Product<std::string*>(new std::string("a"),new std::string("b")),new std::string("b"));
	mono2.coefficient = -3;
	mono2.terms = Product<std::string*>(new std::string("b"), Product<std::string*>(new std::string("a"),new std::string("c")));
	poly1.push_back(mono1);
	poly1.push_back(mono2);*/

	std::cout << "l_1:\t";
	if (value[0].coefficient == 1)
		std::cout << value[0].terms.toString() << " ";
	else if (value[0].coefficient == -1)
		std::cout << "-" << value[0].terms.toString() << " ";
	else
		std::cout << value[0].coefficient << value[0].terms.toString() << " ";
	for (unsigned i = 1; i < value.size(); i++)
		if (value[i].coefficient == 1)
			std::cout << "+ " << value[i].terms.toString() << " ";
		else if (value[i].coefficient == -1)
			std::cout << "- " << value[i].terms.toString() << " ";
		else if (value[i].coefficient > 1)
			std::cout << "+ " << value[i].coefficient << value[i].terms.toString() << " ";
		else
			std::cout << value[i].coefficient << value[i].terms.toString() << " ";
	std::cout << "\n";

	std::vector<Term<Rational> > eval = myal.evaluate(value);

	if (eval.size()) {
		std::cout << " =";
		if (eval[0].coefficient == 1)
			std::cout << " a_{" << eval[0].index << "}";
		else if (eval[0].coefficient == -1)
			std::cout << " -a_{" << eval[0].index << "}";
		else
			std::cout << " " << eval[0].coefficient << "a_{" << eval[0].index << "}";
		for (unsigned j = 1; j < eval.size(); j++)
			if (eval[j].coefficient == 1)
				std::cout << " + a_{" << eval[j].index << "}";
			else if (eval[j].coefficient == -1)
				std::cout << " - a_{" << eval[j].index << "}";
			else if (eval[j].coefficient > 0)
				std::cout << " + " << eval[j].coefficient << "a_{" << eval[j].index << "}";
			else
				std::cout << " " << eval[j].coefficient << "a_{" << eval[j].index << "}";
	}
	std::cout << "\n";


	/*std::string *a = new std::string("a"), *b = new std::string("b");
	Product<Pointer<std::string> > p1(new Product<Pointer<std::string> >(a),new Product<Pointer<std::string> >(b));
	Product<Pointer<std::string> > q1(new Product<Pointer<std::string> >(b),new Product<Pointer<std::string> >(b));
	Product<unsigned> p2(Product<unsigned>(0,1),2);
	Product<Pointer<std::string> >* re[] = { &p1, &q1, &q1 };

	Product<Pointer<std::string> >* final = replace(p2,re);
	std::cout << final->toString() << "\n";
	explode(final,re,3);*/

	return 0;
}