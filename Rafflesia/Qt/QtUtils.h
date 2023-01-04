#pragma once

#include <QProgressBar>

class QSettings;

void updateBar(QProgressBar* bar, int curValue, int maxValue);
void updateBarFlatValue(QProgressBar* bar, int curValue, int maxValue);

const char* getAppName();
QSettings& getGlobalSettings(); 
QString getCharacterSettingsPath(const std::string& characterName);