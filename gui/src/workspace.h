#pragma once

#include <QSettings>

#include <src/log.h>

class WorkspaceConfig {


    fs::path _path;
    QSettings settings;

    template <int to>
    void Update(int from) {}

public:

    static const int version = 1;

    void set(const QString& key, const QVariant& value) {
        settings.setValue(key, value);
    }

    template<typename T>
    T get(const QString& key) const {
        return settings.value(key).value<T>();
    }

    template<>
    std::string get(const QString& key) const {
        return settings.value(key).value<QString>().toUtf8().constData();
    }

    bool empty();

    void addCharacterProject(const QString& path, const QVariant& key);
    void addMiscellaneousProject(const QString& path, const QVariant& key);

    QStringList getCharacterProjects();
    QStringList getMiscellaneousProjects();

    WorkspaceConfig(const fs::path& path);

    const fs::path& getFolder();

};