#ifndef SETTINGS_H
#define SETTINGS_H

enum Metrics {
    Centimeters,
    Meters,
    Inches,
    Feet
};

class Settings
{
public:
    static int tasks;
    static int metrics;
};

#endif // SETTINGS_H
