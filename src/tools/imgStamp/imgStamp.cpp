// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "imgStamp.h"
#include "colorutils.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

#include "src/utils/confighandler.h"

namespace {
#define PADDING_VALUE 0
#define THICKNESS_OFFSET 60
#define STEP_MULTIPLIER 10
}

StampTool::StampTool(QObject* parent)
  : AbstractTwoPointTool(parent)
  , m_valid(false)
{}

QIcon StampTool::icon(const QColor& background, bool inEditor) const
{
    Q_UNUSED(inEditor)
    return QIcon(iconPath(background) + "stamp.svg");
}

QString StampTool::info()
{
    m_tempString = QString("%1 - %2").arg(name()).arg(count());
    return m_tempString;
}

bool StampTool::isValid() const
{
    return m_valid;
}

QRect StampTool::mousePreviewRect(const CaptureContext& context) const
{
    int width = (context.toolSize + THICKNESS_OFFSET);
    QRect rect(0, 0, width, width);
    rect.moveCenter(context.mousePos);
    return rect;
}

QRect StampTool::boundingRect() const
{
    if (!isValid()) {
        return {};
    }
    int bubble_size = (size() * STEP_MULTIPLIER) + THICKNESS_OFFSET + PADDING_VALUE;

    int line_pos_min_x =
      std::min(points().first.x() - bubble_size, points().second.x());
    int line_pos_min_y =
      std::min(points().first.y() - bubble_size, points().second.y());
    int line_pos_max_x =
      std::max(points().first.x() + bubble_size, points().second.x());
    int line_pos_max_y =
      std::max(points().first.y() + bubble_size, points().second.y());

    return { line_pos_min_x,
             line_pos_min_y,
             line_pos_max_x - line_pos_min_x,
             line_pos_max_y - line_pos_min_y };
}

QString StampTool::name() const
{
    return tr("imgStamp");
}

CaptureTool::Type StampTool::type() const
{
    return CaptureTool::TYPE_STAMP;
}

void StampTool::copyParams(const StampTool* from,
                                 StampTool* to)
{
    AbstractTwoPointTool::copyParams(from, to);
    to->setCount(from->count());
    to->m_valid = from->m_valid;
}

QString StampTool::description() const
{
    return tr("Adds a stamp from path");
}

CaptureTool* StampTool::copy(QObject* parent)
{
    auto* tool = new StampTool(parent);
    copyParams(this, tool);
    return tool;
}

void StampTool::process(QPainter& painter, const QPixmap& pixmap)
{
    // save current pen, brush, and font state
    auto orig_pen = painter.pen();
    auto orig_brush = painter.brush();
    auto orig_font = painter.font();

    QPixmap pix(ConfigHandler().stampFileLocation());

    if (pix.isNull()) {
        qWarning() << "Failed to load the image!";
    } else {
        QPoint point = points().first;
        int _size = (size() * STEP_MULTIPLIER + THICKNESS_OFFSET);
        int offset = _size / 2;
        painter.drawPixmap(point.x() - offset, point.y() - offset, _size, _size, pix);
    }

    // restore original font, brush, and pen
    painter.setFont(orig_font);
    painter.setBrush(orig_brush);
    painter.setPen(orig_pen);
}

void StampTool::paintMousePreview(QPainter& painter,
                                        const CaptureContext& context)
{
    onSizeChanged(context.toolSize + PADDING_VALUE);

    auto orig_pen = painter.pen();
    auto orig_brush = painter.brush();
    auto orig_font = painter.font();
    // painter.setPen(QPen(context.color,
    //                     (size() + THICKNESS_OFFSET),
    //                     Qt::SolidLine,
    //                     Qt::RoundCap));
    // painter.drawLine(context.mousePos,
    //                  { context.mousePos.x() + 1, context.mousePos.y() + 1 });

    QPixmap pix(ConfigHandler().stampFileLocation());

    if (pix.isNull()) {
        qWarning() << "Failed to load the image!";
    } else {
        int _size = (size() * STEP_MULTIPLIER) + THICKNESS_OFFSET;
        int offset = _size / 2;
        painter.drawPixmap(context.mousePos.x() - offset, context.mousePos.y() - offset, _size, _size, pix);
    }


    painter.setFont(orig_font);
    painter.setBrush(orig_brush);
    painter.setPen(orig_pen);
}

void StampTool::drawStart(const CaptureContext& context)
{
    AbstractTwoPointTool::drawStart(context);
    m_valid = true;
}

void StampTool::pressed(CaptureContext& context)
{
    Q_UNUSED(context)
}
