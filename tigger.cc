#include "tigger.h"

using std::endl;

namespace compiler_skeleton::tigger
{

void RegPrinter::operator() (const ZeroReg &reg)
{
	out << 'x' << reg.id;
}

void RegPrinter::operator() (const CalleeSavedReg &reg)
{
	out << 's' << reg.id;
}

void RegPrinter::operator() (const CallerSavedReg &reg)
{
	out << 't' << reg.id;
}

void RegPrinter::operator() (const ArgReg &reg)
{
	out << 'a' << reg.id;
}


void TiggerPrinter::operator() (const GlobalVarDeclStmt &stmt)
{
	out << stmt.var << " = " << stmt.initial_val << endl;
}

void TiggerPrinter::operator() (const GlobalArrDeclStmt &stmt)
{
	out << stmt.var << " = malloc " << stmt.size << endl;
}

void TiggerPrinter::operator() (const FuncHeaderStmt &stmt)
{
	out << stmt.func_name << " [" << stmt.arg_cnt << "] ["
		<< stmt.stack_size << ']' << endl;
}

void TiggerPrinter::operator() (const FuncEndStmt &stmt)
{
	out << "end " << stmt.func_name << endl;
}

void TiggerPrinter::operator() (const UnaryOpStmt &stmt)
{
	out << "  " << stmt.opr << " = " << stmt.op_type << stmt.opr1 << endl;
}

void TiggerPrinter::operator() (const BinaryOpStmt &stmt)
{
	out << "  " << stmt.opr << " = " << stmt.opr1 << ' ' << stmt.op_type
		<< ' ' << stmt.opr2 << endl;
}

void TiggerPrinter::operator() (const MoveStmt &stmt)
{
	out << "  " << stmt.opr << " = " << stmt.opr1 << endl;
}

void TiggerPrinter::operator() (const ReadArrStmt &stmt)
{
	out << "  " << stmt.opr << " = " << stmt.opr1 << '[' << stmt.idx << ']' << endl;
}

void TiggerPrinter::operator() (const WriteArrStmt &stmt)
{
	out << "  " << stmt.opr1 << '[' << stmt.idx << "] = " << stmt.opr << endl;
}

void TiggerPrinter::operator() (const CondGotoStmt &stmt)
{
	out << "  if " << stmt.opr1 << ' ' << stmt.op_type << ' ' << stmt.opr2
		<< " goto " << stmt.goto_label << endl;
}

void TiggerPrinter::operator() (const GotoStmt &stmt)
{
	out << "  goto " << stmt.goto_label << endl;
}

void TiggerPrinter::operator() (const LabelStmt &stmt)
{
	out << stmt.label << ':' << endl;
}

void TiggerPrinter::operator() (const FuncCallStmt &stmt)
{
	out << "  call " << stmt.func_name << endl;
}

void TiggerPrinter::operator() (const ReturnStmt &stmt)
{
	out << "  return" << endl;
}

void TiggerPrinter::operator() (const StoreStmt &stmt)
{
	out << "  store " << stmt.opr << ' ' << stmt.stack_offset << endl;
}

void TiggerPrinter::operator() (const LoadStmt &stmt)
{
	out << "  load " << stmt.src << ' ' << stmt.opr << endl;
}

void TiggerPrinter::operator() (const LoadAddrStmt &stmt)
{
	out << "  loadaddr " << stmt.src << ' ' << stmt.opr << endl;
}

} // namespace compiler_skeleton::tigger

std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::Label &label)
{
	return out << 'l' << label.id;
}

std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::Reg &reg)
{
	compiler_skeleton::tigger::RegPrinter printer{out};
	std::visit(printer, reg);
	return out;
}

std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::GlobalVar &global_var)
{
	return out << 'v' << global_var.id;
}

std::ostream &operator << (std::ostream &out, const compiler_skeleton::tigger::TiggerStatement &stmt)
{
	compiler_skeleton::tigger::TiggerPrinter tigger_printer(out);
	std::visit(tigger_printer, stmt);
	return out;
}