#pragma once

#include <Preferences.h>

class AppData
{
protected:
  static AppData *_instance;

private:
  bool begined = false;

  Preferences preferences;
  AppData() = default;

public:
  static AppData *get()
  {
    if (_instance == nullptr)
    {
      _instance = new AppData();
    }

    return _instance;
  }

  static void destroy()
  {
    AppData::get()->end();
    delete _instance;
    _instance = nullptr;
  }

  static void begin()
  {
    AppData::get()->preferences.begin("InterTest", false);
    AppData::get()->begined = true;
  }

  static void end()
  {
    AppData::get()->preferences.end();
    AppData::get()->begined = false;
  }

  static bool isBegined()
  {
    return get()->begined;
  }

  static void flush()
  {
    end();
    begin();
  }

  static void saveInt(const char *key, int value)
  {
    AppData::get()->preferences.putInt(key, value);
  }

  static void saveUInt(const char *key, uint value)
  {
    AppData::get()->preferences.putUInt(key, value);
  }

  static bool exist(const char *key)
  {
    return get()->preferences.isKey(key);
  }

  static uint getUint(const char *key, uint32_t defaultVal = 0)
  {
    return AppData::get()->preferences.getUInt(key, defaultVal);
  }

  static int getInt(const char *key, int32_t defaultVal = 0)
  {
    return AppData::get()->preferences.getInt(key, defaultVal);
  }

  static void saveString(const char *key, String value)
  {
    AppData::get()->preferences.putString(key, value);
  }

  static String getString(const char *key, String defValue = "")
  {
    return AppData::get()->preferences.getString(key, defValue);
  }
  
  static void saveBool(const char *key, bool value)
  {
    AppData::get()->preferences.putBool(key, value);
  }

  static bool getBool(const char *key, bool defValue = false)
  {
    return AppData::get()->preferences.getBool(key, defValue);
  }


};

AppData *AppData::_instance = nullptr;