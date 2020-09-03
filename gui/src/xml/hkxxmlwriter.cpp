#include "hkxxmlwriter.h"
#include "src/filetypes/behaviorfile.h"

#include <QFile>
#include <QTextStream>

using namespace UI;

const QString HkxXMLWriter::version("1.0");
const QString HkxXMLWriter::encoding("ascii");
const QString HkxXMLWriter::classversion("8");
const QString HkxXMLWriter::contentsversion("hk_2010.2.0-r1");
const QString HkxXMLWriter::toplevelobject("#0001");
const QString HkxXMLWriter::filetype("hkpackfile");
const QString HkxXMLWriter::section("hksection");
const QString HkxXMLWriter::object("hkobject");
const QString HkxXMLWriter::parameter("hkparam");
const QString HkxXMLWriter::string("hkcstring");
const QString HkxXMLWriter::name("name");
const QString HkxXMLWriter::clas("class");
const QString HkxXMLWriter::signature("signature");
const QString HkxXMLWriter::numelements("numelements");

HkxXMLWriter::HkxXMLWriter(HkxFile *file)
    : hkxXmlFile(file),
      nestLevel(0)
{
    //
}

void HkxXMLWriter::setFile(HkxFile *file){
    hkxXmlFile = file;
}

bool HkxXMLWriter::writeToXMLFile(){
    auto result = false;
    if (hkxXmlFile){
        nestLevel = 0;
        newfile.setFileName(hkxXmlFile->fileName());
        stream.setDevice(&newfile);
        if (newfile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QStringList list1 = QStringList({"classversion", "contentsversion", "toplevelobject"});
            QStringList list2 = QStringList({classversion, contentsversion, hkxXmlFile->getRootObjectReferenceString()});
            writeHeader(version, encoding);
            writeLine(filetype, list1, list2, "");
            writeLine("\n");
            writeLine(section, QStringList(name), QStringList("__data__"), "");
            writeLine("\n");
            if (hkxXmlFile->getRootObject() && hkxXmlFile->getRootObject()->write(this)){
                writeLine(section, false);
                writeLine("\n");
                writeLine(filetype, false);
                result = true;
            }
        }else{
            LogFile::writeToLog("HkxXMLWriter: writeToXMLFile()!\nUnable to open a new file!!!");
        }
    }else{
        LogFile::writeToLog("HkxXMLWriter: writeToXMLFile()!\nUnable to open a new file!!!");
    }
    newfile.close();
    return result;
}

void HkxXMLWriter::writeHeader(const QString & version, const QString & encoding){
    stream << "<?xml version=\""+version+"\" encoding=\""+encoding+"\"?>\n";
}

bool HkxXMLWriter::writeLine(const QString & tag, const QStringList & attribs, const QStringList & attribValues, const QString & value, bool nullValueAllowed){
    QString text;
    auto ok = true;
    if (tag != "" && attribs.size() == attribValues.size()){
        for (auto i = 0; i < nestLevel; i++){
            text.append("\t");
        }
        text = text+"<"+tag;
        stream << text;
        for (auto j = 0; j < attribs.size(); j++){
            stream << " "+attribs.at(j)+"=\""+attribValues.at(j)+"\"";
        }
        if (value == ""){
            if (!attribValues.last().toInt(&ok) && attribValues.size() == 2 && ok){
                stream << "></"+tag+">\n";
            }else{
                if (nullValueAllowed){
                    stream << "></"+tag+">\n";
                }else{
                    nestLevel++;
                    stream << ">\n";
                }
            }
        }else{
            stream << ">"+value+"</"+tag+">\n";
        }
        return true;
    }else{
        LogFile::writeToLog("HkxXMLWriter: writeLine()!\nXML tag is null or the number of attributes does not match the number of attribute values!!!");
        return false;
    }
}

bool HkxXMLWriter::writeLine(const QString & tag, bool opening){
    QString text;
    auto appendtabs = [&](){
        for (auto i = 0; i < nestLevel; i++)
            text.append("\t");
    };
    if (tag != ""){
        if (opening){
            appendtabs();
            nestLevel++;
            stream << text+"<"+tag+">\n";
        }else{
            (nestLevel > 0) ? nestLevel-- : NULL;
            appendtabs();
            stream << text+"</"+tag+">\n";
        }
        return true;
    }else{
        LogFile::writeToLog("HkxXMLWriter: writeLine()!\nXML tag is null!!!");
        return false;
    }
}

bool HkxXMLWriter::writeLine(const QString & value){
    QString text;
    if (value != "\n"){
        for (auto i = 0; i < nestLevel; i++){
            text.append("\t");
        }
        auto list = value.split('\n', QString::SkipEmptyParts);
        if (!list.isEmpty()){
            for (auto i = 0; i < list.size(); i++){
                stream << text+list.at(i)+"\n";
            }
        }else{
            stream << text+value+"\n";
        }
    }else{
        stream << "\n";
    }
    return true;
}
