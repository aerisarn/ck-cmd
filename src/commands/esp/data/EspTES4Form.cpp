#include <commands\esp\data\EspTES4Form.h>

void EspTES4Form::Write(EspWriter& w)
{
	// FORM HEADER
	EspFormHeader header(GetHeader());
	header.Write(w);

	// MANDATORY DATA
	EspFieldHeader hedr('HEDR', 12);
	WriteField(hedr, w);
	w.Write<EspTES4Hedr>(HEDR);

	if (!GetAuthor().empty())
	{
		EspFieldHeader cnam('CNAM', (EspUInt16)GetAuthor().size() + 1);
		WriteField(cnam, w);
		w.WriteZString(GetAuthor());
	}

	if (!GetDesc().empty())
	{
		EspFieldHeader snam('SNAM', (EspUInt16)GetDesc().size() + 1);
		WriteField(snam, w);
		w.WriteZString(GetDesc());
	}

	for (unsigned int i = 0; i < GetMasters().size(); i++)
	{
		EspTES4Master master = GetMasters().at(i);

		EspFieldHeader mast('MAST', (EspUInt16)master.filename.size() + 1);
		WriteField(mast, w);
		w.WriteZString(master.filename);

		EspFieldHeader data('DATA', (EspUInt16)sizeof(EspUInt64));
		WriteField(data, w);
		w.Write<EspUInt64>(master.size);
	}

	if (!GetOverrides().empty())
	{
		EspFieldHeader onam(
			'ONAM', (EspUInt16)GetOverrides().size() * sizeof(EspFormID)
		);
		WriteField(onam, w);

		for (unsigned int i = 0; i < GetOverrides().size(); i++)
		{
			w.Write<EspFormID>(GetOverrides().at(i));
		}
	}

	// POSSIBLY MANDATORY
	EspFieldHeader intv('INTV', sizeof(EspUInt32));
	WriteField(intv, w);
	w.Write(GetINTV());

	if (GetINCC() != 0)
	{
		EspFieldHeader incc('INCC', sizeof(EspUInt32));
		WriteField(incc, w);
		w.Write(GetINCC());
	}
}

void EspTES4Form::UpdateSize()
{
	EspUInt16 size = 28; // All mandatory data is 28 bytes

	if (!GetAuthor().empty())
	{
		size += (EspUInt16)GetAuthor().size() + 7;
	}

	if (!GetDesc().empty())
	{
		size += (EspUInt16)GetDesc().size() + 7;
	}

	for (unsigned int i = 0; i < GetMasters().size(); i++)
	{
		EspTES4Master master = GetMasters().at(i);
		size += (EspUInt16)sizeof(master) + 13;
	}

	if (!GetOverrides().empty())
	{
		size += (EspUInt16)(GetOverrides().size() * sizeof(EspUInt64)) + 6;
	}

	if (GetINCC() != 0)
	{
		size += (EspUInt16)sizeof(EspUInt32) + 6;
	}

	header.SetDataSize(size);
}
