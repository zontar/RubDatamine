#include <QDateTime>
#include <QString>

struct UserInfo
{
    int id;
    QString nickname;
    QDateTime createtime;
    int type;
    int views;
    int likesput;
    int likesgot;
    int posts;
    double ppd;
    QDateTime lastactivity;
};

struct SectionInfo
{
    int id;
    QString name;
    int parent;
    int threads;
};

struct ThreadInfo
{
    int id;
    QString name;
    int section;
    int author;
    QDateTime create;
    int posts;
};

struct PostInfo
{
    int id;
    int author;
    int thread;
    QDateTime create;
    int likes;
    QString text;
};
