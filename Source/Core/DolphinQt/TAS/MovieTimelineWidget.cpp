

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
static const int s_measureHeight = 20;
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

static const QPen s_cursorLinePen(QColor(QStringLiteral("#07c")));
static const QBrush s_cursorLineBrush(QColor(QStringLiteral("#07c")));
static const QPen s_cursorRectPen(QBrush(QColor(QStringLiteral("#07c"))), 2);
static const QBrush s_cursorRectBrush(QColor(QStringLiteral("transparent")));
static const QPen s_cursorTextPen(QColor(QStringLiteral("#07c")));
static const QBrush s_cursorTextBrush(QColor(QStringLiteral("#07c")));

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
  m_cursorLineItem = m_scene->addRect(0, s_measureHeight, m_scale, s_height - s_measureHeight);
  m_cursorLineItem->setPen(s_cursorLinePen);
  m_cursorLineItem->setBrush(s_cursorLineBrush);
  m_cursorRectItem = m_scene->addRect(0, 1, 20, s_measureHeight - 1);
  m_cursorRectItem->setPen(s_cursorRectPen);
  m_cursorRectItem->setBrush(s_cursorRectBrush);
  m_cursorTextItem = m_scene->addSimpleText(tr("0"));
  m_cursorTextItem->setFont(QFont(QStringLiteral(""), s_measureHeight - 4, 3));
  m_cursorTextItem->setY((s_measureHeight - ((int) m_cursorTextItem->boundingRect().height())) / 2);
  m_cursorTextItem->setPen(s_cursorTextPen);
  m_cursorTextItem->setBrush(s_cursorTextBrush);
  m_cursorLineItem->setZValue(-1);
  m_cursorRectItem->setZValue(-1);
  m_cursorTextItem->setZValue(-1);
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
    line = m_scene->addRect(0, s_measureHeight, m_scale, s_height - 2 * s_measureHeight);
    line->setPen(s_stateLinePen);
    line->setBrush(s_stateLineBrush);
    m_stateSlotLineItems.insert(slot, line);
    // rect item
    rect = m_scene->addRect(0, s_height - s_measureHeight, 20, s_measureHeight - 1);
    rect->setPen(s_stateRectPen);
    rect->setBrush(s_stateRectBrush);
    m_stateSlotRectItems.insert(slot, rect);
    // text item
    text = m_scene->addSimpleText(QString::number(slot));
    text->setFont(QFont(QStringLiteral(""), s_measureHeight - 4, 3));
    text->setY(s_height - s_measureHeight + (s_measureHeight - ((int) text->boundingRect().height())) / 2);
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
  MakeBoxedText(rect, text, &x);
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
  m_cursorLineItem->setRect(0, s_measureHeight, scale, s_height - s_measureHeight);
  // measure lines have to be redrawn
  qDeleteAll(m_measureLineItems);
  m_measureLineItems.clear();



  /// TODO
  // scale state lines and rects



  Update();
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
  QRectF rect(0, 0, m_scale, s_measureHeight);
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
  m_cursorTextItem->setText(QString::number(Movie::GetCurrentFrame()));

  int cursor = Movie::GetCurrentFrame() * m_scale;
  m_cursorLineItem->setX(cursor);
  // adjust rect and cursor
  MakeBoxedText(m_cursorRectItem, m_cursorTextItem, &cursor);
  m_cursorRectItem->setX(cursor);
  m_cursorTextItem->setX(cursor);

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

void MovieTimelineWidget::MakeBoxedText(QGraphicsRectItem* box,
    QGraphicsSimpleTextItem* text, int* xPos)
{
  int width = qFloor(text->boundingRect().width());
  *xPos = qMax(2, *xPos - width / 2 + m_scale / 2);
  width += 6;
  QRectF rect(-3, box->rect().y(), width, s_measureHeight - 1);
  box->setRect(rect);
}

MovieTimelineWidget::Marker::Marker(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent)
{
  m_LineItem = new QGraphicsRectItem(this);
  m_RectItem = new QGraphicsRectItem(this);
  m_TextItem = new QGraphicsSimpleTextItem(this);
}

MovieTimelineWidget::Marker::~Marker()
{
  delete m_LineItem;
  delete m_RectItem;
  delete m_TextItem;
}
