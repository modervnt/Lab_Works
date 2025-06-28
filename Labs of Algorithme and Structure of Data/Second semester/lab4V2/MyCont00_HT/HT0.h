#pragma once
#include <bits/stdc++.h>
using namespace std;


class HT;

struct Node {	
	int key;	
	Node *down;
	Node() : down(nullptr) {}
	Node(int k, Node* d = nullptr) : key(k), down(d) {}
	~Node( ) { delete down; } 
};

//template<class Container = HT>
//struct myiter : public std::iterator<std::forward_iterator_tag, int> //Obsolete depuis c++17	
struct myiter
{
	using iterator_category = std::forward_iterator_tag;
	using value_type = int;
	using difference_type = std::ptrdiff_t;
	using pointer = int*;
	using reference = int&;
	myiter(Node *p) : bct(nullptr), pos(0), Ptr(p) {}
	bool operator == (const myiter & Other) const { return Ptr == Other.Ptr; }
	bool operator != (const myiter & Other) const { return Ptr != Other.Ptr; }
	//Pre-increment
	myiter operator++();
	//Post-increment
	myiter operator++(int) { myiter temp(*this); ++*this; return temp; }
	pointer operator->() { return & Ptr->key; }
	reference operator*() { return Ptr->key; }
	//protected:
	//Container& c; 
	Node **bct;
	size_t pos;
	Node * Ptr;
};

template <typename Container, typename Iter = myiter>
//class outiter : public std::iterator<std::output_iterator_tag, typename Container::value_type>
class outiter
{
protected:
	Container& container;    
	Iter iter;	
public:

	//Types requis
	using itrerator_category = std::output_iterator_tag;
	using value_type = void;
	using difference_type = void;
	using pointer = void;
	using reference = void;
	
	explicit outiter(Container& c, Iter it) : container(c), iter(it) { }

	const outiter<Container>&
		operator = (const typename Container::value_type& value) {
		iter = container.insert(value, iter).first;
		return *this;
	}
	const outiter<Container>& 
		operator = (const outiter<Container>&) { return *this; }
	
	outiter<Container>& operator* () { return *this; }
	
	outiter<Container>& operator++ () { return *this; }
	outiter<Container>& operator++ (int) { return *this; }
};

// Helper fonction pour cree un iterateur
template <typename Container, typename Iter>
inline outiter<Container, Iter> outinserter(Container& c, Iter it)
{
	return outiter<Container, Iter>(c, it);
}
//myiter myiter0(nullptr);

class HT {	
static  size_t tags;
    char tag;
	Node **bucket;
	size_t count = 0;
	static myiter myiter0;

	//Adding of sequence:
	vector<Node*> seq;
public: 
static const size_t Buckets = 32;
	using key_type = int;
	using value_type = int;
	using key_compare = equal_to<int> ;
	void swap(HT & rgt)
	{
		std::swap(tag, rgt.tag);
		std::swap(bucket, rgt.bucket);
		std::swap(count, rgt.count);
		std::swap(seq, rgt.seq);
	}
	int hash(int k)const { return (k*(Buckets - 1) + 7) % Buckets; }
	size_t bucket_count() { return Buckets; }
	myiter Insert(const int& k,myiter where) { return insert(k, where).first; }
	void Display();
	myiter begin()const;
	myiter end()const { return myiter(nullptr); }
//	const myiter cbegin() const { return begin(); }
//	const myiter cend()const { return cend(); }
	pair<myiter, bool> insert(int, myiter = myiter(nullptr));
	pair<myiter, bool> Remove(int);
	HT() :  tag('A' + tags++), bucket(new Node*[Buckets]){
		for (int i = 0; i < Buckets; ++i) bucket[i] = nullptr;
	}
	size_t power() const {
		size_t count = 0; 
		for (size_t i = 0; i < Buckets; ++i)
		{
			Node* element = bucket[i];
			while (element)
			{
				count++;
				element = element->down;
			}
		}
		return count;		
	}
//	HT(size_t Buckets);
//	void resize(size_t Buckets);
	int size() { return count; }
	template<typename MyIt>
	  HT(MyIt, MyIt);
	~HT();
	myiter find(int)const;
	HT(const HT&rgt) : HT() {
		for (auto x = rgt.begin(); x != rgt.end(); ++x) insert(*x);
	}
	HT(HT&& rgt) : HT() {
		swap(rgt);
	}
	HT& operator=(const HT & rgt)
	{
		HT temp;
		for (auto x = rgt.begin(); x != rgt.end(); ++x) insert(*x);
		swap(temp);
		return *this;
	}
	HT& operator=(HT && rgt)
	{
		swap(rgt);
		return *this;
	}
	HT& operator |= (const HT &);
	HT operator | (const HT & rgt) const
	{
		HT result(*this); return (result |= rgt);
	}
	HT& operator &= (const HT &);
	HT operator & (const HT & rgt) const
	{
		HT result(*this); return (result &= rgt);
	}
	HT& operator -= (const HT &);
	HT operator - (const HT & rgt) const
	{
		HT result(*this); return (result -= rgt);
	}
	HT& operator ^= (const HT& rgt);
	HT operator ^ (const HT & rgt) const 
	{
		HT result(*this); return (result ^= rgt);
	}
};
void generator(std::vector<int> &arr, int min, int max, int SIZE);