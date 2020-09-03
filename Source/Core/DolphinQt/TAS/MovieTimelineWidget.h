// Copyright Adrian Richter

#pragma once

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QList>
#include <QMap>

#include "Common/CommonTypes.h"

class Marker;
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
  int GetScale();
  int GetWidth();

private:
  void UpdateSceneRect();
  void UpdateMovie();
  void UpdateMeasureLines();
  void UpdateCursor();

  // void MakeBoxedText(QGraphicsRectItem* box, QGraphicsSimpleTextItem* text, int* xPos);

  QGraphicsScene* m_scene;
  QGraphicsRectItem* m_movieItem;
  QList<QGraphicsRectItem*> m_measureLineItems;

  // QGraphicsRectItem* m_cursorMarkerLineItem;
  // QGraphicsRectItem* m_cursorMarkerRectItem;
  // QGraphicsSimpleTextItem* m_cursorMarkerTextItem;
  Marker* m_cursorMarker;

  QMap<int, QGraphicsRectItem*> m_stateSlotLineItems;
  QMap<int, QGraphicsRectItem*> m_stateSlotRectItems;
  QMap<int, QGraphicsSimpleTextItem*> m_stateSlotTextItems;

  int m_scale;
  int m_width;
  u64 m_previousFrame;

  QList<StateLine> m_stateLines;
};

  
class Marker : public QGraphicsItem
{
public:
  Marker(MovieTimelineWidget *timeline);
  ~Marker();

  void SetScale(int scale);
  void SetLevel(int level);
  void SetText(const QString& text);
  void SetColor(const QColor& color);

  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
      QWidget *widget = 0) override;

private:
  MovieTimelineWidget* m_timeline;

  QGraphicsRectItem* m_lineUpperItem;
  QGraphicsRectItem* m_lineLowerItem;
  QGraphicsRectItem* m_rectItem;
  QGraphicsSimpleTextItem* m_textItem;
};