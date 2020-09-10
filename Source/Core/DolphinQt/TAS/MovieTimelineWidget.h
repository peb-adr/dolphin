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

enum MarkerData
{
  FRAME,
  TIMESTAMP
};


class MovieTimelineWidget : public QGraphicsView
{
public:
  MovieTimelineWidget(QWidget* parent);
  ~MovieTimelineWidget();

  void AddState(const StateInfo& info);
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
  // QGraphicsRectItem* m_measureLinesItem;

  Marker* m_cursorMarker;
  QMap<QString, Marker*> m_stateMarkers;

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
  void RecalculateTextBoxX();
  void RecalculateTextBoxY();
  void RecalculateLineX();
  void RecalculateLineY();

  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
      QWidget *widget = 0) override;
  QPainterPath shape() const override;

private:
  int m_baseHeight;
  int m_levelHeight;
  int m_level;
  QGraphicsRectItem* m_lineItem;
  QGraphicsRectItem* m_rectItem;
  QGraphicsSimpleTextItem* m_textItem;

  MovieTimelineWidget* m_timeline;
  QPointF m_previousPos;
};
