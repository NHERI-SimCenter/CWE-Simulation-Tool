#include "filetreenode.h"

#include "../AgaveClientInterface/remotedatainterface.h"

FileTreeNode::FileTreeNode(FileMetaData contents, FileTreeNode * parent):QObject((QObject *)parent)
{
    fileData = new FileMetaData(contents);
    childList = new QList<FileTreeNode *>();

    if (parent == NULL)
    {
        rootNode = true;
    }
    else
    {
        rootNode = false;
        parent->childList->append(this);
    }
}

FileTreeNode::FileTreeNode(FileTreeNode * parent):QObject((QObject *)parent)
{
    childList = new QList<FileTreeNode *>();

    if (parent == NULL)
    {
        rootNode = true;

        //Create default root folder
        fileData = new FileMetaData();
        fileData->setFullFilePath("/");
        fileData->setType(FileType::DIR);
    }
    else
    {
        rootNode = false;
        parent->childList->append(this);

        //Create loading placeholder
        QString fullPath = parent->fileData->getFullPath();
        fullPath.append("/Loading");
        fileData = new FileMetaData();
        fileData->setFullFilePath(fullPath);
        fileData->setType(FileType::UNLOADED);
    }
}

FileTreeNode::~FileTreeNode()
{
    if (this->parent() != NULL)
    {
        FileTreeNode * parentNode = (FileTreeNode *)this->parent();
        if (parentNode->childList->contains(this))
        {
            parentNode->childList->removeAll(this);
        }
    }
    if (this->fileData != NULL)
    {
        delete this->fileData;
    }
    if (this->childList != NULL)
    {
        while (this->childList->size() > 0)
        {
            FileTreeNode * toDelete = this->childList->takeLast();
            delete toDelete;
        }
        delete this->childList;
    }
}

QList<FileTreeNode *>::iterator FileTreeNode::fileListStart()
{
    return childList->begin();
}

QList<FileTreeNode *>::iterator FileTreeNode::fileListEnd()
{
    return childList->end();
}

bool FileTreeNode::isRootNode()
{
    return rootNode;
}

void FileTreeNode::setFileData(FileMetaData newData)
{
    if (fileData != NULL)
    {
        delete fileData;
    }
    fileData = new FileMetaData(newData);
}

FileTreeNode * FileTreeNode::getParentNode()
{
    FileTreeNode * ret = (FileTreeNode *)this->parent();
    return ret;
}

FileTreeNode * FileTreeNode::getChildNodeWithName(QString filename, bool unrestricted)
{
    for (auto itr = this->childList->begin(); itr != this->childList->end(); itr++)
    {
        FileMetaData toSearch = (*itr)->getFileData();
        if ((unrestricted) || (toSearch.getFileType() == FileType::DIR) || (toSearch.getFileType() == FileType::FILE))
        {
            if (toSearch.getFileName() == filename)
            {
                return (*itr);
            }
        }
    }
    return NULL;
}

void FileTreeNode::clearAllChildren()
{
    while (childList->size() > 0)
    {
        FileTreeNode *toDestroy = childList->takeLast();
        toDestroy->deleteLater();
    }
}

FileMetaData FileTreeNode::getFileData()
{
    return *fileData;
}

bool FileTreeNode::childIsUnloaded()
{
    for (auto itr = childList->cbegin(); itr != childList->cend(); itr++)
    {
        if ((*itr)->getFileData().getFileType() == FileType::UNLOADED)
        {
            return true;
        }
    }
    return false;
}

void FileTreeNode::setMark(bool newSetting)
{
    marked = newSetting;
}

bool FileTreeNode::isMarked()
{
    return marked;
}
