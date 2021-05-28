/*
 *  Copyright (C) 2014 John Layt <john@layt.net>
 *  Copyright (C) 2018 Eike Hein <hein@kde.org>
 *  Copyright (C) 2019 Kevin Ottens <kevin.ottens@enioka.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "translations.h"
#include "translationsmodel.h"
#include "translationssettings.h"
#include "translatetool.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>

K_PLUGIN_CLASS_WITH_JSON(Translations, "kcm_translations.json")

Translations::Translations(QObject *parent, const QVariantList &args)
    : KQuickAddons::ManagedConfigModule(parent, args)
    , m_settings(new TranslationsSettings(this))
    , m_translationsModel(new TranslationsModel(this))
    , m_selectedTranslationsModel(new SelectedTranslationsModel(this))
    , m_availableTranslationsModel(new AvailableTranslationsModel(this))
    , m_everSaved(false)
{

    qmlRegisterType<TranslateTool>("org.jingos.settings.language", 1, 0,  "TranslateTool");
                    //    qmlRegisterSingletonType<SingletonTypeExample>("Qt.example.qobjectSingleton", 1, 0, "MyApi", example_qobject_singletontype_provider);

    // int registResult = qmlRegisterSingletonType<TranslateTool>("org.jingos.settings.language", 1, 0, "TranslateTool", m_translateTool);
    // qDebug() << "注册结果111" << registResult;

    KAboutData *about = new KAboutData(QStringLiteral("kcm_translations"),
        i18n("Configure Plasma translations"),
        QStringLiteral("2.0"), QString(), KAboutLicense::LGPL);
    setAboutData(about);

    setButtons(Apply | Default);

    connect(m_selectedTranslationsModel, &SelectedTranslationsModel::selectedLanguagesChanged,
            this, &Translations::selectedLanguagesChanged);
    connect(m_selectedTranslationsModel, &SelectedTranslationsModel::selectedLanguagesChanged,
            m_availableTranslationsModel, &AvailableTranslationsModel::setSelectedLanguages);

    // auto res = connect(m_translateTool, &TranslateTool::dlgLanguageChanged,
    //         this, &Translations::onLanguageChanged);
    // qDebug() << "Translations " << "connect res" << res;
}


Translations::~Translations()
{
    qDebug() << "Translations::~Translations()";
}

QAbstractItemModel* Translations::translationsModel() const
{
    return m_translationsModel;
}

QAbstractItemModel* Translations::selectedTranslationsModel() const
{
    return m_selectedTranslationsModel;
}

QAbstractItemModel* Translations::availableTranslationsModel() const
{
    return m_availableTranslationsModel;
}

bool Translations::everSaved() const
{
    return m_everSaved;
}

void Translations::onLanguageChanged(bool result){
    qDebug() << "Translations::onLanguageChanged ::::::::::::" << result ; 
    if(result){
        save();
    }else {
        load();
    }
}

void Translations::load()
{
    qDebug() << "Translations::load :::::::::::: start"  ; 
    KQuickAddons::ManagedConfigModule::load();
    m_availableTranslationsModel->setSelectedLanguages(m_settings->configuredLanguages());
    m_selectedTranslationsModel->setSelectedLanguages(m_settings->configuredLanguages());
}

void Translations::save()
{
    qDebug() << "Translations::save :::::::::::: start"  ; 
    m_everSaved = true;
    emit everSavedChanged();
    KQuickAddons::ManagedConfigModule::save();
}

void Translations::defaults()
{
    KQuickAddons::ManagedConfigModule::defaults();
    m_availableTranslationsModel->setSelectedLanguages(m_settings->configuredLanguages());
    m_selectedTranslationsModel->setSelectedLanguages(m_settings->configuredLanguages());
}

void Translations::selectedLanguagesChanged()
{
    auto configuredLanguages = m_selectedTranslationsModel->selectedLanguages();

    const auto missingLanguages = m_selectedTranslationsModel->missingLanguages();
    for (const auto &lang : missingLanguages) {
        configuredLanguages.removeOne(lang);
    }

    m_settings->setConfiguredLanguages(configuredLanguages);
    m_selectedTranslationsModel->setSelectedLanguages(configuredLanguages);
}

bool Translations::isSaveNeeded() const
{
    return !m_selectedTranslationsModel->missingLanguages().isEmpty();
}

#include "translations.moc"
