#include "HkxLinkedTableVariant.h"

#include <src\hkx\LinkGetter.h>

using namespace ckcmd::HKX;

std::vector<Link> HkxLinkedTableVariant::links()
{
	LinkGetter linker;
	accept(linker);
	return linker.links();
}