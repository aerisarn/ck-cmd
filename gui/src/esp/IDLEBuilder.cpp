#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>

#include "IDLEBuilder.h"

using namespace ckcmd::HKX;

IDLEBuilder::IDLEBuilder(size_t file_index, ProjectNode* root, ResourceManager& manager) :
	_manager(manager),
	_file_index(file_index),
	_root(root)
{
	auto aacts = _manager.actions();
	auto idles = _manager.idles(_file_index);

	std::map<FORMID, ProjectNode*> nodes;

	for (const auto& aact : aacts)
	{
		nodes[aact->formID] = _manager.createActionNode
		(
			_file_index,
			{ aact->EDID.value, (unsigned long long)aact },
			root
		);
		root->appendChild(nodes[aact->formID]);
	}
	nodes[NULL] = _manager.createActionNode
	(
		_file_index,
		{ "LOOSE" },
		root
	);
	root->appendChild(nodes[NULL]);

	for (const auto& idle : idles)
	{
		QString display_name = QString("%1 <%2>").arg(idle->EDID.value).arg(idle->ENAM.value);
		nodes[idle->formID] =  _manager.createActionNode
		(
			_file_index,
			{ 
				display_name,
				(unsigned long long)idle
			},
			NULL
		);
	}

	for (const auto& idle : idles)
	{
		auto child = nodes[idle->formID];
		auto parent = nodes[idle->ANAM.value.parent];
		child->setParent(parent);
		if (!parent->hasChild(child))
			parent->appendChild(child);
	}

	for (const auto& idle : idles)
	{
		auto node = nodes[idle->formID];

		if (node->childCount() > 1)
		{
			//sanity: only one with sibling == null
			std::set<ProjectNode*> nullSiblings;
			for (int i = 0; i < node->childCount(); i++)
			{
				Sk::IDLERecord* another_idle = (Sk::IDLERecord*)node->child(i)->data(1).value<unsigned long long>();
				if (another_idle->ANAM.value.sibling == NULL)
					nullSiblings.insert(node->child(i));
			}

			//if (nullSiblings.size() != 1)
			//	__debugbreak();

			std::vector<ProjectNode*> to_reorder;
			int last_size = to_reorder.size();
			while (to_reorder.size() < node->childCount())
			{


				for (int i = 0; i < node->childCount(); i++)
				{
					Sk::IDLERecord* another_idle = (Sk::IDLERecord*)node->child(i)->data(1).value<unsigned long long>();
					if (to_reorder.empty() && another_idle->ANAM.value.sibling == NULL)
					{
						to_reorder.push_back(node->child(i));
						break;
					}
					else if (!to_reorder.empty() && nodes[another_idle->ANAM.value.sibling] == *to_reorder.rbegin())
					{
						to_reorder.push_back(node->child(i));
						break;
					}
				}
				if (last_size == to_reorder.size())
				{
					Sk::IDLERecord* last_inserted = (Sk::IDLERecord*)(*to_reorder.rbegin())->data(1).value<unsigned long long>();
					//Stich back the queue. Sigh
					for (auto& head_node : nullSiblings)
					{
						if (head_node != *to_reorder.begin())
						{
							to_reorder.push_back(head_node);
							break;
						}
					}
				}
				last_size = to_reorder.size();
			}
			for (int i = 0; i < node->childCount(); i)
				node->removeChild(0);
			for (auto& child : to_reorder)
				node->appendChild(child);
		}
	}
}