#ifndef TIGGER_H
#define TIGGER_H

#include <iostream>
#include <string>
#include <variant>
#include "variable_printer.h"
#include "eeyore.h"

namespace compiler_skeleton::tigger
{

// Basic elements in Tigger: labels and registers.

struct Label
{
	int id;
	Label(int _id): id(_id) {}
	Label(eeyore::Label label): id(label.id) {}
};

struct RegBase
{
	int id;
	RegBase(int _id): id(_id) {}
};
struct ZeroReg: public RegBase
{
	ZeroReg(int _id): RegBase(_id) {}
};
struct CalleeSavedReg: public RegBase
{
	CalleeSavedReg(int _id): RegBase(_id) {}
};
struct CallerSavedReg: public RegBase
{
	CallerSavedReg(int _id): RegBase(_id) {}
};
struct ArgReg: public RegBase
{
	ArgReg(int _id): RegBase(_id) {}
};

const ZeroReg ZERO_REG = {0};
const std::vector<CalleeSavedReg> ALL_CALLEE_SAVED_REG = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const std::vector<CallerSavedReg> ALL_CALLER_SAVED_REG = {0, 1, 2, 3, 4, 5, 6};
const std::vector<ArgReg> ALL_ARG_REG = {0, 1, 2, 3, 4, 5, 6, 7};

using Reg = std::variant<ZeroReg, CalleeSavedReg, CallerSavedReg, ArgReg>;

// Tigger statement definition.

using RegOrNum = std::variant<int, Reg>;

using UnaryOp = eeyore::UnaryOp;
using BinaryOp = eeyore::BinaryOp;

struct GlobalVar
{
	int id;

	GlobalVar(int _id): id(_id) {}
};
using GlobalVarOrNum = std::variant<int, GlobalVar>;

struct GlobalVarDeclStmt
{
	GlobalVar var;
	int initial_val;

	GlobalVarDeclStmt(GlobalVar _var, int _initial_val=0)
	  : var(_var), initial_val(_initial_val) {}
};

struct GlobalArrDeclStmt
{
	GlobalVar var;
	int size;

	GlobalArrDeclStmt(GlobalVar _var, int _size)
	  : var(_var), size(_size) {}
};

struct FuncHeaderStmt
{
	std::string func_name;
	int arg_cnt;
	int stack_size;

	FuncHeaderStmt(std::string _func_name, int _arg_cnt, int _stack_size=0)
	  : func_name(_func_name), arg_cnt(_arg_cnt), stack_size(_stack_size) {}
};

struct FuncEndStmt
{
	std::string func_name;

	FuncEndStmt(std::string _func_name): func_name(_func_name) {}
};

struct UnaryOpStmt
{
	UnaryOp op_type;
	Reg opr, opr1;

	UnaryOpStmt(Reg _opr, UnaryOp _op_type, Reg _opr1)
	  : op_type(_op_type), opr(_opr), opr1(_opr1) {}
};

struct BinaryOpStmt
{
	BinaryOp op_type;
	Reg opr, opr1;
	RegOrNum opr2; // operand 2 can be either a oprister or an int

	BinaryOpStmt(Reg _opr, Reg _opr1, BinaryOp _op_type, RegOrNum _opr2)
	  : op_type(_op_type), opr(_opr), opr1(_opr1), opr2(_opr2) {}
};

struct MoveStmt
{
	Reg opr;
	RegOrNum opr1;

	MoveStmt(Reg _opr, RegOrNum _opr1): opr(_opr), opr1(_opr1) {}
};

struct ReadArrStmt
{
	int idx;
	Reg opr, opr1;

	ReadArrStmt(Reg _opr, Reg _opr1, int _idx)
	  : idx(_idx), opr(_opr), opr1(_opr1) {}
};

struct WriteArrStmt
{
	int idx;
	Reg opr, opr1;

	WriteArrStmt(Reg _opr1, int _idx, Reg _opr)
	  : idx(_idx), opr(_opr), opr1(_opr1) {}
};

struct CondGotoStmt
{
	BinaryOp op_type;
	Reg opr1, opr2;
	Label goto_label;

	CondGotoStmt(Reg _opr1, BinaryOp _op_type, Reg _opr2, Label _goto_label)
	  : op_type(_op_type), opr1(_opr1), opr2(_opr2), goto_label(_goto_label) {}
};

struct GotoStmt
{
	Label goto_label;

	GotoStmt(Label _goto_label): goto_label(_goto_label) {}
};

struct LabelStmt
{
	Label label;

	LabelStmt(Label _label): label(_label) {}
};

struct FuncCallStmt
{
	std::string func_name;

	FuncCallStmt(std::string _func_name): func_name(_func_name) {}
};

struct ReturnStmt
{
};

struct StoreStmt
{
	int stack_offset;
	Reg opr;

	StoreStmt(int _stack_offset, Reg _opr)
	  : stack_offset(_stack_offset), opr(_opr) {}
};

struct LoadStmt
{
	GlobalVarOrNum src;
	Reg opr;

	LoadStmt(Reg _opr, GlobalVarOrNum _src): src(_src), opr(_opr) {}
};

struct LoadAddrStmt
{
	GlobalVarOrNum src;
	Reg opr;

	LoadAddrStmt(Reg _opr, GlobalVarOrNum _src): src(_src), opr(_opr) {}
};

using TiggerStatement = std::variant
<
	GlobalVarDeclStmt,
	GlobalArrDeclStmt,
	FuncHeaderStmt,
	FuncEndStmt,
	UnaryOpStmt,
	BinaryOpStmt,
	MoveStmt,
	ReadArrStmt,
	WriteArrStmt,
	CondGotoStmt,
	GotoStmt,
	LabelStmt,
	FuncCallStmt,
	ReturnStmt,
	StoreStmt,
	LoadStmt,
	LoadAddrStmt
>;

struct RegPrinter
{
	std::ostream &out;

	void operator() (const ZeroReg &reg);
	void operator() (const CalleeSavedReg &reg);
	void operator() (const CallerSavedReg &reg);
	void operator() (const ArgReg &reg);
};

class TiggerPrinter
{
  public:
	std::ostream &out;

	TiggerPrinter(std::ostream &_out): out(_out) {}

	void operator() (const GlobalVarDeclStmt &stmt);
	void operator() (const GlobalArrDeclStmt &stmt);
	void operator() (const FuncHeaderStmt &stmt);
	void operator() (const FuncEndStmt &stmt);
	void operator() (const UnaryOpStmt &stmt);
	void operator() (const BinaryOpStmt &stmt);
	void operator() (const MoveStmt &stmt);
	void operator() (const ReadArrStmt &stmt);
	void operator() (const WriteArrStmt &stmt);
	void operator() (const CondGotoStmt &stmt);
	void operator() (const GotoStmt &stmt);
	void operator() (const LabelStmt &stmt);
	void operator() (const FuncCallStmt &stmt);
	void operator() (const ReturnStmt &stmt);
	void operator() (const StoreStmt &stmt);
	void operator() (const LoadStmt &stmt);
	void operator() (const LoadAddrStmt &stmt);
};

} // namespace compiler_skeleton::tigger

std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::Label &label);
std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::Reg &reg);
std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::GlobalVar &global_var);
std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::TiggerStatement &stmt);
template<template<class...> class Container>
std::ostream &operator << (std::ostream &out, const Container<compiler_skeleton::tigger::TiggerStatement> &stmts)
{
	compiler_skeleton::tigger::TiggerPrinter printer{out};
	for(const auto &stmt : stmts)
		std::visit(printer, stmt);
	return out;
}

#endif