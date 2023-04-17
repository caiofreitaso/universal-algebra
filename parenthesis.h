#ifndef PARENTHESIS_H
#define PARENTHESIS_H

template<class T>
struct Parenthesis {
	union {
		struct {
			unsigned left;
			unsigned right;
		};
		T element;
	};
	bool isSingle;

	Parenthesis(Parenthesis const& c) : isSingle(c.isSingle) {
		if (isSingle)
			element = c.element;
		else {
			left = c.left;
			right = c.right;
		}
	}
	Parenthesis(unsigned left, unsigned right) : isSingle(false) {
		this->right = right;
		this->left = left;
	}
	Parenthesis(T element) : element(element), isSingle(true) { }

	Parenthesis& operator=(Parenthesis r) {
		isSingle = r.isSingle;
		if (isSingle)
			element = r.element;
		else {
			right = r.right;
			left = r.left;
		}
		return *this;
	}

	bool operator==(Parenthesis const& r) const {
		if (isSingle && r.isSingle)
			return element == r.element;
		else if (!isSingle && !r.isSingle) {
			return left == r.left && right == r.right;
		}
		return false;
	}
	bool operator!=(Parenthesis const& r) const { return !(*this == r); }
};

#endif