#pragma once

template <typename T>
class Instance
{
public:
	static T* GetInstance() {
		static T instance;
		return &instance;
	}
};