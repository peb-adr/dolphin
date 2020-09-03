

#include "DolphinQt/TAS/MovieTimelineWidget.h"

// DEBUG
#include <QAbstractSlider>
#include <QScrollBar>
#include <iostream>
// DEBUG END

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPen>
#include <QPixmap>
#include <QRectF>
#include <QtMath>
#include <QVBoxLayout>

#include "DolphinQt/Resources.h"
#include "Core/Movie.h"

// TODO dynamic height (dragable slider in VisualizerWindow layout)
static const int s_height = 100;
static const int s_padding = 50;
static const int s_markerHeight = 20;
static const int s_measureInterval = 10;
// static const int s_cursorRectWidth = 30;

static const QPen s_linePen(QBrush(QColor(QStringLiteral("#000"))), 2);
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

static const QPen s_stateLinePen(QColor(QStringLiteral("#000")));
static const QBrush s_stateLineBrush(QColor(QStringLiteral("#000")));
static const QPen s_stateRectPen(QBrush(QColor(QStringLiteral("#000"))), 2);
static const QBrush s_stateRectBrush(QColor(QStringLiteral("transparent")));
static const QPen s_stateTextPen(QColor(QStringLiteral("#000")));
static const QBrush s_stateTextBrush(QColor(QStringLiteral("#000")));

MovieTimelineWidget::MovieTimelineWidget(QWidget* parent) : QGraphicsView(parent)
{
  // Initialize data members
  m_scale = 1;
  m_width = s_padding;
  m_previousFrame = 0;

  // Customize GraphicsView
  setMinimumHeight(s_height + 2 * frameWidth());
  setMaximumHeight(s_height + 2 * frameWidth());
  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  verticalScrollBar()->setEnabled(false);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Setup Scene
  m_scene = new QGraphicsScene();
  setScene(m_scene);
  m_scene->setBackgroundBrush(s_backgroundBrush);
  m_scene->setSceneRect(QRectF(0, 0, 0, s_height));

  // Setup GraphicsItems
  m_movieItem = m_scene->addRect(0, 0, 0, s_height);
  m_movieItem->setZValue(-4);
  m_cursorMarker = new Marker(this);
  m_cursorMarker->SetColor(s_cursorColor);
  m_cursorMarker->setZValue(-1);
  m_scene->addItem(m_cursorMarker);
}

void MovieTimelineWidget::AddState(const QString& name, int frame)
{
  // TODO!
}

void MovieTimelineWidget::AddStateSlot(int slot, int frame)
{
  QGraphicsRectItem* line;
  QGraphicsRectItem* rect;
  QGraphicsSimpleTextItem* text;

  if (!m_stateSlotLineItems.contains(slot))
  {
    // line item
    line = m_scene->addRect(0, s_markerHeight, m_scale, s_height - 2 * s_markerHeight);
    line->setPen(s_stateLinePen);
    line->setBrush(s_stateLineBrush);
    m_stateSlotLineItems.insert(slot, line);
    // rect item
    rect = m_scene->addRect(0, s_height - s_markerHeight, 20, s_markerHeight - 1);
    rect->setPen(s_stateRectPen);
    rect->setBrush(s_stateRectBrush);
    m_stateSlotRectItems.insert(slot, rect);
    // text item
    text = m_scene->addSimpleText(QString::number(slot));
    text->setFont(QFont(QStringLiteral(""), s_markerHeight - 4, 3));
    text->setY(s_height - s_markerHeight + (s_markerHeight - ((int) text->boundingRect().height())) / 2);
    text->setPen(s_stateTextPen);
    text->setBrush(s_stateTextBrush);
    m_stateSlotTextItems.insert(slot, text);
    // set z value
    line->setZValue(-3);
    rect->setZValue(-3);
    text->setZValue(-3);
  }

  line = m_stateSlotLineItems.value(slot);
  rect = m_stateSlotRectItems.value(slot);
  text = m_stateSlotTextItems.value(slot);
  int x = frame * m_scale;
  line->setX(x);
  // MakeBoxedText(rect, text, &x);
  rect->setX(x);
  text->setX(x);
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
  // DEBUG END
}

void MovieTimelineWidget::SetScale(int scale)
{
  if (scale < 1)
  {
    return;
  }

  m_scale = scale;
  // m_cursorLineItem->setRect(0, s_markerHeight, scale, s_height - s_markerHeight);
  m_cursorMarker->SetScale(scale);
  // measure lines have to be redrawn
  qDeleteAll(m_measureLineItems);
  m_measureLineItems.clear();



  /// TODO
  // scale state markers



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


void MovieTimelineWidget::UpdateSceneRect()
{
  m_scene->setSceneRect(QRectF(0, 0, m_width, s_height));
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
  m_movieItem->setRect(0, 0, Movie::GetTotalFrames() * m_scale, s_height);
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
    item->setZValue(-2);
    item->setX(i * m_scale);
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
    m_measureLineItems.append(item);

    i += 1;
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

void Marker::SetLevel(int level)
{
  // adjust yPos and height of items
  QRectF r;
  // upper line
  r = m_lineUpperItem->rect();
  r.setY(0);
  r.setHeight(level * s_markerHeight);
  m_lineUpperItem->setRect(r);
  // lower line
  r = m_lineLowerItem->rect();
  r.setY((level + 1) * s_markerHeight);
  r.setHeight(s_height - (level + 1) * s_markerHeight);
  m_lineLowerItem->setRect(r);
  // rect
  r = m_rectItem->rect();
  r.setY(level * s_markerHeight + 1);
  r.setHeight(s_markerHeight - 1);
  m_rectItem->setRect(r);
  // text
  m_textItem->setY(level * s_markerHeight - 1);
}

void Marker::SetText(const QString& text)
{
  m_textItem->setText(text);
  // adjust xPos of text and rect
  // boxed text should be centered ...
  int width = qFloor(m_textItem->boundingRect().width());
  int xPos = -width / 2 + m_timeline->GetScale() / 2 - 3;
  // ... but never over the left scene border
  if (x() < -xPos)
  {
    xPos = -x();
  }
  m_rectItem->setX(xPos);
  m_textItem->setX(xPos + 3);
  // adjust rect width
  width += 6;
  QRectF rect = m_rectItem->rect();
  rect.setWidth(width);
  m_rectItem->setRect(rect);
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

QRectF Marker::boundingRect() const
{
  QRectF br = m_rectItem->boundingRect();
  return QRectF(br.x(), 0, br.width(), s_height);
}

void Marker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
  // not needed as paint() will delegate to children (i.e. line, rect and text items)
}
