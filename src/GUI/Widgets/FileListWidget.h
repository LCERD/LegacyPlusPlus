//
// Created by DexrnZacAttack on 9/7/25 using zPc-i2.
//
#ifndef LEGACYPLUSPLUS_FILELISTWIDGET_H
#define LEGACYPLUSPLUS_FILELISTWIDGET_H
#include <QTreeWidget>

#include "LCE/archive/Archive.h"
#include "LCE/filesystem/File.h"
#include "IO/FileType.h"
#include "LCE/save/SaveFileOld.h"
#include "LCE/soundbank/Soundbank.h"

namespace LPP::GUI::Widgets {
    class FileListWidget : public QTreeWidget {
        Q_OBJECT
    public:
        explicit FileListWidget(IO::FileType type, QWidget *parent = nullptr);

        void create(lce::fs::Filesystem *fs);

        void createFromPhysical(QString *path, lce::fs::Filesystem *fs);

        void exportObject(const lce::fs::FSObject *o);

        void addObject(const std::wstring &n, lce::fs::FSObject &o);

        void refreshItems();

        IO::FileType getFileType() const;

        lce::fs::Filesystem *getFilesystem() const;

        QString *getPath() const;
    private:
        // what the fuck is QMap, why not use stl map?
        QMap<QString, QTreeWidgetItem*> mPaths;
        lce::fs::Filesystem *mFs;
        IO::FileType mType;
        QString *mCurrentPath = nullptr;

    private slots:
        void onFileDoubleClicked(const QTreeWidgetItem *item);
        void onContextMenuShow(const QPoint &pos);

        // Context Menu
        void onExportObject(const QTreeWidgetItem *item);
        void onExportObjects(const QList<QTreeWidgetItem *> &items);
        void onDeleteObjects(const QList<QTreeWidgetItem *> &items);

        void onSaveButtonClicked();

    public slots:
        // Toolbar
        void onAddFile();
    };
}

#endif //LEGACYPLUSPLUS_FILELISTWIDGET_H
