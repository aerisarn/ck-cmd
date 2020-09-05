#include "hkxfile.h"
#include "src/ui/mainwindow.h"

using namespace UI;

HkxFile::HkxFile(MainWindow *window, const QString & name)
    : QFile(name),
      ui(window),
      changed(false)
{
    fileNameWithoutPath = name.section("/", -1, -1);
}

void HkxFile::closeFile(){
    (isOpen()) ? close() : NULL;
}

bool HkxFile::getIsChanged() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return changed;
}

void HkxFile::setIsChanged(bool wasEdited){
    //std::lock_guard <std::mutex> guard(mutex);
    changed = wasEdited;
}

QString HkxFile::getFileName() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return fileNameWithoutPath;
}

void HkxFile::setHKXFileName(const QString &name){
    //std::lock_guard <std::mutex> guard(mutex);
    QFile::setFileName(name);
    fileNameWithoutPath = name.section("/", -1, -1);
}

HkxSharedPtr & HkxFile::getRootObject(){
    //std::lock_guard <std::mutex> guard(mutex);
    return rootObject;
}

bool HkxFile::parse(){
    return true;
}

bool HkxFile::link(){
    return true;
}

void HkxFile::setRootObject(HkxSharedPtr & obj){
    //std::lock_guard <std::mutex> guard(mutex);
    rootObject = obj;
}

QString HkxFile::getRootObjectReferenceString() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (rootObject.data()){
        return rootObject->getReferenceString();
    }
    return true;
}

bool HkxFile::appendAndReadData(long &index, HkxObject *obj){
    //std::lock_guard <std::mutex> guard(mutex);
    if (!obj->readData(reader, ++index)){
        return false;
    }
    return true;
}

HkxXmlReader & HkxFile::getReader(){
    //std::lock_guard <std::mutex> guard(mutex);
    return reader;
}

HkxXMLWriter & HkxFile::getWriter(){
    //std::lock_guard <std::mutex> guard(mutex);
    return writer;
}

HkxBinaryHandler & HkxFile::getBinaryHandler() {
	return binaryHandler;
}

MainWindow *HkxFile::getUI() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return ui;
}

