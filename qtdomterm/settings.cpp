/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settings.h"

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "webview.h"

#include <QtCore/QLocale>
#include <QtCore/QSettings>
#include <QtWidgets/QtWidgets>
#include <QtWebEngineWidgets/QtWebEngineWidgets>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    connect(setHomeToCurrentPageButton, SIGNAL(clicked()), this, SLOT(setHomeToCurrentPage()));
    connect(standardFontButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(fixedFontButton, SIGNAL(clicked()), this, SLOT(chooseFixedFont()));

    loadDefaults();
    loadFromSettings();
}

static QString defaultAcceptLanguage()
{
    const QStringList langs = QLocale().uiLanguages();
    if (langs.isEmpty())
        return QString();
    QString str = langs.first();
    const float qstep = 1.0f / float(langs.count());
    float q = 1.0f - qstep;
    for (int i = 1; i < langs.count(); ++i) {
        str += QStringLiteral(", ") + langs.at(i) + QStringLiteral(";q=") + QString::number(q, 'f', 2);
        q -= qstep;
    }
    return str;
}

void SettingsDialog::loadDefaults()
{
    QWebEngineSettings *defaultSettings = QWebEngineSettings::globalSettings();
    QString standardFontFamily = defaultSettings->fontFamily(QWebEngineSettings::StandardFont);
    int standardFontSize = defaultSettings->fontSize(QWebEngineSettings::DefaultFontSize);
    standardFont = QFont(standardFontFamily, standardFontSize);
    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(standardFont.family()).arg(standardFont.pointSize()));

    QString fixedFontFamily = defaultSettings->fontFamily(QWebEngineSettings::FixedFont);
    int fixedFontSize = defaultSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize);
    fixedFont = QFont(fixedFontFamily, fixedFontSize);
    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(fixedFont.family()).arg(fixedFont.pointSize()));

    enableJavascript->setChecked(defaultSettings->testAttribute(QWebEngineSettings::JavascriptEnabled));
    enablePlugins->setChecked(defaultSettings->testAttribute(QWebEngineSettings::PluginsEnabled));

    enableScrollAnimator->setChecked(defaultSettings->testAttribute(QWebEngineSettings::ScrollAnimatorEnabled));

    persistentDataPath->setText(QWebEngineProfile::defaultProfile()->persistentStoragePath());
    httpUserAgent->setText(QWebEngineProfile::defaultProfile()->httpUserAgent());
    httpAcceptLanguage->setText(defaultAcceptLanguage());
}

void SettingsDialog::loadFromSettings()
{
    QSettings settings;

    settings.beginGroup(QLatin1String("general"));
    openLinksIn->setCurrentIndex(settings.value(QLatin1String("openLinksIn"), openLinksIn->currentIndex()).toInt());

    settings.endGroup();

    // Appearance
    settings.beginGroup(QLatin1String("websettings"));
    fixedFont = qvariant_cast<QFont>(settings.value(QLatin1String("fixedFont"), fixedFont));
    standardFont = qvariant_cast<QFont>(settings.value(QLatin1String("standardFont"), standardFont));

    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(standardFont.family()).arg(standardFont.pointSize()));
    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(fixedFont.family()).arg(fixedFont.pointSize()));

    enableJavascript->setChecked(settings.value(QLatin1String("enableJavascript"), enableJavascript->isChecked()).toBool());
    enablePlugins->setChecked(settings.value(QLatin1String("enablePlugins"), enablePlugins->isChecked()).toBool());
    userStyleSheetFile->setText(settings.value(QLatin1String("userStyleSheetFile")).toString());
    userStyleSheetRules->setPlainText(settings.value(QLatin1String("userStyleSheetRules")).toString());
    enableScrollAnimator->setChecked(settings.value(QLatin1String("enableScrollAnimator"), enableScrollAnimator->isChecked()).toBool());
    httpUserAgent->setText(settings.value(QLatin1String("httpUserAgent"), httpUserAgent->text()).toString());
    httpAcceptLanguage->setText(settings.value(QLatin1String("httpAcceptLanguage"), httpAcceptLanguage->text()).toString());
    settings.endGroup();

    // Privacy
    settings.beginGroup(QLatin1String("cookies"));

    int persistentCookiesPolicy = settings.value(QLatin1String("persistentCookiesPolicy"), sessionCookiesCombo->currentIndex()).toInt();
    sessionCookiesCombo->setCurrentIndex(persistentCookiesPolicy);

    QString pdataPath = settings.value(QLatin1String("persistentDataPath"), persistentDataPath->text()).toString();
    persistentDataPath->setText(pdataPath);

    settings.endGroup();
}

void SettingsDialog::saveToSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("MainWindow"));
    settings.setValue(QLatin1String("home"), homeLineEdit->text());
    settings.endGroup();

    settings.beginGroup(QLatin1String("general"));
    settings.setValue(QLatin1String("openLinksIn"), openLinksIn->currentIndex());
    settings.endGroup();

    settings.beginGroup(QLatin1String("history"));
    int historyExpire = expireHistory->currentIndex();
    int idx = -1;
    switch (historyExpire) {
    case 0: idx = 1; break;
    case 1: idx = 7; break;
    case 2: idx = 14; break;
    case 3: idx = 30; break;
    case 4: idx = 365; break;
    case 5: idx = -1; break;
    }
    settings.setValue(QLatin1String("historyExpire"), idx);
    settings.endGroup();

    // Appearance
    settings.beginGroup(QLatin1String("websettings"));
    settings.setValue(QLatin1String("fixedFont"), fixedFont);
    settings.setValue(QLatin1String("standardFont"), standardFont);
    settings.setValue(QLatin1String("enableJavascript"), enableJavascript->isChecked());
    settings.setValue(QLatin1String("enablePlugins"), enablePlugins->isChecked());
    settings.setValue(QLatin1String("enableScrollAnimator"), enableScrollAnimator->isChecked());
    settings.setValue(QLatin1String("userStyleSheetFile"), userStyleSheetFile->text());
    settings.setValue(QLatin1String("userStyleSheetRules"), userStyleSheetRules->toPlainText());
    settings.setValue(QLatin1String("httpUserAgent"), httpUserAgent->text());
    settings.setValue(QLatin1String("httpAcceptLanguage"), httpAcceptLanguage->text());
    settings.endGroup();

    //Privacy
    settings.beginGroup(QLatin1String("cookies"));

    int persistentCookiesPolicy = sessionCookiesCombo->currentIndex();
    settings.setValue(QLatin1String("persistentCookiesPolicy"), persistentCookiesPolicy);

    QString pdataPath = persistentDataPath->text();
    settings.setValue(QLatin1String("persistentDataPath"), pdataPath);

    settings.endGroup();

    BrowserApplication::instance()->loadSettings();
}

void SettingsDialog::accept()
{
    saveToSettings();
    QDialog::accept();
}

void SettingsDialog::showExceptions()
{
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
    CookiesExceptionsDialog *dialog = new CookiesExceptionsDialog(BrowserApplication::cookieJar(), this);
    dialog->exec();
#endif
}

void SettingsDialog::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, standardFont, this);
    if ( ok ) {
        standardFont = font;
        standardLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}

void SettingsDialog::chooseFixedFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, fixedFont, this);
    if ( ok ) {
        fixedFont = font;
        fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
    }
}

void SettingsDialog::on_httpUserAgent_editingFinished()
{
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(httpUserAgent->text());
}

void SettingsDialog::on_httpAcceptLanguage_editingFinished()
{
    QWebEngineProfile::defaultProfile()->setHttpAcceptLanguage(httpAcceptLanguage->text());
}

void SettingsDialog::setHomeToCurrentPage()
{
    BrowserMainWindow *mw = static_cast<BrowserMainWindow*>(parent());
    WebView *webView = mw->currentTab();
    if (webView)
        homeLineEdit->setText(webView->url().toString());
}
