

#include "DolphinQt/TAS/MovieTimelineWidget.h"

// DEBUG
#include <QAbstractSlider>
#include <QScrollBar>
#include <iostream>
// DEBUG END

#include <QBrush>
#include <QColor>
#include <QFileInfo>
#include <QFont>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QPen>
#include <QPixmap>
#include <QRectF>
#include <QtMath>
#include <QVBoxLayout>

#include "DolphinQt/Resources.h"
#include "Core/Movie.h"

// TODO dynamic height (dragable slider in VisualizerWindow layout)
// static const int s_height = 100;
static const int s_padding = 50;
static const int s_markerHeight = 20;
static const int s_measureInterval = 10;

static const QBrush s_backgroundBrush(QColor(QStringLiteral("#ddd")));

static const QPen s_movieRecordingPen(QColor(QStringLiteral("#fcc")));
static const QBrush s_movieRecordingBrush(QColor(QStringLiteral("#fcc")));
static const QPen s_moviePlayingPen(QColor(QStringLiteral("#cfc")));
static const QBrush s_moviePlayingBrush(QColor(QStringLiteral("#cfc")));
static const QPen s_movieInactivePen(QColor(QStringLiteral("#aaa")));
static const QBrush s_movieInactiveBrush(QColor(QStringLiteral("#aaa")));

static const QPen s_measureEvenPen(QColor(QStringLiteral("#bbb")));
static const QBrush s_measureEvenBrush(QColor(QStringLiteral("#bbb")));
static const QPen s_measureOddPen(QColor(QStringLiteral("#ccc")));
static const QBrush s_measureOddBrush(QColor(QStringLiteral("#ccc")));

static const QColor s_cursorColor(QStringLiteral("#07c"));
static const QColor s_stateSlotColor(QStringLiteral("#333"));
static const QColor s_stateColor(QStringLiteral("#373"));

MovieTimelineWidget::MovieTimelineWidget(QWidget* parent) : QGraphicsView(parent)
{
  // Initialize data members
  m_scale = 1;
  m_width = s_padding;
  m_height = 100;
  m_previousFrame = 0;

  // Customize GraphicsView
  // setMinimumHeight(m_height + 2 * frameWidth());
  // setMaximumHeight(m_height + 2 * frameWidth());
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  // verticalScrollBar()->setEnabled(false);
  // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Setup Scene
  m_scene = new QGraphicsScene();
  setScene(m_scene);
  m_scene->setBackgroundBrush(s_backgroundBrush);
  m_scene->setSceneRect(QRectF(0, 0, 0, m_height));

  // Setup GraphicsItems
  m_movieItem = m_scene->addRect(0, 0, 0, m_height);
  m_movieItem->setZValue(-5);
  m_cursorMarker = new Marker(this);
  m_cursorMarker->SetColor(s_cursorColor);
  m_cursorMarker->SetBaseHeight(0);
  m_cursorMarker->SetLevelHeight(s_markerHeight);
  m_cursorMarker->setZValue(-1);
  m_scene->addItem(m_cursorMarker);
}

void MovieTimelineWidget::AddState(const QString& path, int frame)
{
  Marker* marker;

  if (!m_stateLines.contains(path))
  {
    marker = new Marker(this);
    marker->SetText(QFileInfo(path).baseName());
    marker->SetColor(s_stateColor);
    marker->SetBaseHeight(m_height - s_markerHeight - 5);
    marker->SetLevelHeight(s_markerHeight);
    marker->setZValue(-4);
    m_scene->addItem(marker);

    StateLine line;
    line.label = QFileInfo(path).baseName();
    line.frame = frame;
    line.marker = marker;

    m_stateLines.insert(path, line);
  }

  // FUCXK THIS
  // m_stateLines.value(path).frame = frame;
  marker = m_stateLines.value(path).marker;
  marker->setX(frame * m_scale);

  bool collides = true;
  int l = 0;

  while(collides) {
    marker->SetLevel(l--);
    
    collides = false;
    QMap<QString, StateLine>::iterator i;
    for (i = m_stateLines.begin(); !collides && i != m_stateLines.end(); ++i)
    {
      if (marker != i.value().marker)
      {
        collides = marker->collidesWithItem(i.value().marker);
      }
    }
  }
}

void MovieTimelineWidget::AddStateSlot(int slot, int frame)
{
  Marker* marker;

  if (!m_slotStateLines.contains(slot))
  {
    marker = new Marker(this);
    marker->SetText(QString::number(slot));
    marker->SetColor(s_stateSlotColor);
    marker->SetBaseHeight(2 * s_markerHeight);
    marker->SetLevelHeight(s_markerHeight);
    marker->setZValue(-3);
    m_scene->addItem(marker);

    StateLine line;
    line.label = QString::number(slot);
    line.frame = frame;
    line.marker = marker;

    m_slotStateLines.insert(slot, line);
  }

  // SHIT IM OUT
  // m_slotStateLines.value(slot).frame = frame;
  marker = m_slotStateLines.value(slot).marker;
  marker->setX(frame * m_scale);

  bool collides = true;
  int l = 0;

  while(collides) {
    marker->SetLevel(l++);
    
    collides = false;
    QMap<int, StateLine>::iterator i;
    for (i = m_slotStateLines.begin(); !collides && i != m_slotStateLines.end(); ++i)
    {
      if (marker != i.value().marker)
      {
        collides = marker->collidesWithItem(i.value().marker);
      }
    }
  }
}



void MovieTimelineWidget::Update()
{
  m_width = qMax(Movie::GetTotalFrames(), Movie::GetCurrentFrame()) * m_scale + s_padding;

  UpdateSceneRect();
  UpdateMovie();
  UpdateMeasureLines();
  UpdateCursor();

  m_previousFrame = Movie::GetCurrentFrame();

  // DEBUG
  // std::cout << qwe << " | " <<  asd << std::endl;
  // std::cout << m_height << std::endl;
  // DEBUG END
}

void MovieTimelineWidget::SetScale(int scale)
{
  if (scale < 1)
  {
    return;
  }

  m_scale = scale;

  // measure lines have to be redrawn
  qDeleteAll(m_measureLineItems);
  m_measureLineItems.clear();

  m_cursorMarker->SetScale(scale);

  // scale state markers
  for (QMap<int, StateLine>::iterator i = m_slotStateLines.begin(); i != m_slotStateLines.end(); ++i)
  {
    i.value().marker->setX(i.value().frame * scale);
    i.value().marker->SetScale(scale);
  }
  for (QMap<QString, StateLine>::iterator i = m_stateLines.begin(); i != m_stateLines.end(); ++i)
  {
    i.value().marker->setX(i.value().frame * scale);
    i.value().marker->SetScale(scale);
  }

  Update();
}

int MovieTimelineWidget::GetScale()
{
  return m_scale;
}

int MovieTimelineWidget::GetWidth()
{
  return m_width;
}

int MovieTimelineWidget::GetHeight()
{
  return m_height;
}

void MovieTimelineWidget::resizeEvent(QResizeEvent *event)
{
  m_height = height() - 2 * frameWidth();
  m_cursorMarker->RecalculateVertical();
  for (QMap<int, StateLine>::iterator i = m_slotStateLines.begin(); i != m_slotStateLines.end(); ++i)
  {
    i.value().marker->RecalculateVertical();
  }
  for (QMap<QString, StateLine>::iterator i = m_stateLines.begin(); i != m_stateLines.end(); ++i)
  {
    i.value().marker->SetBaseHeight(m_height - s_markerHeight - 5);
    i.value().marker->RecalculateVertical();
  }
}

void MovieTimelineWidget::UpdateSceneRect()
{
  m_scene->setSceneRect(QRectF(0, 0, m_width, m_height));
}

void MovieTimelineWidget::UpdateMovie()
{
  if (Movie::IsRecordingInput()) {
    m_movieItem->setPen(s_movieRecordingPen);
    m_movieItem->setBrush(s_movieRecordingBrush);
  }
  else if (Movie::IsPlayingInput()) {
    m_movieItem->setPen(s_moviePlayingPen);
    m_movieItem->setBrush(s_moviePlayingBrush);
  }
  else {
    m_movieItem->setPen(s_movieInactivePen);
    m_movieItem->setBrush(s_movieInactiveBrush);
  }
  m_movieItem->setRect(0, 0, Movie::GetTotalFrames() * m_scale, m_height);
}

void MovieTimelineWidget::UpdateMeasureLines()
{
  QRectF rect(0, 0, m_scale, s_markerHeight);
  QGraphicsRectItem* item;
  int i = m_measureLineItems.length();
  // expand list of measure lines to fill the sceneRect but at least the view
  int w = qMax(m_width, width() - 2 * frameWidth());
  while(m_measureLineItems.length() * m_scale < w) {
    item = m_scene->addRect(rect);
    if (i % (2 * s_measureInterval) < s_measureInterval)
    {
      item->setPen(s_measureEvenPen);
      item->setBrush(s_measureEvenBrush);
    }
    else
    {
      item->setPen(s_measureOddPen);
      item->setBrush(s_measureOddBrush);
    }
    item->setZValue(-2);
    item->setX(i++ * m_scale);
    m_measureLineItems.append(item);
  }
}

void MovieTimelineWidget::UpdateCursor()
{
  int cursor = Movie::GetCurrentFrame() * m_scale;
  m_cursorMarker->SetText(QString::number(Movie::GetCurrentFrame()));
  m_cursorMarker->setX(cursor);

  // if cursor position didnt change dont scroll
  if (Movie::GetCurrentFrame() == m_previousFrame)
  {
    return;
  }
  int cursorL = cursor - s_padding;
  int cursorR = cursor + s_padding;
  int viewWidth = m_width - horizontalScrollBar()->maximum();
  // if cursor left to view scroll left
  if (cursorL < horizontalScrollBar()->value())
  {
    horizontalScrollBar()->setValue(cursorL);
  }
  // if cursor right to view scroll right
  if (cursorR > horizontalScrollBar()->value() + viewWidth)
  {
    horizontalScrollBar()->setValue(cursorR - viewWidth);
  }
}


////////////////////////
////////////////////////
///// class Marker /////
////////////////////////
////////////////////////

Marker::Marker(MovieTimelineWidget *timeline = nullptr) : QGraphicsItem(nullptr)
{
  m_baseHeight = 0;
  m_levelHeight = 10;
  m_level = 0;
  m_previousPos = QPointF(0, 0);

  m_timeline = timeline;
  m_lineUpperItem = new QGraphicsRectItem(this);
  m_lineLowerItem = new QGraphicsRectItem(this);
  m_rectItem = new QGraphicsRectItem(this);
  m_textItem = new QGraphicsSimpleTextItem(this);
  m_textItem->setFont(QFont(QStringLiteral(""), s_markerHeight - 4, 3));

  SetScale(m_timeline->GetScale());
  SetLevel(0);
  SetText(QString());
}

Marker::~Marker()
{
  delete m_lineUpperItem;
  delete m_lineLowerItem;
  delete m_rectItem;
  delete m_textItem;
}

void Marker::SetColor(const QColor& color)
{
  m_lineUpperItem->setPen(QPen(color));
  m_lineUpperItem->setBrush(QBrush(color));
  m_lineLowerItem->setPen(QPen(color));
  m_lineLowerItem->setBrush(QBrush(color));
  m_rectItem->setPen(QPen(QBrush(color), 2));
  m_rectItem->setBrush(QBrush(QColor(QStringLiteral("transparent"))));
  m_textItem->setPen(QPen(color));
  m_textItem->setBrush(QBrush(color));
}

void Marker::SetText(const QString& text)
{
  m_textItem->setText(text);
  RecalculateHorizontal();
}

void Marker::SetBaseHeight(int baseHeight)
{
  m_baseHeight = baseHeight;
  RecalculateVertical();
}

void Marker::SetLevelHeight(int levelHeight)
{
  m_levelHeight = levelHeight;
  RecalculateVertical();
}

void Marker::SetLevel(int level)
{
  m_level = level;
  RecalculateVertical();
}

void Marker::SetScale(int scale)
{
  // adjust width of lines
  QRectF r;
  // upper line
  r = m_lineUpperItem->rect();
  r.setWidth(scale);
  m_lineUpperItem->setRect(r);
  // lower line
  r = m_lineLowerItem->rect();
  r.setWidth(scale);
  m_lineLowerItem->setRect(r);
}

void Marker::RecalculateHorizontal()
{
  QRectF rect = m_rectItem->rect();
  int width = qFloor(m_textItem->boundingRect().width());
  // adjust xPos of text and rect
  // boxed text should be centered ...
  int xPos = -width / 2 + m_timeline->GetScale() / 2 - 3;
  // ... but never go over the left scene border
  if (x() < -xPos)
  {
    xPos = -x();
  }
  m_textItem->setX(xPos + 3);
  rect.setX(xPos);
  // adjust rect width
  width += 6;
  rect.setWidth(width);
  m_rectItem->setRect(rect);
}

void Marker::RecalculateVertical()
{
  // adjust yPos and height of items
  QRectF r;
  // upper line
  r = m_lineUpperItem->rect();
  r.setY(0);
  r.setHeight(m_baseHeight + m_level * m_levelHeight);
  m_lineUpperItem->setRect(r);
  // lower line
  r = m_lineLowerItem->rect();
  r.setY(m_baseHeight + (m_level + 1) * m_levelHeight);
  r.setHeight(m_timeline->GetHeight() - m_baseHeight - (m_level + 1) * m_levelHeight);
  m_lineLowerItem->setRect(r);
  // rect
  r = m_rectItem->rect();
  r.setY(m_baseHeight + m_level * m_levelHeight + 1);
  r.setHeight(m_levelHeight - 1);
  m_rectItem->setRect(r);
  // text
  m_textItem->setY(m_baseHeight + m_level * m_levelHeight - 1);
}

QRectF Marker::boundingRect() const
{
  QRectF br = m_rectItem->boundingRect();
  return QRectF(br.x(), 0, br.width(), m_timeline->GetHeight());
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
  if (pos() != m_previousPos)
  {
    RecalculateHorizontal();
  }
  m_previousPos = pos();
}

QPainterPath Marker::shape() const
{
  QPainterPath path;
  QRectF rect = m_rectItem->rect();
  rect.setX(rect.x() + m_rectItem->x());
  path.addRect(rect);
  return path;
}
