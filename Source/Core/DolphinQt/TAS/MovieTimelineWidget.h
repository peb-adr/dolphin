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

struct StateInfo
{
  // -1 for non slot state
  int slot;
  QString path;
  QString label;
  int frame;
  u64 timestamp;
};

struct StateLine
{
  StateInfo* info;
  Marker* marker;
};


class MovieTimelineWidget : public QGraphicsView
{
public:
  MovieTimelineWidget(QWidget* parent);
  ~MovieTimelineWidget();

  void AddState(StateInfo* info);
  // void AddState(const QString& path, int frame);
  // void AddStateSlot(int slot, int frame);
  void Update();
  void SetScale(int scale);
  int GetScale();
  int GetWidth();
  int GetHeight();
  void resizeEvent(QResizeEvent *event) override;

private:
  void UpdateSceneRect();
  void UpdateMovie();
  void UpdateMeasureLines();
  void UpdateCursor();

  QGraphicsScene* m_scene;
  QGraphicsRectItem* m_movieItem;
  QList<QGraphicsRectItem*> m_measureLineItems;

  Marker* m_cursorMarker;
  // QMap<int, StateLine> m_slotStateLines;
  QMap<QString, StateLine*> m_stateLines;

  int m_scale;
  int m_width;
  int m_height;
  u64 m_previousFrame;
};

  
class Marker : public QGraphicsItem
{
public:
  Marker(MovieTimelineWidget *timeline);
  ~Marker();

  void SetColor(const QColor& color);
  void SetText(const QString& text);
  void SetBaseHeight(int baseHeight);
  void SetLevelHeight(int levelHeight);
  void SetLevel(int level);
  void SetScale(int scale);
  void RecalculateHorizontal();
  void RecalculateVertical();

  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
      QWidget *widget = 0) override;
  QPainterPath shape() const override;

private:
  MovieTimelineWidget* m_timeline;

  // QGraphicsRectItem* m_lineUpperItem;
  // QGraphicsRectItem* m_lineLowerItem;
  QGraphicsRectItem* m_lineItem;
  QGraphicsRectItem* m_rectItem;
  QGraphicsSimpleTextItem* m_textItem;

  int m_baseHeight;
  int m_levelHeight;
  int m_level;
  QPointF m_previousPos;
};
