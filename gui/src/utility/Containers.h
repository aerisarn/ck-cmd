#pragma once

#include <Common/Base/hkBase.h>
#include <Common/Base/Container/Array/hkArray.h>
#include <hkbVariableInfo_1.h>

#include <functional>

namespace ckcmd {
	namespace HKX {

		bool isWordVariable(int type);
		bool isQuadVariable(int type);
		bool isRefVariable(int type);

		template <typename T>
		static bool addToContainer(int row_start, int count, hkArray<T>& container)
		{
			if (row_start > container.getSize())
				return false;
			if (row_start == container.getSize())
			{
				for (int i = 0; i < count; i++)
					container.pushBack(T());
				return true;
			}
			for (int i = 0; i < count; i++)
				container.insertAt(row_start, T());
			return false;
		}

		template<typename T>
		int countElementsByType(hkArray<T>& container, std::function<bool(int)> typeClassifier)
		{
			int count = 0;
			for (int v = 0; v < container.getSize(); ++v)
			{
				if (typeClassifier(container[v].m_type))
					count++;
			}
			return count;
		}

		template<typename T>
		int findIndexByType(int row, hkArray<T>& container, std::function<bool(int)> typeClassifier)
		{
			int count = 0;
			for (int v = 0; v <= row; ++v)
			{
				if (typeClassifier(container[v].m_type))
					count++;
			}
			return count;
		}


		template<typename T>
		T& getElementByTypeIndex(int row_index, hkArray<T>& container, const hkArray<hkbVariableInfo>& type_container, std::function<bool(int)> typeClassifier)
		{
			int real_row_index = 0;
			int type_row_index = 0;
			for (int v = 0; v < type_container.getSize(); ++v)
			{
				if (typeClassifier(type_container[v].m_type))
				{
					if (type_row_index == row_index)
						return container[real_row_index];
					type_row_index++;
				}
				real_row_index++;
			}
			return T();
		}
	}
}


