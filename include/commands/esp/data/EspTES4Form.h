#ifndef ESPTES4FORM_H
#define ESPTES4FORM_H

#define FORM_MEMBER(type, name) \
private: \
    type name; \
public: \
    const type& get##name##() const { return name; } \
    void set##name##(const type& val) { name = val; }

#include <commands\esp\data\EspForm.h>
#include <vector>

struct EspTES4Master
{
	std::string filename;
	EspUInt64 size;
};

struct EspTES4Hedr
{
	EspFloat32 version;
	EspInt32 numRecords;
	EspUInt32 nextFormID;
};

typedef struct EspTES4Master EspTES4Master;
typedef struct EspTES4Hedr EspTES4Hedr;

class EspTES4Form : public EspForm
{
	FORM_MEMBER(EspTES4Hedr, HEDR)
	FORM_MEMBER(std::string, Author)
	FORM_MEMBER(std::string, Desc)
	FORM_MEMBER(std::vector<EspTES4Master>, Masters)
	FORM_MEMBER(std::vector<EspFormID>, Overrides)
	FORM_MEMBER(EspUInt32, INTV)
	FORM_MEMBER(EspUInt32, INCC)

	EspTES4Form(const EspTES4Hedr& hedr, const EspUInt32 intv) 
		: HEDR(hedr), INTV(intv), INCC(0) {}
	~EspTES4Form() {}
	
	void Write(EspWriter& w);
};

#endif //ESPTES4FORM_H