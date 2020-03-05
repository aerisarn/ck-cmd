#include <QApplication>
#include <QQmlApplicationEngine>

#include <core/HKXWrangler.h>
#include <hkbProjectData_2.h>

#include <QAbstractItemModel>

using namespace ckcmd::HKX;

class QHkClassItemModel : public QAbstractItemModel
{
	hkRootLevelContainer* root = NULL;
	hkArray<hkVariant> objects;

private:

	QVariant TypeConverter(hkClassMember& member, void* object) {
		const hkClass* memberClass;
		if (member.hasClass())
			memberClass = member.getClass();
		switch (member.getType())
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
			return QVariant("TODO");
		/// hkQuaternion type
		case hkClassMember::Type::TYPE_QUATERNION:
			return QVariant("TODO");
				/// hkMatrix3 type
		case hkClassMember::Type::TYPE_MATRIX3:
			return QVariant("TODO");
				/// hkRotation type
		case hkClassMember::Type::TYPE_ROTATION:
			return QVariant("TODO");
				/// hkQsTransform type
		case hkClassMember::Type::TYPE_QSTRANSFORM:
			return QVariant("TODO");
				/// hkMatrix4 type
		case hkClassMember::Type::TYPE_MATRIX4:
			return QVariant("TODO");
				/// hkTransform type
		case hkClassMember::Type::TYPE_TRANSFORM:
			return QVariant("TODO");
				/// Serialize as zero - deprecated.
		case hkClassMember::Type::TYPE_ZERO:
			return QVariant("TODO");
				/// Generic pointer, see member flags for more info
		case hkClassMember::Type::TYPE_POINTER:
			return QVariant("TODO");
				/// Function pointer
		case hkClassMember::Type::TYPE_FUNCTIONPOINTER:
			return QVariant("TODO");
				/// hkArray<T>, array of items of type T
		case hkClassMember::Type::TYPE_ARRAY:

			return QVariant("TODO");
				/// hkInplaceArray<T,N> or hkInplaceArrayAligned16<T,N>, array of N items of type T
		case hkClassMember::Type::TYPE_INPLACEARRAY:
			return QVariant("TODO");
		/// hkEnum<ENUM,STORAGE> - enumerated values
		case hkClassMember::Type::TYPE_ENUM:
			return QVariant("TODO");
				/// Object
		case hkClassMember::Type::TYPE_STRUCT:
			return QVariant("TODO");
				/// Simple array (ptr(typed) and size only)
		case hkClassMember::Type::TYPE_SIMPLEARRAY:
			return QVariant("TODO");
				/// Simple array of homogeneous types, so is a class id followed by a void* ptr and size
		case hkClassMember::Type::TYPE_HOMOGENEOUSARRAY:
			return QVariant("TODO");
				/// hkVariant (void* and hkClass*) type
		case hkClassMember::Type::TYPE_VARIANT:
			return QVariant("TODO");
				/// char*, null terminated string
		case hkClassMember::Type::TYPE_CSTRING:
			return QVariant("TODO");
				/// hkUlong, unsigned long, defined to always be the same size as a pointer
		case hkClassMember::Type::TYPE_ULONG:
			return QVariant("TODO");
				/// hkFlags<ENUM,STORAGE> - 8,16,32 bits of named values.
		case hkClassMember::Type::TYPE_FLAGS:
			return QVariant("TODO");
				/// hkHalf, 16-bit float value
		case hkClassMember::Type::TYPE_HALF:
			return QVariant("TODO");
				/// hkStringPtr, c-string
		case hkClassMember::Type::TYPE_STRINGPTR:
			return QVariant("TODO");
				/// hkRelArray<>, attached const array values
		case hkClassMember::Type::TYPE_RELARRAY:
			return QVariant("TODO");
		default:
			return QVariant("Not Implemented!");
		}
		return QVariant();
	}

	template<typename ReflectionType>
	QVariant getReflectionData(ReflectionType* object, int row, int column)
	{
		const hkClass& m_class = object->staticClass();
		size_t members = m_class.getNumMembers();
		if (row < members + 1) {

			if (row == 0) {
				if (column == 0)
					return m_class.getName();
				else
					return QVariant();
			}
			auto member = m_class.getMember(row - 1);
			if (column == 0)
				return m_class.getMember(row - 1).getName();
			auto type = member.getType();
			auto subtype = member.getSubType();
			if (type != hkClassMember::Type::TYPE_ARRAY && column > 1)
				return QVariant();
			if (type == hkClassMember::Type::TYPE_ARRAY)
			{
				void* m_data = (void*)(*(int*)object);
				int m_size = *(int*)((char*)object + 4);
				if (subtype == hkClassMember::Type::TYPE_STRUCT)
				{
					char* struct_p = (char*)m_data;
					size_t struct_data = member.getClass()->getObjectSize();
					size_t index = column - 1;
					if (index < m_size) {
						struct_p = struct_p + struct_data * index;
						return this->createIndex(row, column, struct_p);
					}
				}
			}
			auto offset = member.getOffset();
			return TypeConverter(member, object);
		}
	}

public:
	QHkClassItemModel(const fs::path& file) {
		HKXWrapper wrapper;
		root = wrapper.read(file, objects);

		auto test = getReflectionData(root, 0, 0);
		auto test2 = getReflectionData(root, 1, 0);
		auto test3 = getReflectionData(root, 1, 1);
	}

	Q_INVOKABLE virtual QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override
	{
		return QModelIndex();
	}

	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const override
	{
		return QModelIndex();
	}

	Q_INVOKABLE virtual QModelIndex sibling(int row, int column, const QModelIndex &idx) const override {
		return QModelIndex();
	}
	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override  {
		return 1;
	}
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override  {
		return 1;
	}
	Q_INVOKABLE virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const override  {
		return false;
	}

	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
	{
		return QVariant();
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
	QHkClassItemModel model(fs::path("C:\\git_ref\\resources\\bsa\\meshes\\actors\\character\\defaultmale.hkx"));



    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    auto result = app.exec();
	CloseHavok();
	return result;
}

