#ifndef DEPLIEURSCENE_H
#define DEPLIEURSCENE_H

#include <QGraphicsScene>

class DeplieurScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DeplieurScene(QObject *parent = nullptr);

signals:
    void changeCouleur(int);
};

#endif // DEPLIEURSCENE_H
