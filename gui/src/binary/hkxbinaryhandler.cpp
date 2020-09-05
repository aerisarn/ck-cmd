#include "hkxbinaryhandler.h"
#include "src/filetypes/behaviorfile.h"

#include <mutex>

using namespace UI;

HkxBinaryHandler::HkxBinaryHandler(HkxFile *file)
    : hkxBinaryFile(file)
{
}

bool HkxBinaryHandler::parse(){
	hkIstream stream(hkxBinaryFile->getFileName().toLocal8Bit().data());
	hkStreamReader *reader = stream.getStreamReader();
	hkResource* resource = hkSerializeLoadResource(reader, objects);
    return false;
}

void HkxBinaryHandler::setFile(HkxFile *file){
    hkxBinaryFile = file;
}

int HkxBinaryHandler::getNumElements() const{
	return objects.getSize();
}

void HkxBinaryHandler::clear(){
	objects.clear();
}
