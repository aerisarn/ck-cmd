#pragma once

#include <QSettings>

class GlobalConfig {
public:
    static GlobalConfig& GetInstance() {
        static GlobalConfig singleton;
        return singleton;
    }

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

    static const int version = 1;

private:

    QSettings settings;

    void Create();

    template <int to>
    void Update(int from) {}

    GlobalConfig() : settings("Settings.ini", QSettings::IniFormat) 
    {
        if (!settings.contains("general/version"))
            Create();
        
        if (settings.value("general/version").toInt() != 1)
            Update<version>(settings.value("general/version").toInt());
    }

    // Delete copy/move so extra instances can't be created/moved.
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
    GlobalConfig(GlobalConfig&&) = delete;
    GlobalConfig& operator=(GlobalConfig&&) = delete;
};

#define Settings GlobalConfig::GetInstance()