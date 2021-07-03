#ifndef SKELETON_TYPE_H
#define SKELETON_TYPE_H

/*
 * The recursive tree-style definition of tyes in SysY. Can be used in the
 * entries of symbol tables or in the leaf nodes of ASTs to mark the type.
 * 
 * The main class: TypePtr, a universal type pointer recording the type.
 *  + To get a TypePtr, use `make_void()', `make_int()', or other handy
 *    constructors.
 *  + To check what is in a TypePtr, use `is_void()', `is_int()' or many others.
 *  + Also support type checking such that `is_same_type' to check whether the
 *    two types are the same, or `can_accept' to check if one types accepts
 *    another type.
 *  + Simplely print them using std::cout or any other type of std::ostream.
 * 
 * Example:
 *     using namespace compiler_skeleton::sysy;
 *     TypePtr int_type = make_int();
 *     TypePtr const_int_type = make_int(true);
 *     std::cout << can_accept(int_type, const_int_type); // true
 *     TypePtr arr1 = make_arr(make_arr(make_int(), 3), 2);
 * 										// Build an array in a recursive way.
 *     std::vector<int> dim = {2, 3};
 *     TypePtr arr2 = make_arr(make_int(), dim.begin(), dim.end());
 * 										// Build an array by a dimension vector.
 *     std::cout << is_same_type(arr1, arr2); // true
 *     std::cout << arr1 << std::endl; // "int[2][3]"
 */

#include <iostream>
#include <memory>
#include <vector>
#include <variant>

namespace compiler_skeleton::sysy
{

// The base type of SysY types.
class TypeBase;

// All the types in SysY.
class VoidType;
class IntType;
class ArrType;
class PtrType;
class FuncType;

// The universal type & type pointer.
using Type = std::variant
<
	VoidType,
	IntType,
	ArrType,
	PtrType,
	FuncType
>;
using TypePtr = std::shared_ptr<Type>;
using TypePtrVec = std::vector<TypePtr>;

// Corresponding type pointers.
using VoidTypePtr = std::shared_ptr<VoidType>;
using IntTypePtr = std::shared_ptr<IntType>;
using ArrTypePtr = std::shared_ptr<ArrType>;
using PtrTypePtr = std::shared_ptr<PtrType>;
using FuncTypePtr = std::shared_ptr<FuncType>;

// Type query functions.
bool is_void(const TypePtr &type);
bool is_int(const TypePtr &type);
bool is_arr(const TypePtr &type);
bool is_ptr(const TypePtr &type);
bool is_func(const TypePtr &type);

// Actual type getters.
VoidType get_void(const TypePtr &type);
IntType get_int(const TypePtr &type);
ArrType get_arr(const TypePtr &type);
PtrType get_ptr(const TypePtr &type);
FuncType get_func(const TypePtr &type);

// Handy type constructors.
TypePtr make_void();
TypePtr make_int(bool is_const=false);
TypePtr make_arr(TypePtr ele_type, int len);
template<class Iter>
TypePtr make_arr(TypePtr base_type, Iter dim_begin, Iter dim_end)
{
	return std::make_shared<Type>(ArrType(base_type, dim_begin, dim_end));
}
TypePtr make_ptr(TypePtr base_type, bool is_const=false);
TypePtr make_func(TypePtr retval_type);
template<class Iter>
TypePtr make_func(TypePtr retval_type, Iter arg_types_begin, Iter arg_types_end)
{
	return std::make_shared<Type>(FuncType(retval_type, arg_types_begin, arg_types_end));
}

// Type info functions.
bool is_const_type(const TypePtr &type);
bool is_same_type(const TypePtr &type1, const TypePtr &type2);
bool can_accept(const TypePtr &req_type, const TypePtr &prov_type);
bool can_operate(const TypePtr &type1, const TypePtr &type2);
TypePtr common_type(const TypePtr &type1, const TypePtr &type2);
int size_of_type(const TypePtr &type);

class TypeBase
{
  public:
	virtual bool is_const() const;
	virtual int size() const;
};

class VoidType: public TypeBase
{
};

class IntType: public TypeBase
{
  protected:
	bool _is_const;

  public:
	IntType(bool is_const);

	// Getters.
	bool is_const() const override;
	int size() const override;

	// Setters.
	void set_is_const(bool is_const);
};

class ArrType: public TypeBase
{
  protected:
	int _len;
	TypePtr _ele_type;

  public:
	// Build an array type given the length and the type of its elements.
	// This function does no error checking, so make sure ele_type is a
	// valid int/array type.
	ArrType(TypePtr ele_type, int len);

	// Build an array type given the size of all dimension sizes and a base type.
	template<class Iter>
	ArrType(TypePtr base_type, Iter dim_begin, Iter dim_end)
	{
		// TODO
		// std::holds_alternative<IntType>(*base_type);
		_len = *dim_begin;
		++dim_begin;
		if(dim_begin != dim_end)
			_ele_type = make_arr(base_type, dim_begin, dim_end);
		else
			_ele_type = base_type;
	}

	// Getters.
	bool is_const() const override;
	int size() const override;
	int len() const;
	TypePtr element_type() const;
	int element_size() const;

	// Setters.
	void set_len(int len);
	void set_ele_type(TypePtr ele_type);
};

/*
 * Although there are no pointers in Sys-Y, we still define and use it (at least
 * for function arguments).
 */
class PtrType: public TypeBase
{
  protected:
	bool _is_const;
	TypePtr _base_type; // The type that the pointer points to.

  public:
	PtrType(TypePtr base_type, bool is_const=false);

	// Getters.
	bool is_const() const override;
	int size() const override;
	TypePtr base_type() const;

	// Setters.
	void set_is_const(bool is_const);
	void set_base_type(TypePtr base_type);
};

class FuncType: public TypeBase
{
  protected:
	TypePtr _retval_type;
	TypePtrVec _arg_types;

  public:
	FuncType(TypePtr retval_type);
	template<class Iter>
	FuncType(TypePtr retval_type, Iter arg_types_begin, Iter arg_types_end)
	  : _retval_type(retval_type),
		_arg_types(TypePtrVec(arg_types_begin, arg_types_end)) {}

	// Getters.
	TypePtr retval_type() const;
	int arg_cnt() const;
	const TypePtrVec &arg_types() const;
	TypePtr arg_type(int idx) const;

	// Setters.
	void set_retval_type(TypePtr retval_type) { _retval_type = retval_type; }
	template<class Iter>
	void set_args_types(Iter arg_types_begin, Iter arg_types_end)
		{ _arg_types = TypePtrVec(arg_types_begin, arg_types_end); }
	void set_arg_type(int idx, TypePtr arg_type);
};

// Prints the types to a stream.
class TypePrinter: public TypeBase
{
  protected:
	void _print_base_type(const ArrType &t);
	void _print_dim_size(const ArrType &t);
	std::ostream &_out;

  public:
	TypePrinter(std::ostream &out): _out(out) {}

	void operator() (const VoidType &t); // "void"
	void operator() (const IntType &t); // "int" or "const int"
	void operator() (const ArrType &t); // something like "int[2][3]"
	void operator() (const PtrType &t); // something like "int[][3]"
	void operator() (const FuncType &t); // something like "int (*)(int, int[])"
};

} // namespace compiler_skeleton

// use std::ostream to output TypePtr.
std::ostream &operator << (std::ostream &out, const compiler_skeleton::sysy::TypePtr &type);

#endif