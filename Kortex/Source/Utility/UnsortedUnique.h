#pragma once
#include <Kortex/Kortex.hpp>

// TODO: I'm not sure that thing even works. Check and fix or remove.
namespace Kortex::Utility
{
	template<class IteratorType, class EqCmpFunc, class LtCmpFunc>
	IteratorType UnsortedUnique(IteratorType begin, IteratorType end, EqCmpFunc eqcmp, LtCmpFunc ltcmp)
	{
		using value_t = typename std::iterator_traits<IteratorType>::value_type;
		using pair_t = std::pair<value_t, size_t>;

		std::vector<pair_t> v;
		v.reserve(std::distance(begin, end));
		for (IteratorType c = begin; c != end; ++c)
		{
			// Second is start index
			v.push_back(std::make_pair(*c, v.size()));
		}

		// Sort by value then by index
		std::sort(v.begin(), v.end(), [&ltcmp](const pair_t& a, const pair_t& b)
		{
			return ltcmp(a.first, b.first);
		});
		v.erase(std::unique(v.begin(), v.end(), [&eqcmp](const pair_t& a, const pair_t& b)
		{
			return eqcmp(a.first, b.first);
		}), v.end());

		// Restore order
		std::sort(v.begin(), v.end(), [](const pair_t& a, const pair_t& b)
		{
			return a.second < b.second;
		});

		IteratorType c = begin;
		for (const auto& x: v)
		{
			*(c++) = x.first;
		}
		return c;
	}
}
