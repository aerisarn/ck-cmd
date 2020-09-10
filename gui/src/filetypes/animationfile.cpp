#include "animationfile.h"

using namespace UI;

AnimationFile::AnimationFile(MainWindow *window, const QString & name)
    : HkxFile(window, name),
      duration(0)
{
    getReader().setFile(this);
}

qreal AnimationFile::getDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return duration;
}

bool AnimationFile::addObjectToFile(HkxObject *, long ){
    return true;
}

bool AnimationFile::parseBinary() {
	std::lock_guard <std::mutex> guard(mutex);
	auto ok = false;
	auto& handler = getBinaryHandler();
	if (handler.parse()) {
		size_t elements = handler.getNumElements();
		for (size_t i = 0; i < elements; i++) {
			const auto* hkclass = handler.getElementClass(i);
			const void* obj = handler.getElementObject(i);
		}
	}
	return false;
}

bool AnimationFile::parse(){
    std::lock_guard <std::mutex> guard(mutex);
    auto ok = false;
    if (getReader().parse()){
        auto value = getReader().findFirstValueWithAttributeValue("duration");
        duration = value.toFloat(&ok);
        if (!ok){
            LogFile::writeToLog(getFileName()+" :parse(): The animation duration was not found!");
        }
    }else{
        LogFile::writeToLog(getFileName()+" :parse(): The animation file failed to parse!");
    }
    return ok;
}
