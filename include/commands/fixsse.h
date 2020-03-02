// Command Base
#include <commands/CommandBase.h>

#include <filesystem>

#include <niflib.h>
#include <obj\NiObject.h>
#include <obj\BSFadeNode.h>

#include <objDecl.cpp>
#include <field_visitor.h>
#include <interfaces\typed_visitor.h>

//hierarchy
#include <obj/NiTimeController.h>
#include <obj/NiExtraData.h>
#include <obj/NiCollisionObject.h>
#include <obj/NiProperty.h>
#pragma once

#include <obj/NiDynamicEffect.h>

#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <utility>

#include <bitset>
#include <filesystem>

#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>
#include <core/MathHelper.h>
#include <commands/Geometry.h>

#include <unordered_map>

namespace fs = std::experimental::filesystem;

Niflib::NiTriShapeRef remake_partitions(Niflib::NiTriBasedGeomRef iShape, int & maxBonesPerPartition, int & maxBonesPerVertex, bool make_strips, bool pad);

template <typename T, typename Compare>
std::vector<std::size_t> sort_permutation(
	const std::vector<T>& vec,
	Compare& compare)
{
	std::vector<std::size_t> p(vec.size());
	std::iota(p.begin(), p.end(), 0);
	std::sort(p.begin(), p.end(),
		[&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });
	return p;
}

template <typename T>
std::vector<T> apply_permutation(
	const std::vector<T>& vec,
	const std::vector<std::size_t>& p)
{
	std::vector<T> sorted_vec(vec.size());
	std::transform(p.begin(), p.end(), sorted_vec.begin(),
		[&](std::size_t i) { return vec[i]; });
	return sorted_vec;
}

namespace ckcmd {
	namespace fixsse {

		class FixSSENif : public CommandBase
		{
			REGISTER_COMMAND_HEADER(FixSSENif)

		private:
			FixSSENif();
			virtual ~FixSSENif();

		public:
			virtual string GetName() const;
			virtual string GetHelp() const;
			virtual string GetHelpShort() const;

		protected:
			virtual bool InternalRunCommand(map<string, docopt::value> parsedArgs);
		};
	}
}
