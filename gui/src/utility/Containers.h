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
		int variantLink(int row, int column, hkVariant* variant);

		template <typename T>
		static bool addToContainer(int row_start, int count, hkArray<T>& container)
		{
			if (row_start > container.getSize())
				return false;
			if (row_start == container.getSize())
			{
				T t;
				memset(&t, 0, sizeof(T));
				for (int i = 0; i < count; i++)
					container.pushBack(t);
				return true;
			}
			for (int i = 0; i < count; i++)
			{
				T t;
				memset(&t, 0, sizeof(T));
				container.insertAt(row_start, t);
				return true;
			}
			return false;
		}

		template <typename T>
		static bool removeFromContainer(int row_start, int count, hkArray<T>& container)
		{
			if (row_start + count <= container.getSize())
			{
				container.removeAtAndCopy(row_start, count);
				return true;
			}
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


