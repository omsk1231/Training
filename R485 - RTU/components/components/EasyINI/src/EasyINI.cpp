#include <EasyINI.h>
#include <SPIFFS.h>
EasyINI::EasyINI(const char *fileName)
{
    SPIFFS.begin();
    this->fileName = fileName;
}

bool EasyINI::Open(EasyIniFileMode mode)
{
    fp = SPIFFS.open(fileName, mode == EasyIniFileMode::Read ? "r" : "w");
    if (!fp)
        Serial.println("Where's the file?");
    return fp;
}

char *EasyINI::ProcLine(const char *name, char *buf, size_t n)
{
    size_t nBuf = fp.readBytesUntil(EINI_ROW_SEP, buf, EINI_BUF_LEN);
    if (strncmp(name, buf, n) == 0) // red pocinje trazenim nazivom (name)
    {
        uint i = n;
        while (isWhitespace(buf[i])) // ukidaju se sve beline do znaka '='
            i++;
        if (buf[i++] == EINI_VAL_SEP)
        {
            while (isWhitespace(buf[i])) // ukidaju se beline posle znaka '='
                i++;
            buf[nBuf] = '\0';
            return buf + i;
        }
    }
    return NULL;
}

int EasyINI::GetInt(const char *name, int def)
{
    size_t initPos = fp.position();
    size_t n = strlen(name);
    char buf[EINI_BUF_LEN];
    char *p;

    while (fp.available())
        if ((p = ProcLine(name, buf, n)))
            return atoi(p);
    fp.seek(0);
    while (fp.position() < initPos)
        if ((p = ProcLine(name, buf, n)) != NULL)
            return atoi(p);

    return def;
}

float EasyINI::GetFloat(const char *name, float def)
{
    size_t initPos = fp.position();
    size_t n = strlen(name);
    char buf[EINI_BUF_LEN];
    char *p;

    while (fp.available())
        if ((p = ProcLine(name, buf, n)))
            return atof(p);
    fp.seek(0);
    while (fp.position() < initPos)
        if ((p = ProcLine(name, buf, n)) != NULL)
            return atof(p);

    return def;
}

String EasyINI::ProcLineString(const char *name)
{
    String line = fp.readStringUntil(EINI_ROW_SEP);
    if (line.startsWith(name))
    {
        size_t n = strlen(name);
        uint i = n;
        while (isWhitespace(line[i])) // ukidaju se beline do znaka '='
            i++;
        if (line[i++] == EINI_VAL_SEP)
        {
            while (isWhitespace(line[i])) // ukidaju se beline posle znaka '='
                i++;
            return line.substring(i);
        }
    }
    return EINI_DEF_STR;
}

String EasyINI::GetString(const char *name, String def)
{
    size_t initPos = fp.position();
    String s;
    while (fp.available())
        if (!(s = ProcLineString(name)).isEmpty())
            return s;
    fp.seek(0);
    while (fp.position() < initPos)
        if (!(s = ProcLineString(name)).isEmpty())
            return s;
    return def;
}

char *EasyINI::GetCharArray(const char *name)
{
    size_t initPos = fp.position();
    size_t n = strlen(name);
    char *buf = (char *)malloc(EINI_BUF_LEN);
    char *p;

    while (fp.available())
        if ((p = ProcLine(name, buf, n)))
            return p;
    fp.seek(0);
    while (fp.position() < initPos)
        if ((p = ProcLine(name, buf, n)))
            return p;

    free(buf);
    return NULL;
}

String EasyINI::GetAll()
{
    fp.seek(0);
    return fp.readString();
}

void EasyINI::SetInt(const char *name, int val) { PRINT_TO_FILE }

void EasyINI::SetFloat(const char *name, float val) { PRINT_TO_FILE }

void EasyINI::SetString(const char *name, const char *val) { PRINT_TO_FILE }
