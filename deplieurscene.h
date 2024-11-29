#ifndef DEPLIEURSCENE_H
#define DEPLIEURSCENE_H

#include <QGraphicsScene>
#include <QTableWidget>

class DeplieurScene : public QGraphicsScene
{
public:
    explicit DeplieurScene(QObject *parent = nullptr);
    QTableWidget * twC;
};

#endif // DEPLIEURSCENE_H
