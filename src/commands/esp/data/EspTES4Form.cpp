#include <commands\esp\data\EspTES4Form.h>

void EspTES4Form::Write(EspWriter& w)
{
	// MANDATORY DATA
	EspFieldHeader hedr('HEDR', 12);
	WriteField(hedr, w);
	w.Write<EspTES4Hedr>(HEDR);

	if (!getAuthor().empty())
	{
		EspFieldHeader cnam('CNAM', getAuthor().size() + 1);
		WriteField(cnam, w);
		w.Write<std::string>(getAuthor());
	}

	if (!getDesc().empty())
	{
		EspFieldHeader snam('SNAM', getDesc().size() + 1);
		WriteField(snam, w);
		w.Write<std::string>(getDesc());
	}

	for (int i = 0; i < getMasters().size(); i++)
	{
		EspTES4Master master = getMasters().at(i);

		EspFieldHeader mast('MAST', master.filename.size() + 1);
		WriteField(mast, w);
		w.Write<std::string>(master.filename);

		EspFieldHeader data('DATA', sizeof(EspUInt64));
		WriteField(data, w);
		w.Write<EspUInt64>(master.size);
	}

	if (!getOverrides().empty())
	{
		EspFieldHeader onam('ONAM', getOverrides().size() * sizeof(EspFormID));
		WriteField(onam, w);

		for (int i = 0; i < getOverrides().size(); i++)
		{
			w.Write<EspFormID>(getOverrides().at(i));
		}
	}

	// POSSIBLY MANDATORY
	EspFieldHeader intv('INTV', sizeof(EspUInt32));
	WriteField(intv, w);
	w.Write(getINTV());

	if (getINCC() != 0)
	{
		EspFieldHeader incc('INCC', sizeof(EspUInt32));
		WriteField(incc, w);
		w.Write(getINCC());
	}
}
