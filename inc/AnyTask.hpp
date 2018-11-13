#pragma  once
#include <functional>

class _AnyTask
{
public:
	virtual ~_AnyTask() {}

	virtual void _Run() = 0;
};

template<class _Fx, class... _Args>
class _AnyTaskImpl : public _AnyTask
{
public:

	_AnyTaskImpl(_Fx&& _Func, _Args&&... _Ax)
		: _binder(std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...) {}

	~_AnyTaskImpl() {}

	virtual void _Run()
	{
		_binder();
	}

private:
	std::_Binder<std::_Unforced, _Fx, _Args...> _binder;
};