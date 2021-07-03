#include <cassert>
#include "lambda_visitor.h"
#include "sysy_type.h"


namespace compiler_skeleton::sysy
{

bool is_void(const TypePtr &type)
{
	return std::holds_alternative<VoidType>(*type);
}

bool is_int(const TypePtr &type)
{
	return std::holds_alternative<IntType>(*type);
}

bool is_arr(const TypePtr &type)
{
	return std::holds_alternative<ArrType>(*type);
}

bool is_ptr(const TypePtr &type)
{
	return std::holds_alternative<PtrType>(*type);
}

bool is_func(const TypePtr &type)
{
	return std::holds_alternative<FuncType>(*type);
}


VoidType get_void(const TypePtr &type)
{
	return std::get<VoidType>(*type);
}

IntType get_int(const TypePtr &type)
{
	return std::get<IntType>(*type);
}

ArrType get_arr(const TypePtr &type)
{
	return std::get<ArrType>(*type);
}

PtrType get_ptr(const TypePtr &type)
{
	return std::get<PtrType>(*type);
}

FuncType get_func(const TypePtr &type)
{
	return std::get<FuncType>(*type);
}



TypePtr make_void()
{
	static const auto VOID_T = std::make_shared<Type>(VoidType());
	return VOID_T;
}

TypePtr make_int(bool is_const)
{
	static const auto CONST_INT_T = std::make_shared<Type>(IntType(true));
	static const auto INT_T = std::make_shared<Type>(IntType(false));
	return is_const? CONST_INT_T : INT_T;
}

TypePtr make_arr(TypePtr ele_type, int len)
{
	return std::make_shared<Type>(ArrType(ele_type, len));
}

TypePtr make_ptr(TypePtr base_type, bool is_const)
{
	static const auto INT_PTR_T = std::make_shared<Type>(PtrType(base_type, false));
	if(!is_const && is_int(base_type))
		return INT_PTR_T;
	return std::make_shared<Type>(PtrType(base_type, is_const));
}

TypePtr make_func(TypePtr retval_type)
{
	return std::make_shared<Type>(FuncType(retval_type));
}


bool is_const_type(const TypePtr &type)
{
	static utils::LambdaVisitor const_checker =
	{
		[](const auto &any_type) { return any_type.is_const(); }
	};
	return std::visit(const_checker, *type);
}

bool is_same_type(const TypePtr &type1, const TypePtr &type2)
{
	static utils::LambdaVisitor same_type_checker =
	{
		[](const VoidType &void_type1, const VoidType &void_type2)
			{ return true; },
		[](const IntType &int_type1, const IntType &int_type2)
			{ return int_type1.is_const() == int_type2.is_const(); },
		[](const ArrType &arr_type1, const ArrType &arr_type2)
			{ return arr_type1.len() == arr_type2.len()
				&& is_same_type(arr_type1.element_type(), arr_type2.element_type()); },
		[](const PtrType &ptr_type1, const PtrType &ptr_type2)
			{ return is_same_type(ptr_type1.base_type(), ptr_type2.base_type()); },
		[](const FuncType &func_type1, const FuncType &func_type2)
		{
			if(func_type1.arg_cnt() != func_type2.arg_cnt()
				|| !is_same_type(func_type1.retval_type(), func_type2.retval_type()))
			{
				return false;
			}
			for(int i = 0, arg_cnt = func_type1.arg_cnt(); i < arg_cnt; i++)
				if(!is_same_type(func_type1.arg_type(i), func_type2.arg_type(i)))
					return false;
			return true;
		},
		[](const auto &type1, const auto &type2) { return false; }
	};
	return std::visit(same_type_checker, *type1, *type2);
}

// Check if a function argument type `req_type' can accept `prov_type' as its
// argument type.
bool can_accept(const TypePtr &req_type, const TypePtr &prov_type)
{
	static utils::LambdaVisitor can_accept_checker =
	{
		[](const VoidType &void_type1, const VoidType &void_type2)
			{ return true; },
		[](const IntType &int_type1, const IntType &int_type2)
			{ return !int_type1.is_const() || int_type2.is_const(); },
		[](const ArrType &arr_type1, const ArrType &arr_type2)
			{ return arr_type1.len() == arr_type2.len()
				&& can_accept(arr_type1.element_type(), arr_type2.element_type()); },
		[](const PtrType &ptr_type1, const PtrType &ptr_type2)
			{ return can_accept(ptr_type1.base_type(), ptr_type2.base_type()); },
		[](const PtrType &ptr_type, const ArrType &arr_type)
			{ return can_accept(ptr_type.base_type(), arr_type.element_type()); },
			// This is special, a pointer type can accept an array type.
		[](const FuncType &func_type1, const FuncType &func_type2)
		{
			// Same as `is_same' for FuncType and FuncType.
			if(func_type1.arg_cnt() != func_type2.arg_cnt()
				|| !is_same_type(func_type1.retval_type(), func_type2.retval_type()))
			{
				return false;
			}
			for(int i = 0, arg_cnt = func_type1.arg_cnt(); i < arg_cnt; i++)
				if(!is_same_type(func_type1.arg_type(i), func_type2.arg_type(i)))
					return false;
			return true;
		},
		[](const auto &type1, const auto &type2) { return false; }
	};
	return std::visit(can_accept_checker, *req_type, *prov_type);
}

bool can_operate(const TypePtr &type1, const TypePtr &type2)
{
	// only `int op int' is valid
	return is_int(type1) && is_int(type2);
}

TypePtr common_type(const TypePtr &type1, const TypePtr &type2)
{
	assert(is_int(type1) && is_int(type2));
	bool type1_is_const = std::get<IntType>(*type1).is_const(),
	     type2_is_const = std::get<IntType>(*type2).is_const();
	return make_int(type1_is_const && type2_is_const);
}

int size_of_type(const TypePtr &type)
{
	// As for size, assume the code is run in 32-bit enviroinment.
	static utils::LambdaVisitor size_getter =
	{
		[](const auto &any_type) { return any_type.size(); }
	};
	return std::visit(size_getter, *type);
}

bool TypeBase::is_const() const { return false; }
int TypeBase::size() const { return 0; }

IntType::IntType(bool is_const): _is_const(is_const) {}
bool IntType::is_const() const { return _is_const; }
int IntType::size() const  { return 4; }
void IntType::set_is_const(bool is_const) { _is_const = is_const; }

ArrType::ArrType(TypePtr ele_type, int len): _len(len), _ele_type(ele_type) {}
bool ArrType::is_const() const  { return is_const_type(_ele_type); }
int ArrType::size() const { return _len * size_of_type(_ele_type); }
int ArrType::len() const { return _len; }
TypePtr ArrType::element_type() const { return _ele_type; }
int ArrType::element_size() const { return size_of_type(_ele_type); }
void ArrType::set_len(int len) { _len = len; }
void ArrType::set_ele_type(TypePtr ele_type) { _ele_type = ele_type; }

PtrType::PtrType(TypePtr base_type, bool is_const)
  : _base_type(base_type), _is_const(is_const) {}
bool PtrType::is_const() const  { return _is_const; }
int PtrType::size() const { return 4; }
TypePtr PtrType::base_type() const { return _base_type; }
void PtrType::set_is_const(bool is_const) { _is_const = is_const; }
void PtrType::set_base_type(TypePtr base_type) { _base_type = base_type; }

FuncType::FuncType(TypePtr retval_type): _retval_type(retval_type) {}
TypePtr FuncType::retval_type() const { return _retval_type; }
int FuncType::arg_cnt() const { return _arg_types.size(); }
const TypePtrVec &FuncType::arg_types() const { return _arg_types; }
TypePtr FuncType::arg_type(int idx) const { return _arg_types[idx]; }
void FuncType::set_arg_type(int idx, TypePtr arg_type) { _arg_types[idx] = arg_type; }


void TypePrinter::_print_base_type(const ArrType &t)
{
	const ArrType *arr_ptr = &t;
	while(!is_int(arr_ptr->element_type()))
		arr_ptr = &std::get<ArrType>(*arr_ptr->element_type());
	std::visit(*this, *arr_ptr->element_type());
}

void TypePrinter::_print_dim_size(const ArrType &t)
{
	const ArrType *arr_ptr = &t;
	while(true)
	{
		_out << '[' << arr_ptr->len() << ']';
		if(is_int(arr_ptr->element_type()))
			break;
		arr_ptr = &std::get<ArrType>(*arr_ptr->element_type());
	}
}

void TypePrinter::operator() (const VoidType &t)
{
	_out << "void";
}

void TypePrinter::operator() (const IntType &t)
{
	if(t.is_const())
		_out << "const ";
	_out << "int";
}

void TypePrinter::operator() (const ArrType &t)
{
	_print_base_type(t);
	_print_dim_size(t);
}

void TypePrinter::operator() (const PtrType &t)
{
	if(is_int(t.base_type()))
	{
		std::visit(*this, *t.base_type());
		_out << "[]";
	}
	else if(is_arr(t.base_type()))
	{
		const auto &arr = std::get<ArrType>(*t.base_type());
		_print_base_type(arr);
		_out << "[]";
		_print_dim_size(arr);
	}
	else // Pointer of array or function types is not supported by default.
		 // Simply prints "pointer of #base_type".
	{
		_out << "pointer of ";
		std::visit(*this, *t.base_type());
	}
}

void TypePrinter::operator() (const FuncType &t)
{
	std::visit(*this, *t.retval_type());
	_out << "(*)(";
	for(int i = 0, arg_cnt = t.arg_cnt(); i < arg_cnt; i++)
	{
		if(i >= 1)
			_out << ", ";
		std::visit(*this, *t.arg_type(i));
	}
	_out << ')';
}

} // namespace compiler_skeleton::sysy

std::ostream &operator << (std::ostream &out, const compiler_skeleton::sysy::TypePtr &type)
{
	compiler_skeleton::sysy::TypePrinter printer(out);
	std::visit(printer, *type);
	return out;
}