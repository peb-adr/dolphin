// Copyright Adrian Richter

#pragma once

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <QMap>

#include "Common/CommonTypes.h"

class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;
class QRectF;

struct StateLine
{
  const QString& label;
  int frame;
};

class MovieTimelineWidget : public QGraphicsView
{
public:
  MovieTimelineWidget(QWidget* parent);

  // void AddStateLine(StateLine state);
  void AddState(const QString& name, int frame);
  void AddStateSlot(int slot, int frame);
  void Update();
  void SetScale(int scale);

private:
  void UpdateSceneRect();
  void UpdateMovie();
  void UpdateMeasureLines();
  void UpdateCursor();

  void MakeBoxedText(QGraphicsRectItem* box, QGraphicsSimpleTextItem* text, int* xPos);

  QGraphicsScene* m_scene;
  QGraphicsRectItem* m_movieItem;
  QList<QGraphicsRectItem*> m_measureLineItems;

  QGraphicsRectItem* m_cursorLineItem;
  QGraphicsRectItem* m_cursorRectItem;
  QGraphicsSimpleTextItem* m_cursorTextItem;

  QMap<int, QGraphicsRectItem*> m_stateSlotLineItems;
  QMap<int, QGraphicsRectItem*> m_stateSlotRectItems;
  QMap<int, QGraphicsSimpleTextItem*> m_stateSlotTextItems;

  int m_scale;
  int m_width;
  u64 m_previousFrame;

  QList<StateLine> m_stateLines;

  
  class Marker : public QGraphicsItem
  {
  public:
    Marker(QGraphicsItem *parent);
    ~Marker();

  private:
    void MakeBoxedText(QGraphicsRectItem* box, QGraphicsSimpleTextItem* text, int* xPos);

    QGraphicsRectItem* m_LineItem;
    QGraphicsRectItem* m_RectItem;
    QGraphicsSimpleTextItem* m_TextItem;
  };

};
