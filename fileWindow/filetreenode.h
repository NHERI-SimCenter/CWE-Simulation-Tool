#ifndef FILETREENODE_H
#define FILETREENODE_H

#include <QObject>
#include <QList>

class FileMetaData;

class FileTreeNode : public QObject
{
    Q_OBJECT
public:
    FileTreeNode(FileMetaData contents, FileTreeNode * parent = NULL);
    FileTreeNode(FileTreeNode * parent = NULL); //This creates either the default root folder, or default load pending,
                                                //depending if the parent is NULL
    ~FileTreeNode();

    QList<FileTreeNode *>::iterator fileListStart();
    QList<FileTreeNode *>::iterator fileListEnd();

    void setFileData(FileMetaData newData);
    void setMark(bool newSetting);
    bool isMarked();

    bool isRootNode();
    FileTreeNode * getParentNode();
    FileTreeNode * getChildNodeWithName(QString filename, bool unrestricted = false);
    void clearAllChildren();
    FileMetaData getFileData();

    bool childIsUnloaded();

private:
    FileMetaData * fileData = NULL;
    QList<FileTreeNode *> * childList = NULL;
    bool rootNode;
    bool marked = false;
};

#endif // FILETREENODE_H
