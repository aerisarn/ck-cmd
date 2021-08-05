//#include "HkxItemTreeModel.h"
//
//#include <src\hkx\NameGetter.h>
//#include <src\hkx\Getter.h>
//#include <src\hkx\RowCalculator.h>
//#include <src\hkx\ColumnCalculator.h>
//#include <src\hkx\HkxItemPointer.h>
//#include <src\hkx\HkxTableVariant.h>
//
//
//
//#include <QStack>
//
//namespace ckcmd {
//	namespace HKX {
//
//		class TreeBuilder : public HkxConcreteVisitor<TreeBuilder> {
//			const HkxItemTreeModel& _model;
//			ProjectNode* _parent;
//		public:
//			TreeBuilder(ProjectNode* parent, const HkxItemTreeModel& model)
//				: _model(model), _parent(parent),
//				HkxConcreteVisitor(*this)
//			{
//			}
//
//			template<typename T>
//			void visit(T& value) {}
//
//			void visit(char* value) {}
//
//			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override
//			{
//				ProjectNode* member_node = _parent->appendChild(
//					new ProjectNode({ QString::fromStdString(_classmember->getName()) }, _parent));
//				//if (pointer_type. == &hkReferencedObjectClass) {
//					int object_index = _model.findIndex((const void*)*(uintptr_t*)(v));
//					if (object_index >= 0)
//					{
//						ProjectNode* object_node = member_node->appendChild(
//							new ProjectNode({ QString::fromStdString(_model.at(object_index).m_class->getName()), object_index }, member_node));
//						HkxTableVariant h(const_cast<hkVariant&>(_model.at(object_index)));
//						TreeBuilder t(object_node, _model);
//						h.accept(t);
//					}
//				//}
//			}
//			virtual void visit(void* object, const hkClassMember& definition) override {
//				//array
//				
//				const auto& arraytype = definition.getArrayType();
//				const auto& arrayclass = definition.getStructClass();
//				size_t elements = definition.getSizeInBytes() / definition.getArrayMemberSize();
//				char* base = (char*)object;
//				if (hkClassMember::TYPE_ARRAY == definition.getType())
//				{
//					base = (char*)*(uintptr_t*)object; //m_data
//					elements = *(int*)((char*)object + sizeof(uintptr_t)); // m_size
//				}
//				for (int i = 0; i < elements; i++) {
//					void* element = base + i * definition.getArrayMemberSize();
//					if (definition.hasClass()) {
//						hkVariant v;
//						v.m_class = definition.getClass();
//						v.m_object = element;
//						HkxTableVariant h(v);
//						TreeBuilder t(_parent, _model);
//						h.accept(t);
//					}
//					else if (hkClassMember::TYPE_POINTER == arraytype) {
//						recurse(element, arraytype);
//					}
//				}				
//			}
//			virtual void visit(void*, const hkClassEnum& enum_type) override 
//			{
//
//			}
//			virtual void visit(void* object, const hkClass& object_type) override 
//			{
//				hkVariant v;
//				v.m_class = &object_type;
//				v.m_object = object;
//				HkxTableVariant h(v);
//				TreeBuilder t(_parent, _model);
//				h.accept(t);
//			}
//			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override 
//			{
//
//			}
//
//		};
//	}
//}
//
//using namespace ckcmd::HKX;
//
//
//
//HkxItemTreeModel::HkxItemTreeModel(const fs::path& file, QObject* parent) :
//	QAbstractItemModel(parent)
//{
//	HKXWrapper temp;
//	temp.read(file, _objects);
//	int root_index = -1;
//	for (int i = 0; i < _objects.getSize(); i++) {
//		if (_objects[i].m_class == &hkRootLevelContainerClass)
//		{
//			_root = &_objects[i];
//			root_index = i;
//			break;
//		}
//	}
//	QStack<QVariant> stack;
//	_rootNode = new ProjectNode({ QString::fromStdString(_root->m_class->getName()), root_index });
//	TreeBuilder tree(_rootNode, *this);
//	HkxTableVariant h(*_root);
//	h.accept(tree);
//}
//
//const hkVariant* HkxItemTreeModel::find(const void* object) const
//{
//	for (int i = 0; i < _objects.getSize(); i++) {
//		if (_objects[i].m_object == object)
//			return &_objects[i];
//	}
//	return NULL;
//}
//
//int HkxItemTreeModel::findIndex(const void* object) const
//{
//	for (int i = 0; i < _objects.getSize(); i++) {
//		if (_objects[i].m_object == object)
//			return i;
//	}
//	return -1;
//}
//
//
//hkVariant* HkxItemTreeModel::find(void* object)
//{
//	return const_cast<hkVariant*>(const_cast<const HkxItemTreeModel*>(this)->find(object));
//}
//
//int HkxItemTreeModel::findIndex(void* object)
//{
//	return const_cast<const HkxItemTreeModel*>(this)->findIndex(object);
//}
//
//hkVariant* HkxItemTreeModel::getObject(const QModelIndex& index) const {
//	return (hkVariant*)index.internalId();
//}
//
///*
//** AbstractItemModel(required methods)
//*/
//
//QVariant HkxItemTreeModel::data(const QModelIndex& index, int role) const
//{
//	if (!index.isValid())
//		return QVariant();
//
//	if (role != Qt::DisplayRole)
//		return QVariant();
//
//	ProjectNode* item = static_cast<ProjectNode*>(index.internalPointer());
//
//	return item->data(index.column());
//}
//
////row = field
////column = number of values in the field (1 scalar, >1 vector/matrix)
//QModelIndex HkxItemTreeModel::index(int row, int column, const QModelIndex& parent) const
//{
//	if (!hasIndex(row, column, parent))
//		return QModelIndex();
//
//	ProjectNode* parentItem;
//
//	if (!parent.isValid())
//		parentItem = _rootNode;
//	else
//		parentItem = static_cast<ProjectNode*>(parent.internalPointer());
//
//	ProjectNode* childItem = parentItem->child(row);
//	if (childItem)
//		return createIndex(row, column, childItem);
//	return QModelIndex();
//}
//
//QModelIndex HkxItemTreeModel::parent(const QModelIndex& index) const
//{
//	if (!index.isValid())
//		return QModelIndex();
//
//	ProjectNode* childItem = static_cast<ProjectNode*>(index.internalPointer());
//	ProjectNode* parentItem = childItem->parentItem();
//
//	if (parentItem == _rootNode)
//		return QModelIndex();
//
//	return createIndex(parentItem->row(), 0, parentItem);
//}
//
//int HkxItemTreeModel::rowCount(const QModelIndex& parent) const
//{
//	ProjectNode* parentItem;
//	if (parent.column() > 0)
//		return 0;
//
//	if (!parent.isValid())
//		parentItem = _rootNode;
//	else
//		parentItem = static_cast<ProjectNode*>(parent.internalPointer());
//
//	return parentItem->childCount();
//}
//
//int HkxItemTreeModel::columnCount(const QModelIndex& parent) const
//{
//	if (parent.isValid())
//		return static_cast<ProjectNode*>(parent.internalPointer())->columnCount();
//	return _rootNode->columnCount();
//}
//
//QVariant HkxItemTreeModel::headerData(int section, Qt::Orientation orientation,
//	int role) const
//{
//	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
//		return _rootNode->data(section);
//
//	return QVariant();
//}
//
//Qt::ItemFlags HkxItemTreeModel::flags(const QModelIndex& index) const
//{
//	if (!index.isValid())
//		return Qt::NoItemFlags;
//
//	return QAbstractItemModel::flags(index);
//}
//
//bool HkxItemTreeModel::setData(const QModelIndex& index, const QVariant& value,
//	int role)
//{
//	return false;
//}