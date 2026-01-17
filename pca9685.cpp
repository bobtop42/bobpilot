#include "pca9685.h"

int PCA::wakeSequence()
{
    uint8_t mode1[1] = {0x00};
    uint8_t buf[2] = {0x00, 0x00};

    if (write(fd, mode1, 1) != 1)
    {
        close(fd);
        return -1;
    }
    if (read(fd, mode1, 1) != 1)
    {
        close(fd);
        return -1;
    }

    if ((mode1[0] & 0x80) == 0x80)
    {
        buf[1] = mode1[0] | 0x80;
        if (write(fd, buf, 2) != 2)
        {
            close(fd);
            return -1;
        }
        usleep(5000);
        return 0;
    }
    if ((mode1[0] & 0x10) == 0x10)
    {
        buf[1] = mode1[0] | 0x10;
        if (write(fd, buf, 2) != 2)
        {
            close(fd);
            return -1;
        }
        usleep(5000);
        return 0;
    }
    else if ((mode1[0] & 0x90) == 0x00)
    {
        return 0;
    }
    return -1;
}

auto PCA::radianToStep(float radian)
{
    radian = ((1.570795f - radian) / 6.2831f) * 8192.0f;
    int val = static_cast<int>(radian);
    uint8_t L = static_cast<uint8_t>(val & 0xFF);
    uint8_t H = static_cast<uint8_t>((val >> 8) & 0xFF);
    return std::make_tuple(L, H);
}

int PCA::writeReg(uint8_t reg, float radian)
{
    auto [L, H] = radianToStep(radian);
    uint8_t buf[5] = {reg, 0x0, 0x0, L, H};
    if (write(fd, buf, 5) != 5) return -3;
    return 0;
}

int PCA::toggleAutoIncrament(uint8_t togglesettings)
{
    wakeSequence();
    uint8_t mode1[1] = {0x00};
    uint8_t buf[2] = {0x00, 0x00};

    if (write(fd, mode1, 1) != 1)
    {
        return -1;
    }
    if (read(fd, mode1, 1) != 1)
    {
        return -1;
    }

    buf[1] = (mode1[0] ^ 0x20) | togglesettings;

    if (write(fd, buf, 2) != 2)
    {
        return -1;
    }
    return 0;
}

int PCA::setUp()
{
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        return -1;
    }
    if (ioctl(fd, I2C_SLAVE, PCA_ADDRESS) != 0)
    {
        close(fd);
        return -1;
    }
    return 0;
}

PCA::PCA()
{
    PCASHUTDOWNERROR = false;
}

void PCA::updateServos(PLANE* plane, ATC* atc)
{
    bool servoUpdated = false;

    while (!servoUpdated)
    {
        uint8_t catchbuf[1];

        try
        {
            int wake = wakeSequence();
            if (wake != 0){throw wake;}

            if (writeReg(0x06, plane->ap.flap.fL) != 0){throw -1;}/*AILERONS: IN 1*/
            if (writeReg(0x0A, plane->ep.flap.fL) != 0){throw -1;}/*ELEVATORS: PIN 2*/
            if (writeReg(0x0E, plane->fp.flap.fL) != 0){throw -1;}/*LANDING FLAPS: PIN 3*/
            if (writeReg(0x12, plane->rp.flap.fL) != 0){throw -1;}/*RUDDER: PIN 4*/
            if (writeReg(0x42, (static_cast<uint16_t>(atc->speed_))) != 0){throw -1;} /*THROTTLE: PIN 16 (LAST PIN)*/

            servoUpdated = true;
        }
        catch (int error)
        {
            switch (error)
            {
                case -1:
                {
                    catchbuf[0] = 0x00;

                    if (write(fd, catchbuf, 1) != 1)
                    {
                        SHUTDOWNERROR = true;
                        servoUpdated = false;
                        break;
                    }
                    if (read(fd, catchbuf, 1) != 1)
                    {
                        PCASHUTDOWNERROR = true;
                        servoUpdated = false;
                        break;
                    }
                    else
                    {
                        catchbuf[0] |= 0x80;
                        if (write(fd, catchbuf, 1) != 1)
                        {
                            PCASHUTDOWNERROR = true;
                            servoUpdated = false;
                            break;
                        }
                        else
                        {
                            close(fd);
                        }
                    }
                }
                case -2:
                {
                    PCASHUTDOWNERROR = true;
                    break;
                }
            }
        }
    }
}
