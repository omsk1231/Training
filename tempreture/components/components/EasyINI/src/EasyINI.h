#pragma once

#include <FS.h>

#define EINI_BUF_LEN 80
#define EINI_DEF_NUM 0
#define EINI_DEF_STR ""
#define EINI_ROW_SEP '\n'
#define EINI_VAL_SEP '='

#define PRINT_TO_FILE \
    fp.print(name);   \
    fp.print('=');    \
    fp.println(val);

// File open mode: Open or Write
enum EasyIniFileMode
{
    Read = 1,
    Write = 2,
};

// Utility class for reading/writing to .ini files on ESP's SPIFFS
class EasyINI
{
private:
    File fp;
    const char *fileName;
    // Reads a line from file, if line starts with <name>= method returns substring after '='
    String ProcLineString(const char *);
    // Reads a line from file, if line starts with <name>= method returns pointer to char after '='
    char *ProcLine(const char *, char *, size_t);

public:
    // Constructor. Example: EasyINI ei("/config.ini");
    EasyINI(const char *);

    // Open file in requested mode.
    bool Open(EasyIniFileMode);
    // Is file opened or not.
    bool Opened() { return fp; }
    // Close the file.
    void Close() { fp.close(); }
    // Size of the file.
    size_t Size() const { return fp.size(); }

    // Read int (bool) value.
    int GetInt(const char *, int = EINI_DEF_NUM);
    // Read float value.
    float GetFloat(const char *, float = EINI_DEF_NUM);
    // Read text (String class) value.
    String GetString(const char *, String = EINI_DEF_STR);
    // Read text (char *) value.
    char *GetCharArray(const char *name);
    // Returns complete contents of the file.
    String GetAll();

    // Write int value.
    void SetInt(const char *, int);
    // Write float value.
    void SetFloat(const char *, float);
    // Write string value.
    void SetString(const char *, const char *);
};