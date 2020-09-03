#include "hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

#include <mutex>

using namespace UI;

HkxXmlReader::HkxXmlReader(HkxFile *file)
    : hkxXmlFile(file),
      isEOF(false),
      lineNumber(0)
{
    //
}

bool HkxXmlReader::parse(){
    auto result = NoError;
    if (hkxXmlFile->open(QIODevice::ReadOnly)){
        auto line = hkxXmlFile->readLine(MAX_HKXXML_LINE_LENGTH);
        lineNumber = 1;
        isEOF = false;
        if (line == "<?xml version=\"1.0\" encoding=\"ascii\"?>\n" || line == "<?xml version=\"1.0\" encoding=\"ascii\"?>\r\n"){
            while (!isEOF){
                result = readNextLine();
                (result != NoError && result != EmptyLine && result != EmptyLineEndFile) ? LogFile::writeToLog("HkxXmlReader: parse() failed because readNextLine() failed!") : NULL;
            }
            (!indexOfElemTags.isEmpty()) ? LogFile::writeToLog("HkxXmlReader: parse() failed because there are orphaned element tags!!!") : NULL;
            isEOF = true;
            return true;
        }else{
            LogFile::writeToLog("HkxXmlReader: parse() failed!\nThe file "+hkxXmlFile->fileName()+" is not in the correct XML format!");
        }
    }else{
        LogFile::writeToLog("HkxXmlReader: parse() failed!\nThe file "+hkxXmlFile->fileName()+" failed to open!");
    }
    return false;
}

int HkxXmlReader::readElementTag(const QByteArray & line, int startIndex, bool isIsolatedEndElemTag, bool isEndTag){
    QByteArray tag(AVERAGE_ELEMENT_TAG_LENGTH, '\0');
    auto index = 0;
    if (startIndex >= 0){
        for (; startIndex < line.size() && line.at(startIndex) != ' ' && line.at(startIndex) != '>'; startIndex++, index++){
            if (index >= tag.size()){
                tag.append(QByteArray(tag.size() * 2, '\0'));
            }
            tag[index] = line.at(startIndex);
        }
        tag.truncate(index);
        if (isIsolatedEndElemTag){
            if (!elementList.isEmpty() && !(indexOfElemTags.isEmpty() || indexOfElemTags.last() >= elementList.size())){
                if (!elementList.at(indexOfElemTags.last()).isContainedOnOneLine){
                    if (!qstrcmp(elementList.at(indexOfElemTags.last()).name.constData(), tag.constData()) && (!indexOfElemTags.isEmpty())){
                        indexOfElemTags.removeLast();
                    }else{
                        return OrphanedElementTag;
                    }
                }
            }else{
                return UnknownError;
            }
        }else if (!isEndTag){
            elementList.append(Element(tag));
        }
        if (startIndex < line.size()){
            if (line.at(startIndex) == '>'){
                if (!isEndTag && startIndex < line.size() && (line.at(startIndex) == '\n' || line.at(startIndex) == '\r')){
                    indexOfElemTags.append(elementList.size() - 1);
                    elementList.last().isContainedOnOneLine = false;
                }
                return startIndex - 1;
            }else if (line.at(startIndex) == ' '){
                return startIndex - 1;
            }
        }
    }
    return UnknownError;
}

int HkxXmlReader::readAttribute(const QByteArray & line, int startIndex){
    QByteArray attribute(AVERAGE_ATTRIBUTE_LENGTH, '\0');
    QByteArray value(AVERAGE_ATTRIBUTE_VALUE_LENGTH, '\0');
    auto readvalue = [&](QByteArray & string){
        auto index = 0;
        for (; startIndex < line.size(); startIndex++, index++){
            if (line.at(startIndex) == '=' || line.at(startIndex) == '\"'){
                startIndex++;
                break;
            }else{
                if (index >= string.size()){
                    string.append(QByteArray(string.size() * 2, '\0'));
                }
                string[index] = line.at(startIndex);
            }
        }
        string.truncate(index);
    };
    if (startIndex >= 0){
        readvalue(attribute);
        if (startIndex >= line.size() || line.at(startIndex) != '\"'){
            return MalformedAttribute;
        }else if (elementList.isEmpty()){
            return OrphanedAttribute;
        }
        elementList.last().attributeList.append(attribute);
        ++startIndex;
        readvalue(value);
        elementList.last().attributeList.last().value = value;
        if (startIndex < line.size()){
            if (line.at(startIndex) == '>'){
                return startIndex - 1;
            }else if (line.at(startIndex) == ' '){
                return startIndex - 1;
            }
        }
    }
    return startIndex;
}

int HkxXmlReader::readValue(const QByteArray & line, int startIndex, bool isValueSplitOnMultipleLines){
    auto index = 0;
    QByteArray value(AVERAGE_VALUE_LENGTH, '\0');
    if (!elementList.isEmpty() && startIndex >= 0 && startIndex < line.size()){
        if (line.at(startIndex) == '\n' || line.at(startIndex) == '\r'){    //Fucking carriage return character...
            indexOfElemTags.append(elementList.size() - 1);
            elementList.last().isContainedOnOneLine = false;
            return startIndex;
        }else if (line.at(startIndex) == '<'){
            return startIndex - 1;
        }
        for (; startIndex < line.size(); startIndex++, index++){
            if (line.at(startIndex) == '<'){    //Need to deal with embedded comments...FFS
                if (++startIndex < line.size()){
                    if (line.at(startIndex) == '!'){
                        startIndex = skipComment(line, startIndex + 1);
                        if (startIndex < 0){
                            return UnknownError;
                        }
                    }else{
                        break;
                    }
                }else{
                    return InvalidElementValue;
                }
            }else if (line.at(startIndex) == '\n' || line.at(startIndex) == '\r'){
                if (elementList.isEmpty()){
                    return UnknownError;
                }
                value.truncate(index);
                if (isValueSplitOnMultipleLines){
                    elementList.last().value = elementList.last().value.append(' '+QByteArray(value.constData()));
                }
                return startIndex + 1;
            }
            if (index >= value.size()){
                value.append(QByteArray(value.size() * 2, '\0'));
            }
            value[index] = line.at(startIndex);
        }
        value.truncate(index);
        if (!elementList.isEmpty()){
            elementList.last().value = value;
            return startIndex - 1;
        }
    }
    return UnknownError;
}

int HkxXmlReader::skipComment(const QByteArray & line, int index){
    for (auto j = 0; index < line.size(), j < 2; index++, j++){
        if (line.at(index) != '-'){
            return MalformedComment;
        }
    }
    for (; index < line.size(), line.at(index) != '-'; index++);
    if (index >= line.size()){
        return MalformedComment;
    }
    for (auto j = 0; index < line.size(), j < 2; index++, j++){
        if (line.at(index) != '-'){
            return MalformedComment;
        }
    }
    if (line.at(index) != '>'){
        return MalformedComment;
    }
    return index + 1;
}

HkxXmlReader::HkxXmlParseLine HkxXmlReader::readNextLine(){
    auto line = hkxXmlFile->readLine(MAX_HKXXML_LINE_LENGTH);
    lineNumber++;
    if (line.isEmpty()){
        isEOF = true;
        return EmptyLineEndFile;
    }else if (line.size() == 1){
        if (line.at(0) != '\n'){
            LogFile::writeToLog("HkxXmlReader: readNextLine() failed because an orphaned character was found on line "+QString::number(lineNumber)+"!");
            return OrphanedCharacter;
        }else{
            return EmptyLine;
        }
    }
    auto isIsolatedEndElemTag = true;
    auto isValueSplitOnMultipleLines = true;
    auto temp = 0;
    for (auto i = 0; i < line.size(); i++){
        if (line.at(i) == '<'){
            isValueSplitOnMultipleLines = false;
            if (++i < line.size()){
                if (line.at(i) == '/'){
                    i = readElementTag(line, i + 1, isIsolatedEndElemTag, true);
                }else if (line.at(i) == '!'){
                    i = skipComment(line, i + 1);
                }else{
                    isIsolatedEndElemTag = false;
                    i = readElementTag(line, i, isIsolatedEndElemTag, false);
                }
                if (i < 0){
                    return UnknownError;
                }
            }
        }else if (line.at(i) == '\t'){
            //continue...
        }else if (line.at(i) == '\n' || line.at(i) == '\r'){
            return NoError;
        }else if (line.at(i) == '>'){
            i = readValue(line, i + 1, false);
        }else if (line.at(i) == ' '){
            temp = i - 1;
            if (temp >= 0 && line.at(temp) == '>'){
                i = readValue(line, i + 1, false);
            }else{
                if (++i < line.size()){
                    i = readAttribute(line, i);
                }
            }
        }else{
            i = readValue(line, i, isValueSplitOnMultipleLines);
        }
        if (i < 0){
            return UnknownError;
        }
    }
    return NoError;
}

int HkxXmlReader::getNumAttributesAt(int index) const{
    if (index >= 0 && index < elementList.size()){
        return elementList.at(index).attributeList.size();
    }else{
        return 0;
    }
}

QByteArray HkxXmlReader::getElementNameAt(int index) const{
    if (index >= 0 && index < elementList.size()){
        return elementList.at(index).name;
    }else{
        return "";
    }
}

QByteArray HkxXmlReader::getElementValueAt(int index) const{
    if (index >= 0 && index < elementList.size()){
        return elementList.at(index).value;
    }else{
        return "";
    }
}

QByteArray HkxXmlReader::getNthAttributeNameAt(int index, int nth) const{
    if (index >= 0 && index < elementList.size() && nth < elementList.at(index).attributeList.size()){
        return elementList.at(index).attributeList.at(nth).name;
    }else{
        return "";
    }
}

QByteArray HkxXmlReader::getNthAttributeValueAt(int index, int nth) const{
    if (index < 0 || nth < 0){
        return "";
    }
    if (index < elementList.size() && nth < elementList.at(index).attributeList.size()){
        return elementList.at(index).attributeList.at(nth).value;
    }else{
        return "";
    }
}

QByteArray HkxXmlReader::findFirstValueWithAttributeValue(const QString &attributevalue) const{
    for (auto i = 0; i < elementList.size(); i++){
        if (!elementList.at(i).attributeList.isEmpty()){
            for (auto j = 0; j < elementList.at(i).attributeList.size(); j++){
                if (elementList.at(i).attributeList.at(j).value == attributevalue){
                    return elementList.at(i).value;
                }
            }
        }
    }
    return "";
}

void HkxXmlReader::setFile(HkxFile *file){
    hkxXmlFile = file;
}

int HkxXmlReader::getNumElements() const{
    return elementList.size();
}

void HkxXmlReader::clear(){
    elementList.clear();
}
