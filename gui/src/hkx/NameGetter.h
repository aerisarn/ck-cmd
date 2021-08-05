#include "HkxItemVisitor.h"
#include <QString>

namespace ckcmd {
	namespace HKX {

		class NameGetter : public HkxConcreteVisitor<NameGetter> {
			
			QString _name;
			int _rows = 0;
			int _target_row;
			std::string _parent_prefix;

			const hkClassMember& getSerializedMember(size_t row);

		public:

			NameGetter(const int row, std::string parent_prefix) : 
				HkxConcreteVisitor(*this), 
				_target_row(row), 
				_parent_prefix(parent_prefix) 
			{}

			QString name();

			template<typename T> void visit(T& value);

			virtual void visit(char* value) override;
			virtual void visit(void* v, const hkClass& pointer_type, hkClassMember::Flags flags) override;
			virtual void visit(void* object, const hkClassMember& definition) override;
			virtual void visit(void*, const hkClassEnum& enum_type, hkClassMember::Type type) override;
			virtual void visit(void* object, const hkClass& object_type, const char* member_name) override;
			virtual void visit(void* v, const hkClassEnum& enum_type, size_t storage) override;

		};
	}
}