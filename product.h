#ifndef PRODUCT_H
#define PRODUCT_H

#include "parenthesis.h"
#include <vector>
#include <string>
#include <sstream>

template<class T>
struct Product;

template<class K, class U>
Product<U> replace(Product<K> const& base, Product<U>* values[], unsigned from = 0);

template<class T>
struct Product {
	Product() : _terms(0) { }
	Product(Product const& p) : _list(p._list),_terms(p._terms) { }
	Product(T e) : _terms(1) { _list.push_back(e); }
	Product(Product left, Product right) : _terms(right._terms + left._terms) {
		_list.reserve(right.size() + left.size() + 1);
		_list.push_back(Parenthesis<T>(1u, left.size()+1));
		_list.insert(_list.end(), left._list.begin(), left._list.end());
		for (unsigned i = 1; i < _list.size(); i++)
			if (!_list[i].isSingle) {
				_list[i].left += 1;
				_list[i].right += 1;
			}
		_list.insert(_list.end(), right._list.begin(), right._list.end());
		for (unsigned i = left.size()+1; i < _list.size(); i++)
			if (!_list[i].isSingle) {
				_list[i].left += left.size()+1;
				_list[i].right += left.size()+1;
			}
	}

	std::string toString(char variable = '\0') const { return getString(variable, 0); }
	std::string getString(char variable, unsigned i) const {
		std::stringstream str;
		if (_list[i].isSingle) {
			if (variable)
				str << variable << "_";
			str << _list[i].element;
		} else {
			str << "(";
			str << getString(variable, _list[i].left);
			str << ".";
			str << getString(variable, _list[i].right);
			str << ")";
		}
		return str.str();
	}

	unsigned size() const { return _list.size(); }
	unsigned terms() const { return _terms; }

	Parenthesis<T>& operator[](unsigned i) { return _list[i]; }
	Parenthesis<T> const& operator[](unsigned i) const { return _list[i]; }

	bool operator==(Product const& r) const {
		if (r.size() == _list.size()) {
			for (unsigned i = 0; i < _list.size(); i++)
				if (r[i] != _list[i])
					return false;
			return true;
		}
		return false;
	}

	bool operator!=(Product const& r) const { return !(*this == r); }


	std::vector<Parenthesis<T> > _list;
	unsigned _terms;
};

template<class T>
struct Product<T*> {
	Product() : _terms(0) { }
	Product(Product const& p) : _list(p._list),_terms(p._terms) { }
	Product(T* e) : _terms(1) { _list.push_back(e); }
	Product(Product left, Product right) : _terms(right._terms + left._terms) {
		_list.reserve(right.size() + left.size() + 1);
		_list.push_back(Parenthesis<T*>(1u, left.size()+1));
		_list.insert(_list.end(), left._list.begin(), left._list.end());
		for (unsigned i = 1; i < _list.size(); i++)
			if (!_list[i].isSingle) {
				_list[i].left += 1;
				_list[i].right += 1;
			}
		_list.insert(_list.end(), right._list.begin(), right._list.end());
		for (unsigned i = left.size()+1; i < _list.size(); i++)
			if (!_list[i].isSingle) {
				_list[i].left += left.size()+1;
				_list[i].right += left.size()+1;
			}
	}

	std::string toString(char variable = '\0') const { return getString(variable, 0); }
	std::string getString(char variable, unsigned i) const {
		std::stringstream str;
		if (_list[i].isSingle) {
			if (variable)
				str << variable << "_";
			str << *_list[i].element;
		} else {
			str << "(";
			str << getString(variable, _list[i].left);
			str << getString(variable, _list[i].right);
			str << ")";
		}
		return str.str();
	}

	unsigned size() const { return _list.size(); }
	unsigned terms() const { return _terms; }

	Parenthesis<T*>& operator[](unsigned i) { return _list[i]; }
	Parenthesis<T*> const& operator[](unsigned i) const { return _list[i]; }

	bool operator==(Product const& r) const {
		if (r.size() == _list.size()) {
			for (unsigned i = 0; i < _list.size(); i++)
				if (r[i] != _list[i])
					return false;
			return true;
		}
		return false;
	}

	bool operator!=(Product const& r) const { return !(*this == r); }


	std::vector<Parenthesis<T*> > _list;
	unsigned _terms;
};

template<class T>
struct Pointer {
	union {
		T* value;
		Product<Pointer<T> >* tree;
	};
	bool isTree;

	//Pointer(T* value) : value(value), isTree(false) { }
	//Pointer(Product<Pointer<T> >* t) : tree(t), isTree(true) { }

	bool operator== (Pointer p) const {
		if (isTree) {
			if (p.isTree)
				return *tree == *p.tree;
			else
				return false;
		} else {
			if (!p.isTree)
				return value == p.value;
			else
				return false;
		}
	}
	bool operator!= (Pointer p) const { return !((*this) == p); }
};

template<class T>
struct Product<Pointer<T> > {
	Product() { }
	Product(Product const& p) : _list(p._list), _size(p._size), _terms(p._terms) { }
	Product(T* e) : _size(1), _terms(1) {
		_list.push_back(Parenthesis<Pointer<T> >(Pointer<T>()));
		_list[0].element.value = e;
		_list[0].element.isTree = false;
	}
	Product(Product* left, Product* right) :
		_size((left?left->_size:0) + (right?right->_size:0) + 1),
		_terms((left?left->_terms:0) + (right?right->_terms:0)) {
		_list.reserve(3);
		_list.push_back(Parenthesis<Pointer<T> >(1u, 2u));
		_list.push_back(Parenthesis<Pointer<T> >(Pointer<T>()));
		_list[1].element.tree = left;
		_list[1].element.isTree = true;
		_list.push_back(Parenthesis<Pointer<T> >(Pointer<T>()));
		_list[2].element.tree = right;
		_list[2].element.isTree = true;
	}

	std::string toString() const { return getString(0); }
	std::string getString(unsigned i) const {
		std::stringstream str;
		if (_list[i].isSingle) {
			if (_list[i].element.isTree)
				str << _list[i].element.tree->toString();
			else
				str << *_list[i].element.value;
		} else {
			str << "(";
			str << getString(_list[i].left);
			str << getString(_list[i].right);
			str << ")";
		}
		return str.str();
	}

	unsigned size() const { return _list.size(); }
	unsigned realSize() const { return _size; }
	unsigned terms() const { return _terms; }

	Parenthesis<Pointer<T> >& operator[](unsigned i) { return _list[i]; }
	Parenthesis<Pointer<T> > const& operator[](unsigned i) const { return _list[i]; }

	bool operator==(Product const& r) const {
		if (r.realSize() == _size) {
			for (unsigned i = 0; i < _list.size(); i++)
				if (r[i] != _list[i])
					return false;
			return true;
		}
		return false;
	}
	bool operator!=(Product const& r) const { return !(*this == r); }
	
	std::vector<Parenthesis<Pointer<T> > > _list;
	unsigned _size;
	unsigned _terms;
};

template<class T>
struct stacknode {
	unsigned index;
	Pointer<T> next;
	stacknode(unsigned i, Pointer<T> p) :index(i),next(p) { }
};
template<class T>
bool operator==(Product<T*> d, Product<Pointer<T> > e) {
	if (d[0].isSingle && e[0].isSingle)
		return d[0].element == e[0].element.value;
	else {
		std::vector<stacknode<T> > stack;

		Pointer<T> p;
		p.isTree = true;
		p.tree = &e;

		stack.push_back(stacknode<T>(0,p));

		while(stack.size()) {
			if (stack.back().next.isTree) {
				if (d[stack.back().index].isSingle) {
					if(d[stack.back().index].element != (*stack.back().next.tree)[0].element.value)
						return false;
					else
						stack.pop_back();
				} else {
					Product<Pointer<T> >* pointer = stack.back().next.tree;
					unsigned left = d[stack.back().index].left;
					unsigned right = d[stack.back().index].right;

					p.tree = (*pointer)[1].element.tree;
					p.isTree = (*pointer)[1].element.isTree;

					stack.pop_back();
					stack.push_back(stacknode<T>(left,p));
					
					p.tree = (*pointer)[2].element.tree;
					p.isTree = (*pointer)[2].element.isTree;

					stack.push_back(stacknode<T>(right,p));
				}
			} else
				return false;
		}
		return true;
	}
}

template<class T>
bool operator==(Product<Pointer<T> > e, Product<T*> d) { return d == e; }

template<class K, class U>
Product<U> replace(Product<K> const& base, Product<U>* values[], unsigned from) {
	if (base[from].isSingle)
		return *values[base[from].element];
	return Product<U>(replace(base,values,base[from].left),replace(base,values,base[from].right));
}

template<class K, class U>
Product<U> replace(Product<K> const& base, Product<U> const* values[], unsigned from) {
	if (base[from].isSingle)
		return *values[base[from].element];
	return Product<U>(replace(base,values,base[from].left),replace(base,values,base[from].right));
}

template<class K, class U>
Product<Pointer<U> >* replace(Product<K> const& base, Product<Pointer<U> >* values[]) {
	std::vector<Product<Pointer<U> >*> stack;
	
	Product<Pointer<U> >* pointer = new Product<Pointer<U> >(0,0);

	if (base[0].isSingle)
		return values[base[0].element];
	else
		stack.push_back(pointer);

	for (unsigned i = 1; i < base.size(); i++) {
		if (base[i].isSingle) {
			unsigned last = stack.size()-1;
			unsigned position = (*stack[last])[1].element.tree ? 2 : 1;
			(*stack[last])[position].element.tree = values[base[i].element];
			(*stack[last])._terms += values[base[i].element]->_terms;
			(*stack[last])._size += values[base[i].element]->_size;
			if (position == 2)
				stack.erase(stack.begin()+last);
		} else {
			stack.push_back(new Product<Pointer<U> >(0,0));
			unsigned last = stack.size()-2;
			unsigned position = (*stack[last])[1].element.tree ? 2 : 1;
			(*stack[last])[position].element.tree = stack.back();
			(*stack[last])._terms += stack.back()->_terms;
			(*stack[last])._size += stack.back()->_size;
			if (position == 2)
				stack.erase(stack.begin()+last);
		}
	}

	return pointer;
}

template<class U>
void explode(Product<Pointer<U> >* p, Product<Pointer<U> >* val[], unsigned size) {
	for (unsigned i = 0; i < size; i++)
		if (val[i] == p)
			return;

	explode((*p)[1].element.tree,val,size);
	explode((*p)[2].element.tree,val,size);

	delete p;
}
#endif