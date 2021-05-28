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

#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <KQuickAddons/ManagedConfigModule>

class AvailableTranslationsModel;
class SelectedTranslationsModel;
class TranslationsModel;
class TranslationsSettings;
class TranslateTool; 

class Translations : public KQuickAddons::ManagedConfigModule
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel* translationsModel READ translationsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* selectedTranslationsModel READ selectedTranslationsModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* availableTranslationsModel READ availableTranslationsModel CONSTANT)
    Q_PROPERTY(bool everSaved READ everSaved NOTIFY everSavedChanged)
    // Q_PROPERTY(TranslateTool* translateTool, READ translateTool, WRITE setTranslateTool)

    public:
        explicit Translations(QObject* parent = nullptr, const QVariantList &list = QVariantList());
        ~Translations() override;

        QAbstractItemModel* translationsModel() const;
        QAbstractItemModel* selectedTranslationsModel() const;
        QAbstractItemModel* availableTranslationsModel() const;

        bool everSaved() const;
        // TranslateTool *translateTool() {
        //     return m_translateTool;
        // }
        // void setTranslateTool(TranslateTool *translateTool) {
        //     m_translateTool = translateTool;
        //     connect(m_translateTool, &TranslateTool::dlgLanguageChanged,
        //     this, &Translations::onLanguageChanged);
        // }

    public Q_SLOTS:
        Q_INVOKABLE void load() override;
        Q_INVOKABLE void  save() override;
        void defaults() override;
        void onLanguageChanged(bool result);

    Q_SIGNALS:
        void everSavedChanged() const;

    private Q_SLOTS:
        void selectedLanguagesChanged();
       

    private:
        bool isSaveNeeded() const override;

        TranslationsSettings *m_settings;
        TranslationsModel *m_translationsModel;
        SelectedTranslationsModel *m_selectedTranslationsModel;
        AvailableTranslationsModel *m_availableTranslationsModel;
        TranslateTool *m_translateTool;

        bool m_everSaved;
};

#endif
