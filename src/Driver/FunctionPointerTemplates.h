#pragma once
template <typename Function>
struct function_traits
	: public function_traits<decltype(&Function::operator())>
{};

template <typename ClassType, typename ReturnType, typename TContext, typename... Args>
struct function_traits<ReturnType(ClassType::*)(TContext, Args...) const>
{
	typedef ReturnType(*pointer)(TContext, Args...);
	typedef std::function<ReturnType(TContext, Args...)> function;
	typedef TContext ctx;
};

template <typename Function>
typename function_traits<Function>::pointer
to_function_pointer(Function& lambda)
{
	return static_cast<typename function_traits<Function>::pointer>(lambda);
}

template<typename Function>
typename function_traits<Function>::ctxPtr
to_ctx(Function& lambda)
{
	return static_cast<typename function_traits<Function>::ctxPtr>(lambda);
}
