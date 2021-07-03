#ifndef SKELETON_LAMBDA_VISITOR
#define SKELETON_LAMBDA_VISITOR

namespace compiler_skeleton::utils
{

// A visitor class that uses lambda functions to overload operator ().
// Example:
//	LambdaVisitor visitor = {
// 		[](const int &i) { cout << "int" << endl;},
//		[](const double &d) { cout << "double" << endl; },
//		[](const auto &a) { cout << "unrecognized type" << endl; }
//	};
//	std::variant<int,double> v = 2.33;
//	std::visit(visitor, v);
template<class... Ts> struct LambdaVisitor : Ts... { using Ts::operator()...; };
template<class... Ts> LambdaVisitor(Ts...) -> LambdaVisitor<Ts...>;

} // namespace compiler_skeleton::utils

#endif