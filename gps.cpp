#include "gps.h"

void GPS::openSerialFD()
{
    fd = open("/dev/ttyAMA5", O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        std::cerr << "ERROR: failed to open fd\n";
        SHUTDOWNERROR = true;
    }
}

bool GPS::configureSerialPort()
{
    speed_t baudRate = 9600;
    termios tty{};

    if (tcgetattr(fd, &tty) != 0)
    {
        perror("error 1 from tcgetattr");
        SHUTDOWNERROR = true;
        close(fd);
        return false;
    }

    cfsetispeed(&tty, baudRate);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(ICRNL | INLCR);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("error 2 from tcsetattr");
        SHUTDOWNERROR = true;
        close(fd);
        return false;
    }
    return true;
}

void GPS::markPunctuationPositions(const std::string gpsmsg)
{
    int i = 0;
    int pc = 1;
    int gpsmsglen = gpsmsg.length();

    for (int i2 = 0; i < gpsmsglen; i2++)
    {
        if (gpsmsg[i2] == '$')
        {
            puncLocation[0] = i2;
            i = i2;
            break;
        }
    }

    i++;

    while (gpsmsg[i] != '\r')
    {
        puncLocation[pc] = i;
        pc = pc + !(gpsmsg[i] - ',') + !(gpsmsg[i] - '*') + !(gpsmsg[i] - '.') + !(gpsmsg[i] - '-');
        i++;
    }
}

auto GPS::parseGPGGA(const std::string gpsmsg, int puncLocation[67])
{
    int hours;
    int minutes;
    int seconds;

    float gpsLat;
    char latHemisphere;
    float gpsLong;
    char longHemisphere;

    int measureAccuracy;
    int numSatellites;

    int hdop;
    float altitude;
    float geoidalSep;

    if (gpsmsg.substr(puncLocation[0] + 1, 6) != "GPGGA")
    {
        goodRead = false;
        return std::make_tuple(hours, minutes, seconds, gpsLat, latHemisphere,
                               gpsLong, longHemisphere, measureAccuracy,
                               altitude, numSatellites, hdop, false);
    }
    else
    {
        goodRead = true;
    }

    int j = 0;
    while (gpsmsg[j] != '\0')
    {
        std::cout << gpsmsg[j];
        j++;
    }
    std::cout << "\n";

    int i = 1;
    hours = std::stoi(gpsmsg.substr(puncLocation[i] + 1, 2));
    minutes = std::stoi(gpsmsg.substr(puncLocation[i] + 3, 2));
    seconds = std::stoi(gpsmsg.substr(puncLocation[i] + 5, 2));

    i += 2;
    int t = (!(gpsmsg[i + 1] - '-'));
    float latDegrees = std::stof(gpsmsg.substr(puncLocation[i] + 1, t + 2));
    i += 1 + t;

    float latMinutes = std::stof(gpsmsg.substr(puncLocation[i] - 2, 1 + puncLocation[i + 1] - puncLocation[i]));
    gpsLat = latDegrees + latMinutes / 60.0f;
    i++;
    latHemisphere = gpsmsg[puncLocation[i] + 1];

    t = (!gpsmsg[i + 1] - '-');
    float longDegrees = std::stoi(gpsmsg.substr(puncLocation[i] + 1, 3 + t));
    i += 1 + t;

    float longMinutes = std::stof(gpsmsg.substr(puncLocation[i] - 2, 1 + puncLocation[i + 1] - puncLocation[i]));
    gpsLong = longDegrees + longMinutes / 60.0f;
    i++;
    longHemisphere = gpsmsg[puncLocation[i] + 1];
    i++;
    measureAccuracy = std::stoi(gpsmsg.substr(puncLocation[i] + 1, 1));
    i++;
    numSatellites = std::stoi(gpsmsg.substr(puncLocation[1 + i] - puncLocation[i]));
    i++;
    hdop = std::stoi(gpsmsg.substr(puncLocation[i], puncLocation[i + 1] - puncLocation[i]));
    i++;
    altitude = std::stof(gpsmsg.substr(puncLocation[i], 1 + puncLocation[i + 1] - puncLocation[i]));
    i++;
    char altitudeUnit = gpsmsg[puncLocation[i] + 1];
    i++;

    if (toupper(altitudeUnit) == 'M')
    {
        altitude = toFeet(altitude);
    }

    geoidalSep = std::stof(gpsmsg.substr(puncLocation[i], 1 + puncLocation[i + 1] - puncLocation[i]));
    char geoidalUnit = gpsmsg[puncLocation[i] + 1];

    if (toupper(geoidalUnit) == 'M')
    {
        altitude -= toFeet(geoidalSep);
    }
    else
    {
        altitude -= geoidalSep;
    }

    return std::make_tuple(hours, minutes, seconds, gpsLat, latHemisphere,
                           gpsLong, longHemisphere, measureAccuracy,
                           altitude, numSatellites, hdop, true);
}

void GPS::processGPSMessage(PLANE* plane, const std::string gpsmsg)
{
    this->markPunctuationPositions(gpsmsg);

    auto [hrs, min, sec, lat, hem1, Long, hem2, Acc, Alt, numSat, hdop, parseGPGGAdataFlag] =
        parseGPGGA(gpsmsg, puncLocation);

    if (parseGPGGAdataFlag)
    {
        plane->updateGPS(lat, Alt, Long, hem1, hem2, Acc, hrs, min, sec, numSat, hdop);
        goodRead = true;
    }
    else
    {
        goodRead = false;
    }
}

void GPS::readAndParseGPS(PLANE* plane, int tries)
{
    char buf[256];
    std::string sentence = "";

    int n = read(fd, buf, sizeof(buf));

    if (n > 0)
    {
        for (int i = 0; i < n; ++i)
        {
            char ch = buf[i];
            if (ch == '\n')
            {
                break;
            }
            else if (ch != '\n')
            {
                sentence += ch;
            }
        }
    }
    else
    {
        GPGGA = false;
    }

    if (!sentence.empty() && sentence.find("$GPGGA") != std::string::npos)
    {
        processGPSMessage(plane, sentence);
    }

    sentence.clear();
    GPGGA = goodRead;
}

void GPS::refreshGPSData(PLANE* plane)
{
    int tries = 0;

    while (tries < 3)
    {
        readAndParseGPS(plane, tries);
        if (goodRead)
            break;
        else
            tries++;
    }

    if (!GPGGA)
        SHUTDOWNERROR = true;
}
