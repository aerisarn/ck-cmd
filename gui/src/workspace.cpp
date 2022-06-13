#include "workspace.h"

using namespace ckcmd;

WorkspaceConfig::WorkspaceConfig(const fs::path & path) :
    _path(path),
    settings((_path / "Workspace.ini").string().c_str(), QSettings::IniFormat)
{
}

bool WorkspaceConfig::empty() { return settings.childGroups().isEmpty(); }

const fs::path& WorkspaceConfig::getFolder() {
    return _path;
}

void WorkspaceConfig::addCharacterProject(const QString& path, const QVariant& project_key)
{
    QString key = QString("Characters/%1").arg(path);
    set(key, project_key);
}

void WorkspaceConfig::addMiscellaneousProject(const QString& path, const QVariant& project_key)
{
    QString key = QString("Miscellaneous/%1").arg(path);
    set(key, project_key);
}

void WorkspaceConfig::addCrc32(const QString& crc32, const QVariant& project_key)
{
    QString key = QString("CRC32/%1").arg(crc32);
    set(key, project_key);
}

QStringList WorkspaceConfig::getCharacterProjects()
{
    settings.beginGroup("Characters");
    auto list = settings.allKeys();
    settings.endGroup();
    return list;
}

QStringList WorkspaceConfig::getMiscellaneousProjects()
{
    settings.beginGroup("Miscellaneous");
    auto list = settings.allKeys();
    settings.endGroup();
    return list;
}

QStringList WorkspaceConfig::getCRC32s()
{
    settings.beginGroup("CRC32");
    auto list = settings.allKeys();
    settings.endGroup();
    return list;
}

QString WorkspaceConfig::getCRC32(const QString& crc32)
{
    QString key = QString("CRC32/%1").arg(crc32);
    return settings.value(key).toString();
}