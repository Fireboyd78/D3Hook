#pragma once
#include "common.h"
#include <string>

//
// Hook configuration
//

class line_reader {
    HANDLE m_handle = nullptr;
    DWORD m_offset = 0;
    
    int m_line = 0;
    int m_index = -1;
    
    bool m_eof = false;

    char m_buffer[8192] = { NULL };
protected:
    DWORD fill_buffer();

    void prepare_buffer() {
        if (m_index == -1)
            fill_buffer();
    }
public:
    explicit line_reader(HANDLE handle)
        : m_handle(handle) {
        // read first line
        fill_buffer();
    }

    int line() {
        return m_line;
    }

    bool eof() {
        return m_eof || (m_index == -1 && fill_buffer() == 0);
    }

    bool can_read() {
        return !m_eof && (m_index > -1) && (m_index < sizeof(m_buffer));
    }

    std::string read_line();
    std::string read_line(bool(*predicate)(std::string &));
};

class HookConfig {
public:
    static bool Initialize(filename_t filename);

    // closes the config file
    static void Close();

    // reads from the config file (if open)
    static bool Read();

    static bool GetProperty(const char *key, char *value);
    static bool GetProperty(const char *key, bool &value);
    static bool GetProperty(const char *key, int &value);
    static bool GetProperty(const char *key, float &value);

    template <typename TType>
    static TType GetProperty(const char *key, TType defaultValue = TType()) {
        bool (*getter)(const char *, TType &) = GetProperty;

        TType result = defaultValue;

        return getter(key, result),
            result;
    }

    template <>
    static char * GetProperty<char *>(const char *key, char *defaultValue) {
        char *result = defaultValue;
        char buffer[MAX_PATH] { NULL };

        if (GetProperty(key, buffer)) {
            int len = strlen(buffer);

            // allocate a new string buffer
            result = new char[len] { NULL };
            strncpy_s(result, len, buffer, len);
        }

        return result;
    }

    // is flag property present and enabled?
    // e.g. 'UseHackyOverride=1'
    static bool IsFlagEnabled(const char *key) {
        bool result = false;

        return GetProperty(key, result),
            result;
    }
};

class ConfigProperty {
protected:
    const char *name;
public:
    explicit ConfigProperty(const char *propName);

    bool Get();
    bool Get(char *value);
    bool Get(bool &value);
    bool Get(int &value);
    bool Get(float &value);
};

class ConfigWatch : protected ConfigProperty {
private:
    ConfigWatch *next;
protected:
    void Register();

    virtual void Reload() PURE;
public:
    explicit ConfigWatch(const char *propName)
        : ConfigProperty(propName) {
        Register();
    };

    static void ReloadAll();
};

typedef void (*ConfigReloadedCallback)(void);

template <typename TType>
class ConfigValue : protected ConfigWatch {
private:
    ConfigReloadedCallback callback = nullptr;
protected:
    TType value = TType();
    bool parsed = false;

    void CheckValue() {
        // parse once
        if (!parsed) {
            parsed = ConfigProperty::Get(value);
            //LogFile::Format("**** Retrieved config value for '%s', status %d\n", name, parsed);
        }
    }

    void Reload() override {
        // force an update next time it's requested
        parsed = false;

        // optionally, inform the callback
        if (callback)
        {
            LogFile::Format("**** Calling reload callback for config value '%s'\n", name);
            callback();
        }
    }
public:
    explicit ConfigValue(const char *propName)
        : ConfigWatch(propName) { }

    explicit ConfigValue(const char *propName, TType defaultValue)
        : ConfigWatch(propName), value(defaultValue) { }

    inline void Install(ConfigReloadedCallback fnCallback)
    {
        callback = fnCallback;

        if (callback)
            callback();
    }

    inline bool Get(TType &value) {
        return ConfigProperty::Get(value);
    }

    inline TType & Get() {
        return CheckValue(),
            value;
    }

    inline operator ConfigValue<TType> &()          = delete;
    inline operator ConfigValue<TType> &() const    = delete;

    inline operator TType &() {
        return CheckValue(),
            value;
    }

    inline bool operator==(const TType &rhs) const  { return value == rhs; }
    inline bool operator!=(const TType &rhs) const  { return value != rhs; }
};

template <>
class ConfigValue<char *> : protected ConfigProperty {
    static_assert(true, "ConfigValue for strings not implemented yet");
};