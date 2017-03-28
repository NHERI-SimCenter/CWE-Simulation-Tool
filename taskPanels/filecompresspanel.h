#ifndef FILECOMPRESSPANEL_H
#define FILECOMPRESSPANEL_H

#include "taskpanelentry.h"

#include <QModelIndex>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonValue>

class FileMetaData;
class FileTreeModelReader;
class RemoteDataInterface;
enum class RequestState;

class FileCompressPanel : public TaskPanelEntry
{
    Q_OBJECT
public:
    FileCompressPanel(RemoteDataInterface * newDataHandle, FileTreeModelReader * newReader, QObject *parent = 0);

    virtual void setupOwnFrame();
    virtual void frameNowVisible();
    virtual void frameNowInvisible();

private slots:
    void selectedFileChanged(FileMetaData * newSelection);
    void compressSelected();
    void decompressSelected();

    void finishedFileCompress(RequestState finalState, QJsonDocument * rawData);
    void finishedFileExtract(RequestState finalState, QJsonDocument * rawData);

private:
    QModelIndex currentFileSelected;
    FileTreeModelReader * myTreeReader;

    RemoteDataInterface * dataConnection;

    QLabel * contentLabel = NULL;
    QPushButton * compressButton;
    QPushButton * decompressButton;
};

#endif // FILECOMPRESSPANEL_H
