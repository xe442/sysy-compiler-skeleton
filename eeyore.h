#ifndef SKELETON_EEYORE_H
#define SKELETON_EEYORE_H

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace compiler_skeleton::eeyore
{

// The basic component of an Eeyore statement: Label, T-, t- and p-type variable.
struct Label
{
	int id;
	Label(int _id): id(_id) {}
};
struct VarBase
{
	int id;
	VarBase(int _id): id(_id) {}
};
struct OrigVar: public VarBase
{
	int size;
	OrigVar(int _id, int _size=sizeof(int)): VarBase(_id), size(_size) {}
	bool operator == (const OrigVar &other) const { return id == other.id; }
};
struct TempVar: public VarBase
{
	TempVar(int _id): VarBase(_id) {}
	bool operator == (const TempVar &other) const { return id == other.id; }
};
struct Param: public VarBase
{
	Param(int _id): VarBase(_id) {}
	bool operator == (const Param &other) const { return id == other.id; }
};
using Operand = std::variant<int, OrigVar, TempVar, Param>;

// Eeyore statement operators.

enum class UnaryOp
{
	NEG, NOT
};
enum class BinaryOp
{
	ADD, SUB, MUL, DIV, MOD, OR, AND, GT, LT, GE, LE, EQ, NE
};

// Eeyore statements.

struct DeclStmt
{
	Operand var;
	
	DeclStmt(Operand _var): var(_var) {}
};

struct FuncDefStmt
{
	std::string func_name;
	int arg_cnt;

	FuncDefStmt(std::string _func_name, int _arg_cnt)
	  : func_name("f_" + _func_name), arg_cnt(_arg_cnt) {}
};

struct EndFuncDefStmt
{
	std::string func_name;

	EndFuncDefStmt(std::string _func_name)
	  : func_name("f_" + _func_name) {}
};

struct ParamStmt
{
	Operand param;

	ParamStmt(Operand _param): param(_param) {}
};

struct FuncCallStmt
{
	std::string func_name;
	std::optional<Operand> retval_receiver;

	FuncCallStmt(std::string _func_name)
	  : func_name("f_" + _func_name), retval_receiver(std::nullopt)
	{}
	FuncCallStmt(std::string _func_name, Operand _retval_receiver)
	  : func_name("f_" + _func_name), retval_receiver(_retval_receiver)
	{}
};

struct RetStmt
{
	std::optional<Operand> retval;

	RetStmt(): retval(std::nullopt) {}
	RetStmt(Operand _retval): retval(_retval) {}
};

struct GotoStmt
{
	Label goto_label;

	GotoStmt(Label _goto_label): goto_label(_goto_label) {}
};

struct CondGotoStmt
{
	// if opr1 op opr2 goto goto_label
	BinaryOp op;
	Operand opr1, opr2;
	Label goto_label;

	CondGotoStmt(Operand _opr1, BinaryOp _op, Operand _opr2, Label _goto_label)
	  : op(_op), opr1(_opr1), opr2(_opr2), goto_label(_goto_label) {}
};

struct UnaryOpStmt
{
	// opr = op_type opr1
	UnaryOp op_type;
	Operand opr, opr1;

	UnaryOpStmt(Operand _opr, UnaryOp _op_type, Operand _opr1)
	  : op_type(_op_type), opr(_opr), opr1(_opr1) {}
};
struct BinaryOpStmt
{
	// opr = opr1 op_type opr2
	BinaryOp op_type;
	Operand opr, opr1, opr2;

	BinaryOpStmt(Operand _opr, Operand _opr1, BinaryOp _op_type, Operand _opr2)
	  : op_type(_op_type), opr(_opr), opr1(_opr1), opr2(_opr2) {}
};

struct MoveStmt
{
	// opr = opr1
	Operand opr, opr1;

	MoveStmt(Operand _opr, Operand _opr1)
	  : opr(_opr), opr1(_opr1) {}
};

struct ReadArrStmt
{
	// opr = arr_opr[idx_opr]
	Operand opr, arr_opr, idx_opr;

	ReadArrStmt(Operand _opr, Operand _arr_opr, Operand _idx_opr)
	  : opr(_opr), arr_opr(_arr_opr), idx_opr(_idx_opr) {}
};
struct WriteArrStmt
{
	// arr_opr[idx_opr] = opr
	Operand opr, arr_opr, idx_opr;

	WriteArrStmt(Operand _arr_opr, Operand _idx_opr, Operand _opr)
	  : opr(_opr), arr_opr(_arr_opr), idx_opr(_idx_opr) {}
};

struct LabelStmt
{
	Label label;

	LabelStmt(Label _label): label(_label) {}
};

using EeyoreStatement = std::variant
<
	DeclStmt,
	FuncDefStmt,
	EndFuncDefStmt,
	ParamStmt,
	FuncCallStmt,
	RetStmt,
	GotoStmt,
	CondGotoStmt,
	UnaryOpStmt,
	BinaryOpStmt,
	MoveStmt,
	ReadArrStmt,
	WriteArrStmt,
	LabelStmt
>;

std::vector<Operand> used_vars(const EeyoreStatement &stmt);
std::vector<Operand> defined_vars(const EeyoreStatement &stmt);


// Printer classes

class OprPrinter
{
  protected:
	std::ostream &_out;

  public:
	OprPrinter(std::ostream &out): _out(out) {}

	void operator() (const OrigVar &var);
	void operator() (const TempVar &var);
	void operator() (const Param &var);
	void operator() (const int &var);
};

class EeyorePrinter
{
  protected:
	std::ostream &_out;
	bool _indent;

	inline void _print_indent() { if(_indent) _out << "  "; }

  public:
	EeyorePrinter(std::ostream &out): _out(out), _indent(false) {}

	void operator() (const DeclStmt &stmt);
	void operator() (const FuncDefStmt &stmt);
	void operator() (const EndFuncDefStmt &stmt);
	void operator() (const ParamStmt &stmt);
	void operator() (const FuncCallStmt &stmt);
	void operator() (const RetStmt &stmt);
	void operator() (const GotoStmt &stmt);
	void operator() (const CondGotoStmt &stmt);
	void operator() (const UnaryOpStmt &stmt);
	void operator() (const BinaryOpStmt &stmt);
	void operator() (const MoveStmt &stmt);
	void operator() (const ReadArrStmt &stmt);
	void operator() (const WriteArrStmt &stmt);
	void operator() (const LabelStmt &stmt);
};

} // namespace namespace compiler_skeleton::eeyore

std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::UnaryOp &op);
std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::BinaryOp &op);
std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::Operand &opr);
std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::EeyoreStatement &stmt);

// Printer of container (e.g. vector, list ...) of EeyoreStatements. Since
// indents are kept in the printer, you may want to use this method instead of
// printing statements one by one.
template<template<class...> class Container>
std::ostream &operator << (std::ostream &out, const Container<compiler_skeleton::eeyore::EeyoreStatement> &stmts)
{
	compiler_skeleton::eeyore::EeyorePrinter printer{out};
	for(const auto &stmt : stmts)
		std::visit(printer, stmt);
	return out;
}

#endif