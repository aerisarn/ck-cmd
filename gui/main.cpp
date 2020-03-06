#include <QApplication>
#include <QQmlApplicationEngine>

#include <core/HKXWrangler.h>
#include <hkbProjectData_2.h>

#include <QAbstractItemModel>
#include <QTreeView>
#include <QTableView>

using namespace ckcmd::HKX;

Q_DECLARE_METATYPE(hkVariant);
Q_DECLARE_METATYPE(hkRefPtr<hkReferencedObject>);

class QHkObject {
 
	const hkClass* _classInfo = NULL;
	void* _object = NULL;

	QVariant getReflectionData(int row, int column)
	{
		size_t members = _classInfo->getNumMembers();
		if (row < members + 1) {

			if (row == 0) {
				if (column == 0)
					return _classInfo->getName();
				else
					return QVariant();
			}
			auto member = _classInfo->getMember(row - 1);
			if (column == 0)
				return _classInfo->getMember(row - 1).getName();

			auto offset = member.getOffset();
			return TypeConverter(member, (uint8_t*)_object);
		}
		return QVariant();
	}

	QVariant InnerStructConverter(const hkClass* structType, uint8_t* object)
	{
		QMap<QString, QVariant> innerStruct;
		for (int innerfield = 0; innerfield < structType->getNumMembers(); innerfield++)
		{
			innerStruct[structType->getMember(innerfield).getName()] =
				TypeConverter(structType->getMember(innerfield), object);
		}
		return innerStruct;
	}

	size_t hkTypeSizeOf(hkClassMember::Type type)
	{
		switch (type)
		{
		case hkClassMember::Type::TYPE_VOID:
			return 0;
			/// hkBool, boolean type
		case hkClassMember::Type::TYPE_BOOL:
			return sizeof(hkBool);
			/// hkChar, signed char type
		case hkClassMember::Type::TYPE_CHAR:
			return sizeof(hkChar);
			/// hkInt8, 8 bit signed integer type
		case hkClassMember::Type::TYPE_INT8:
			return sizeof(hkInt8);
			/// hkUint8, 8 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT8:
			return sizeof(hkUint8);
			/// hkInt16, 16 bit signed integer type
		case hkClassMember::Type::TYPE_INT16:
			return sizeof(hkInt16);
			/// hkUint16, 16 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT16:
			return sizeof(hkUint16);
			/// hkInt32, 32 bit signed integer type
		case hkClassMember::Type::TYPE_INT32:
			return sizeof(hkInt32);
			/// hkUint32, 32 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT32:
			return sizeof(hkUint32);
			/// hkInt64, 64 bit signed integer type
		case hkClassMember::Type::TYPE_INT64:
			return sizeof(hkInt64);
			/// hkUint64, 64 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT64:
			return sizeof(hkUint64);
			/// hkReal, float type
		case hkClassMember::Type::TYPE_REAL:
			return sizeof(hkReal);
			/// hkVector4 type
		case hkClassMember::Type::TYPE_VECTOR4:
			return sizeof(hkVector4);
			/// hkQuaternion type
		case hkClassMember::Type::TYPE_QUATERNION:
			return sizeof(::hkQuaternion);
			/// hkMatrix3 type
		case hkClassMember::Type::TYPE_MATRIX3:
			return sizeof(hkMatrix3);
			/// hkRotation type
		case hkClassMember::Type::TYPE_ROTATION:
			return sizeof(hkRotation);
			/// hkQsTransform type
		case hkClassMember::Type::TYPE_QSTRANSFORM:
			return sizeof(hkQsTransform);
			/// hkMatrix4 type
		case hkClassMember::Type::TYPE_MATRIX4:
			return sizeof(hkMatrix4);
			/// hkTransform type
		case hkClassMember::Type::TYPE_TRANSFORM:
			return sizeof(hkTransform);
			/// Serialize as zero - deprecated.
		case hkClassMember::Type::TYPE_ZERO:
			return 0;
			/// Generic pointer, see member flags for more info
		case hkClassMember::Type::TYPE_POINTER:
			return sizeof(void*);
			/// Function pointer
		case hkClassMember::Type::TYPE_FUNCTIONPOINTER:
			return sizeof(void*);
		};
	}

	QVariant CollectionTypeConverter(hkClassMember::Type type, uint8_t* object, const hkClass* structType = NULL) {
		QList<QVariant> innerArray;
		
		uint8_t* m_array_data = (uint8_t*)(*(int*)_object);
		int m_array_size = *(int*)(object + 4);
		for (int arrayindex = 0; arrayindex < m_array_size; arrayindex++)
		{
			if (type == hkClassMember::Type::TYPE_STRUCT)
			{
				size_t struct_data_size = structType->getObjectSize();
				innerArray.push_back(InnerStructConverter(structType, object + 16 + struct_data_size * arrayindex));
			}
			else {
				innerArray.push_back(TypeConverter(type, object + 16 + hkTypeSizeOf(type)* arrayindex));
			}
		}
		return innerArray;
	}

	QVariant TypeConverter(hkClassMember::Type type, uint8_t* object)
	{
		QVariant outptr;
		switch (type)
		{
		case hkClassMember::Type::TYPE_VOID:
			return QVariant("VOID");
			/// hkBool, boolean type
		case hkClassMember::Type::TYPE_BOOL:
			return (*(hkBool*)(object));
			/// hkChar, signed char type
		case hkClassMember::Type::TYPE_CHAR:
			return (*(hkChar*)(object));
			/// hkInt8, 8 bit signed integer type
		case hkClassMember::Type::TYPE_INT8:
			return (*(hkInt8*)(object));
			/// hkUint8, 8 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT8:
			return (*(hkUint8*)(object));
			/// hkInt16, 16 bit signed integer type
		case hkClassMember::Type::TYPE_INT16:
			return (*(hkInt16*)(object));
			/// hkUint16, 16 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT16:
			return (*(hkUint16*)(object));
			/// hkInt32, 32 bit signed integer type
		case hkClassMember::Type::TYPE_INT32:
			return (*(hkInt32*)(object));
			/// hkUint32, 32 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT32:
			return (*(hkUint32*)(object));
			/// hkInt64, 64 bit signed integer type
		case hkClassMember::Type::TYPE_INT64:
			return (*(hkInt64*)(object));
			/// hkUint64, 64 bit unsigned integer type
		case hkClassMember::Type::TYPE_UINT64:
			return (*(hkUint64*)(object));
			/// hkReal, float type
		case hkClassMember::Type::TYPE_REAL:
			return (*(hkReal*)(object));
			/// hkVector4 type
		case hkClassMember::Type::TYPE_VECTOR4:
			return QVariant("TYPE_VECTOR4");
			/// hkQuaternion type
		case hkClassMember::Type::TYPE_QUATERNION:
			return QVariant("TYPE_QUATERNION");
			/// hkMatrix3 type
		case hkClassMember::Type::TYPE_MATRIX3:
			return QVariant("TYPE_MATRIX3");
			/// hkRotation type
		case hkClassMember::Type::TYPE_ROTATION:
			return QVariant("TYPE_ROTATION");
			/// hkQsTransform type
		case hkClassMember::Type::TYPE_QSTRANSFORM:
			return QVariant("TYPE_QSTRANSFORM");
			/// hkMatrix4 type
		case hkClassMember::Type::TYPE_MATRIX4:
			return QVariant("TYPE_MATRIX4");
			/// hkTransform type
		case hkClassMember::Type::TYPE_TRANSFORM:
			return QVariant("TYPE_TRANSFORM");
			/// Serialize as zero - deprecated.
		case hkClassMember::Type::TYPE_ZERO:
			return QVariant("TYPE_ZERO");
			/// Generic pointer, see member flags for more info
		case hkClassMember::Type::TYPE_POINTER:
			outptr.setValue(*(hkRefPtr<hkReferencedObject>*)(object));
			return outptr;
			/// Function pointer
		case hkClassMember::Type::TYPE_FUNCTIONPOINTER:
			return QVariant("TYPE_FUNCTIONPOINTER");
			/// char*, null terminated string
		case hkClassMember::Type::TYPE_CSTRING:
			return QString((char*)(*(hkUint32*)(object)));
			/// hkUlong, unsigned long, defined to always be the same size as a pointer
		case hkClassMember::Type::TYPE_ULONG:
			return (*(hkUint32*)(object));
			/// hkFlags<ENUM,STORAGE> - 8,16,32 bits of named values.
		case hkClassMember::Type::TYPE_FLAGS:
			return QVariant("TYPE_FLAGS");
			/// hkHalf, 16-bit float value
		case hkClassMember::Type::TYPE_HALF:
			return QVariant("TYPE_HALF");
			/// hkStringPtr, c-string
		case hkClassMember::Type::TYPE_STRINGPTR:		
			return QString(((hkStringPtr*)(object))->cString());
		};
	}

	QVariant TypeConverter(const hkClassMember& member, uint8_t* object) {
		const hkClass* memberClass;
		QVariant out;
		auto subtype = member.getSubType();

		if (member.getType() < hkClassMember::Type::TYPE_ARRAY || 
			(member.getType() > hkClassMember::Type::TYPE_VARIANT &&
			member.getType() != hkClassMember::Type::TYPE_RELARRAY))
			return TypeConverter(member.getType(), object + member.getOffset());

		switch (member.getType())
		{
		
			/// hkArray<T>, array of items of type T
		case hkClassMember::Type::TYPE_ARRAY:
			return CollectionTypeConverter(subtype, object + member.getOffset(), member.getClass());
			/// hkInplaceArray<T,N> or hkInplaceArrayAligned16<T,N>, array of N items of type T
		case hkClassMember::Type::TYPE_INPLACEARRAY:
			return QVariant("TYPE_INPLACEARRAY");
			/// hkEnum<ENUM,STORAGE> - enumerated values
		case hkClassMember::Type::TYPE_ENUM:
			return QVariant("TYPE_ENUM");
			/// Object
		case hkClassMember::Type::TYPE_STRUCT:
			return InnerStructConverter(member.getClass(), object + member.getOffset());
			/// Simple array (ptr(typed) and size only)
		case hkClassMember::Type::TYPE_SIMPLEARRAY:
			return QVariant("TYPE_SIMPLEARRAY");
			/// Simple array of homogeneous types, so is a class id followed by a void* ptr and size
		case hkClassMember::Type::TYPE_HOMOGENEOUSARRAY:
			return QVariant("TYPE_HOMOGENEOUSARRAY");
			/// hkVariant (void* and hkClass*) type
		case hkClassMember::Type::TYPE_VARIANT:
			out.setValue(*(hkVariant*)(*(int32_t*)object + member.getOffset()));
			return out;
			/// hkRelArray<>, attached const array values
		case hkClassMember::Type::TYPE_RELARRAY:
			return QVariant("TYPE_RELARRAY");
		default:
			return QVariant("Not Implemented!");
		}
		return out;
	}

public:
	QHkObject(const hkClass* classInfo, void* object) :
		_classInfo(classInfo), _object(object)
	{}

	QHkObject(const hkVariant& variant) :
		_classInfo(variant.m_class), _object(variant.m_object)
	{}

	QVariant at(int row, int column) {
		return getReflectionData(row, column);
	}
};

class QHkClassItemModel : public QAbstractItemModel
{
	size_t root_index = -1;
	hkArray<hkVariant> objects;

private:

	size_t indexOf(hkVariant* variant) {
		for (int i = 0; i < objects.getSize(); i++)
		{
			if (&objects[i] == variant)
				return i;
		}
		return -1;
	}



public:
	QHkClassItemModel(const fs::path& file) {
		HKXWrapper wrapper;
		auto rootPtr = wrapper.read(file, objects);
		for (int i = 0; i < objects.getSize(); i++)
		{
			if (objects[i].m_object == rootPtr)
			{
				root_index = i;
				break;
			}
		}
	}

	Q_INVOKABLE virtual QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override
	{
		if (!parent.isValid())
			return createIndex(row, column, (void*)&objects[root_index]);

		return QModelIndex();
	}

	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const override
	{
		return QModelIndex();
	}

	Q_INVOKABLE virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const override {
		return QModelIndex();
	}
	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
		const hkVariant* inner;
		if (!parent.isValid())
			inner = &objects[root_index];
		else
			inner = (hkVariant*)parent.internalPointer();
		return 1 + inner->m_class->getNumMembers();
	}
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override {
		return 2;
	}
	Q_INVOKABLE virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override {
		return false;
	}

	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
	{
		if (!index.isValid() || index == QModelIndex())
			return QHkObject(objects[root_index]).at(index.row(), index.column());
		
		hkVariant* inner = (hkVariant*)index.internalPointer();
		return QHkObject(*inner).at(index.row(), index.column());
	}

};

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter*		pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
	InitializeHavok();
	QHkClassItemModel* model = new QHkClassItemModel(fs::path("C:\\git_ref\\resources\\bsa\\meshes\\actors\\character\\defaultmale.hkx"));


	QTableView tree;
	tree.setModel(model);
	tree.show();

    //QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    auto result = app.exec();
	//CloseHavok();
	return result;
}

