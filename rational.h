#ifndef RATIONAL_H
#define RATIONAL_H

struct Rational
{
	bool negative;
	unsigned numerator;
	unsigned denominator;

	Rational() : negative(false),numerator(0),denominator(1) { }
	Rational(Rational const& r) : negative(r.negative),numerator(r.numerator),denominator(r.denominator) { }
	Rational(int x) : negative(x < 0),numerator(x < 0 ? -x : x),denominator(1) { }
	Rational(unsigned x, unsigned d = 1) : negative(false), numerator(x), denominator(d) { optimize(); }
	Rational(bool n, unsigned x, unsigned d = 1) : negative(n), numerator(x), denominator(d) { optimize(); }

	Rational& operator=(Rational r) {
		negative = r.negative;
		numerator = r.numerator;
		denominator = r.denominator;
		return *this;
	}

	Rational operator+(Rational r) const {
		unsigned a = numerator*r.denominator;
		unsigned b = r.numerator*denominator;
		if ((negative && r.negative) || (!negative && !r.negative)) {
			Rational ret(negative, a + b, denominator*r.denominator);
			ret.optimize();
			return ret;
		} else {
			if (a > b) {
				Rational ret(negative, a - b, denominator*r.denominator);
				ret.optimize();
				return ret;
			} else {
				Rational ret(r.negative, b - a, denominator*r.denominator);
				ret.optimize();
				return ret;
			}
		}
	}
	Rational operator-(Rational r) const {
		unsigned a = numerator*r.denominator;
		unsigned b = r.numerator*denominator;
		if ((negative && r.negative) || (!negative && !r.negative)) {
			if (a > b) {
				Rational ret(negative, a - b, denominator*r.denominator);
				ret.optimize();
				return ret;
			} else {
				Rational ret(!negative, b - a, denominator*r.denominator);
				ret.optimize();
				return ret;
			}
		} else {
			Rational ret(negative, a + b, denominator*r.denominator);
			ret.optimize();
			return ret;
		}
	}
	Rational operator*(Rational r) const {
		Rational ret((negative && !r.negative)||(!negative && r.negative), numerator*r.numerator, denominator*r.denominator);
		ret.optimize();
		return ret;
	}
	Rational operator/(Rational r) const {
		Rational ret((negative && !r.negative)||(!negative && r.negative), numerator*r.denominator, denominator*r.numerator);
		ret.optimize();
		return ret;
	}
	Rational operator-() { return Rational(!negative,numerator,denominator); }


	Rational& operator+=(Rational r) {
		unsigned a = numerator*r.denominator;
		unsigned b = r.numerator*denominator;
		if ((negative && r.negative) || (!negative && !r.negative))
			numerator = a + b;
		else {
			if (a > b)
				numerator = a - b;
			else {
				negative = !negative;
				numerator = b - a;
			}
		}

		denominator *= r.denominator;
		optimize();
		return *this;
	}
	Rational& operator-=(Rational r) {
		unsigned a = numerator*r.denominator;
		unsigned b = r.numerator*denominator;
		if ((negative && r.negative) || (!negative && !r.negative)) {
			if (a > b)
				numerator = a - b;
			else {
				negative = !negative;
				numerator = b - a;
			}
		}
		else
			numerator = a + b;

		denominator *= r.denominator;
		optimize();
		return *this;
	}
	Rational& operator*=(Rational r) {
		negative = (negative && !r.negative)||(!negative && r.negative);
		numerator *= r.numerator;
		denominator *= r.denominator;
		optimize();
		return *this;
	}
	Rational& operator/=(Rational r) {
		negative = (negative && !r.negative)||(!negative && r.negative);
		numerator *= r.denominator;
		denominator *= r.numerator;
		optimize();
		return *this;	
	}

	bool operator==(Rational r) const {
		return negative == r.negative && numerator == r.numerator && denominator == r.denominator;
	}
	bool operator!=(Rational r) const { return !(*this == r); }

	bool operator>(Rational r) const {
		if (r.negative && !negative)
			return true;
		if (!r.negative && negative)
			return false;
		if (r.numerator == 0)
			return numerator != r.numerator && !negative;

		Rational t = *this / r;
		return t.numerator > t.denominator;
	}
	bool operator<(Rational r) const {
		if (r.negative && !negative)
			return false;
		if (!r.negative && negative)
			return true;
		if (r.numerator == 0)
			return numerator != r.numerator && negative;
		
		Rational t = *this / r;
		return t.numerator < t.denominator;
	}
	bool operator>=(Rational r) const { return !(*this < r); }
	bool operator<=(Rational r) const { return !(*this > r); }

	private:
		void optimize() {
			if (numerator) {
				unsigned a, b;
				if (numerator > denominator) {
					a = numerator;
					b = denominator;
				} else {
					a = denominator;
					b = numerator;
				}

				unsigned c;
				while (a % b != 0) {
					c = a % b;
					a = b;
					b = c;
				}

				numerator /= b;
				denominator /= b;
			} else {
				negative = false;
				denominator = 1;
			}
		}
};

inline Rational abs(Rational r) { r.negative = false; return r; }

std::ostream& operator<<(std::ostream& stream, const Rational& r) {
	stream << (r.negative ? "-" : "");
	stream << r.numerator;
	if (r.denominator > 1) {
		stream << "/";
		stream << r.denominator;
	}
	return stream;
}

#endif