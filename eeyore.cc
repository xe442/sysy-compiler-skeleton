#include "lambda_visitor.h"
#include "eeyore.h"

using std::endl;

namespace
{

std::vector<compiler_skeleton::eeyore::Operand> &
remove_int_from_vec(std::vector<compiler_skeleton::eeyore::Operand> &oprs)
{
	for(auto iter = oprs.begin(); iter != oprs.end(); )
	{
		if(std::holds_alternative<int>(*iter))
			iter = oprs.erase(iter);
		else
			++iter;
	}
	return oprs;
}

} // namespace

namespace compiler_skeleton::eeyore
{

std::vector<Operand> used_vars(const EeyoreStatement &stmt)
{
	using OperandVec = std::vector<Operand>;
	static utils::LambdaVisitor used_opr_getter =
	{
		[](const ParamStmt &stmt) { return OperandVec{stmt.param}; },
		[](const RetStmt &stmt)
		{
			const auto &retval = stmt.retval;
			return retval.has_value()? OperandVec{retval.value()} : OperandVec{};
		},
		[](const CondGotoStmt &stmt) { return OperandVec{stmt.opr1, stmt.opr2}; },
		[](const UnaryOpStmt &stmt) { return OperandVec{stmt.opr1}; },
		[](const BinaryOpStmt &stmt) { return OperandVec{stmt.opr1, stmt.opr2}; },
		[](const MoveStmt &stmt) { return OperandVec{stmt.opr1}; },
		[](const ReadArrStmt &stmt) { return OperandVec{stmt.arr_opr, stmt.idx_opr}; },
		[](const WriteArrStmt &stmt) { return OperandVec{stmt.opr, stmt.arr_opr, stmt.idx_opr}; },
		[](const FuncCallStmt &stmt)
		{
			// This is intended. A function use all the variable in its body,
			// but we do not know what is actually used here in this function.
			// You should add the variables used after calling `used_vars', or
			// implement this case by passing some other arguments to `used_vars'.
			return OperandVec{};
		},
		[](const auto &stmt) { return OperandVec{}; }
	};

	OperandVec used_oprs = std::visit(used_opr_getter, stmt); 
	return remove_int_from_vec(used_oprs);
}

std::vector<Operand> defined_vars(const EeyoreStatement &stmt)
{
	using OperandVec = std::vector<Operand>;
	static utils::LambdaVisitor defined_opr_getter =
	{
		[](const DeclStmt &stmt) { return OperandVec{stmt.var}; },
		[](const UnaryOpStmt &stmt) { return OperandVec{stmt.opr}; },
		[](const BinaryOpStmt &stmt) { return OperandVec{stmt.opr}; },
		[](const MoveStmt &stmt) { return OperandVec{stmt.opr}; },
		[](const FuncCallStmt &stmt)
		{
			// This is intended. See the comment in the case of FuncCallStmt
			// in function `used_vars'.
			return OperandVec{};
		},
		[](const auto &stmt) { return OperandVec{}; }
	};

	OperandVec defined_oprs = std::visit(defined_opr_getter, stmt);
	return remove_int_from_vec(defined_oprs);
}

void OprPrinter::operator() (const OrigVar &var)
{
	_out << 'T' << var.id;
}

void OprPrinter::operator() (const TempVar &var)
{
	_out << 't' << var.id;
}

void OprPrinter::operator() (const Param &var)
{
	_out << 'p' << var.id;
}

void OprPrinter::operator() (const int &var)
{
	_out << var;
}

void EeyorePrinter::operator() (const DeclStmt &stmt)
{
	_print_indent();
	_out << "var ";
	// for original variable, it may have a size
	if(std::holds_alternative<OrigVar>(stmt.var))
	{
		OrigVar var = std::get<OrigVar>(stmt.var);
		if(var.size != 4)
			_out << var.size << ' ';
	}
	_out << stmt.var << endl;
}

void EeyorePrinter::operator() (const FuncDefStmt &stmt)
{
	_out << stmt.func_name << " [" << stmt.arg_cnt << ']' << endl;
	_indent = true;
}

void EeyorePrinter::operator() (const EndFuncDefStmt &stmt)
{
	_out << "end " << stmt.func_name << endl;
	_indent = false;
}

void EeyorePrinter::operator() (const ParamStmt &stmt)
{
	_print_indent();
	_out << "param " << stmt.param << endl;
}

void EeyorePrinter::operator() (const FuncCallStmt &stmt)
{
	_print_indent();
	if(stmt.retval_receiver.has_value())
		_out << stmt.retval_receiver.value() << " = ";
	_out << "call " << stmt.func_name << endl;
}

void EeyorePrinter::operator() (const RetStmt &stmt)
{
	_print_indent();
	_out << "return";
	if(stmt.retval.has_value())
		_out << ' ' << stmt.retval.value();
	_out << endl;
}

void EeyorePrinter::operator() (const GotoStmt &stmt)
{
	_print_indent();
	_out << "goto l" << stmt.goto_label.id << endl;
}

void EeyorePrinter::operator() (const CondGotoStmt &stmt)
{
	_print_indent();
	_out << "if " << stmt.opr1 << ' ' << stmt.op << ' ' << stmt.opr2
		<< " goto l" << stmt.goto_label.id << endl;
}

void EeyorePrinter::operator() (const UnaryOpStmt &stmt)
{
	_print_indent();
	_out << stmt.opr << " = " << stmt.op_type << stmt.opr1 << endl;
}

void EeyorePrinter::operator() (const BinaryOpStmt &stmt)
{
	_print_indent();
	_out << stmt.opr << " = "
		<< stmt.opr1 << ' ' << stmt.op_type << ' ' << stmt.opr2 << endl;
}

void EeyorePrinter::operator() (const MoveStmt &stmt)
{
	_print_indent();
	_out << stmt.opr << " = " << stmt.opr1 << endl;
}

void EeyorePrinter::operator() (const ReadArrStmt &stmt)
{
	_print_indent();
	_out << stmt.opr << " = " << stmt.arr_opr << '[' << stmt.idx_opr << ']' << endl;
}

void EeyorePrinter::operator() (const WriteArrStmt &stmt)
{
	_print_indent();
	_out << stmt.arr_opr << '[' << stmt.idx_opr << ']' << " = " << stmt.opr << endl;
}

void EeyorePrinter::operator() (const LabelStmt &stmt)
{
	_out << 'l' << stmt.label.id << ':' << endl;
}

} // namespace compiler_skeleton::eeyore

std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::UnaryOp &op)
{
	using UnaryOp = compiler_skeleton::eeyore::UnaryOp;
	switch(op)
	{
		case UnaryOp::NEG: out << '-'; break;
		case UnaryOp::NOT: out << '!'; break;
	}
	return out;
}

std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::BinaryOp &op)
{
	using BinaryOp = compiler_skeleton::eeyore::BinaryOp;
	switch(op)
	{
		case BinaryOp::ADD: out << '+'; break;
		case BinaryOp::SUB: out << '-'; break;
		case BinaryOp::MUL: out << '*'; break;
		case BinaryOp::DIV: out << '/'; break;
		case BinaryOp::MOD: out << '%'; break;
		case BinaryOp::OR: out << '|'; break;
		case BinaryOp::AND: out << '&'; break;
		case BinaryOp::GT: out << '>'; break;
		case BinaryOp::LT: out << '<'; break;
		case BinaryOp::GE: out << ">="; break;
		case BinaryOp::LE: out << "<="; break;
		case BinaryOp::EQ: out << "=="; break;
		case BinaryOp::NE: out << "!="; break;
	}
	return out;
}

std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::Operand &opr)
{
	compiler_skeleton::eeyore::OprPrinter printer{out};
	std::visit(printer, opr);
	return out;
}

std::ostream &operator << (std::ostream &out, const compiler_skeleton::eeyore::EeyoreStatement &stmt)
{
	compiler_skeleton::eeyore::EeyorePrinter printer{out};
	std::visit(printer, stmt);
	return out;
}

/*

Test case (also serve as an example):

int main()
{
	using namespace compiler_skeleton::eeyore;
	std::vector<EeyoreStatement> stmts;
	stmts.push_back(DeclStmt(OrigVar(0)));
	stmts.push_back(DeclStmt(OrigVar(1, 40)));
	stmts.push_back(FuncDefStmt("main", 0));
	stmts.push_back(DeclStmt(OrigVar(2)));
	stmts.push_back(DeclStmt(OrigVar(3)));
	stmts.push_back(DeclStmt(TempVar(0)));
	stmts.push_back(DeclStmt(TempVar(1)));
	stmts.push_back(DeclStmt(TempVar(2)));
	stmts.push_back(FuncCallStmt("getint", OrigVar(0)));
	stmts.push_back(CondGotoStmt(OrigVar(0), BinaryOp::LE, 10, Label(0)));
	stmts.push_back(RetStmt(1));
	stmts.push_back(LabelStmt(Label(0)));
	stmts.push_back(ParamStmt(OrigVar(3)));
	stmts.push_back(FuncCallStmt("putint"));
	stmts.push_back(RetStmt(0));
	stmts.push_back(EndFuncDefStmt("main"));
	std::cout << stmts << std::endl;
	return 0;
}

*/