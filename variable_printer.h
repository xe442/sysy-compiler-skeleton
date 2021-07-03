#ifndef SKELETON_VARIABLE_PRINTER_H
#define SKELETON_VARIABLE_PRINTER_H

#include <iostream>
#include <variant>
#include "lambda_visitor.h"

template<class T, class ...Ts>
std::ostream &operator << (std::ostream &out, const std::variant<T, Ts...> &v)
{
	compiler_skeleton::utils::LambdaVisitor printer =
	{
		[&out](const auto &tp) -> std::ostream & { return out << tp; }
	};
	return std::visit(printer, v);
}

#endif