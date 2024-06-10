#pragma once

template <size_t N>
constexpr const char* ExtractVariableNameFromDataTypeName(const char(&name)[N])
{
	for (size_t i = N - 1; i > 0; --i)
	{
		if (name[i - 1] == ':')
		{
			return name + i;
		}
	}

	return name;
}