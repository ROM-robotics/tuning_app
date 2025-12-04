#include "carto_rom_map_widget.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <QColor>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>

namespace rom_dynamics::ui::qt {

CartoRomMapWidget::CartoRomMapWidget(QWidget* parent)
    : RomMapWidget(parent)
{
    ensureLayers();
}

void CartoRomMapWidget::ensureLayers()
{
    auto* sc = scenePtr();
    if (!constraintsLayer_ && sc) {
        constraintsLayer_ = new QGraphicsItemGroup();
        constraintsLayer_->setZValue(10.0);
        sc->addItem(constraintsLayer_);
    }
    if (!trajectoryLayer_ && sc) {
        trajectoryLayer_ = new QGraphicsItemGroup();
        trajectoryLayer_->setZValue(11.0);
        sc->addItem(trajectoryLayer_);
    }
    if (!robotLayer_ && sc) {
        robotLayer_ = new QGraphicsItemGroup();
        robotLayer_->setZValue(12.0);
        sc->addItem(robotLayer_);
    }
}

void CartoRomMapWidget::clearOverlays()
{
    if (constraintsLayer_) {
        const auto children = constraintsLayer_->childItems();
        for (QGraphicsItem* it : children) {
            delete it;
        }
    }
    if (trajectoryLayer_) {
        const auto children = trajectoryLayer_->childItems();
        for (QGraphicsItem* it : children) {
            delete it;
        }
    }
}

static QColor readColor(const QJsonObject& marker, const QColor& fallback)
{
    if (marker.contains("color")) {
        const QJsonObject c = marker.value("color").toObject();
        const double r = c.value("r").toDouble(fallback.redF());
        const double g = c.value("g").toDouble(fallback.greenF());
        const double b = c.value("b").toDouble(fallback.blueF());
        const double a = c.value("a").toDouble(fallback.alphaF());
        QColor col;
        col.setRgbF(r, g, b, a);
        return col;
    }
    return fallback;
}

static double readLineWidth(const QJsonObject& marker, double fallbackPx)
{
    // Marker.scale may be scalar or an object with x
    if (marker.contains("scale")) {
        const QJsonValue s = marker.value("scale");
        if (s.isDouble()) {
            double m = s.toDouble(0.0);
            if (m > 0.0) return fallbackPx; // meters not directly used; keep pixel width
        } else if (s.isObject()) {
            const QJsonObject so = s.toObject();
            double x = so.value("x").toDouble(0.0);
            if (x > 0.0) return fallbackPx; // keep pixel width; could map meters->pixels with resolution if needed
        }
    }
    return fallbackPx;
}

void CartoRomMapWidget::drawLineList(QGraphicsItemGroup* layer,
                                     const QJsonArray& points,
                                     const QPen& pen)
{
    if (!layer) return;
    auto* sc = scenePtr();
    for (int i = 0; i + 1 < points.size(); i += 2) {
        const QJsonObject p0 = points.at(i).toObject();
        const QJsonObject p1 = points.at(i + 1).toObject();
        QPointF s0 = worldToScene(p0.value("x").toDouble(), p0.value("y").toDouble());
        QPointF s1 = worldToScene(p1.value("x").toDouble(), p1.value("y").toDouble());
        auto* item = sc->addLine(QLineF(s0, s1), pen);
        item->setParentItem(layer);
    }
}

void CartoRomMapWidget::drawLineStrip(QGraphicsItemGroup* layer,
                                      const QJsonArray& points,
                                      const QPen& pen)
{
    if (!layer) return;
    auto* sc = scenePtr();
    for (int i = 0; i + 1 < points.size(); ++i) {
        const QJsonObject p0 = points.at(i).toObject();
        const QJsonObject p1 = points.at(i + 1).toObject();
        QPointF s0 = worldToScene(p0.value("x").toDouble(), p0.value("y").toDouble());
        QPointF s1 = worldToScene(p1.value("x").toDouble(), p1.value("y").toDouble());
        auto* item = sc->addLine(QLineF(s0, s1), pen);
        item->setParentItem(layer);
    }
}

void CartoRomMapWidget::drawPointList(QGraphicsItemGroup* layer,
                                      const QJsonArray& points,
                                      const QBrush& brush,
                                      qreal radius)
{
    if (!layer) return;
    auto* sc = scenePtr();
    const qreal d = 2.0 * radius;
    for (const auto& v : points) {
        const QJsonObject p = v.toObject();
        QPointF s = worldToScene(p.value("x").toDouble(), p.value("y").toDouble());
        auto* item = sc->addEllipse(s.x() - radius, s.y() - radius, d, d, QPen(Qt::NoPen), brush);
        item->setParentItem(layer);
    }
}

void CartoRomMapWidget::updateConstraintList(const QJsonObject& markerArrayMsg)
{
    ensureLayers();
    if (!constraintsLayer_) return;
    // Require map metadata to convert meters->pixels
    if (mapResolution() <= 0.0 || mapWidth() <= 0 || mapHeight() <= 0) return;

    // Redraw constraints each update for a simple, correct picture
    const auto children = constraintsLayer_->childItems();
    for (QGraphicsItem* it : children) delete it;

    const QJsonArray markers = markerArrayMsg.value("markers").toArray();
    for (const auto& mv : markers) {
        const QJsonObject m = mv.toObject();

        // 0=ADD/MODIFY, 2=DELETE
        const int action = m.value("action").toInt(0);
        if (action == 2) continue;

        // Marker type (int)
        int type = 0;
        if (m.value("type").isDouble()) type = m.value("type").toInt(0);

        const QJsonArray points = m.value("points").toArray();
        const QJsonArray colors = m.value("colors").toArray();
        QColor baseColor = readColor(m, QColor(0, 200, 255, 255));

        // Convert meters->pixels for widths/radii
        double pxFromMeters = 1.0 / mapResolution();
        double pxWidth = 1.5; // default line width in px
        if (m.contains("scale")) {
            const QJsonValue s = m.value("scale");
            double sx = 0.0;
            if (s.isDouble()) sx = s.toDouble(0.0);
            else if (s.isObject()) sx = s.toObject().value("x").toDouble(0.0);
            if (sx > 0.0) {
                pxWidth = std::clamp(sx * pxFromMeters, 1.0, 6.0);
            }
        }

    QPen pen(baseColor);
    pen.setCosmetic(true); // stay constant width in view
    pen.setWidthF(pxWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

        switch (type) {
            case 5: // LINE_LIST
                if (!colors.isEmpty() && colors.size() == points.size()) {
                    // Per-segment color if provided (pairs of points)
                    for (int i = 0; i + 1 < points.size(); i += 2) {
                        const QJsonObject p0 = points.at(i).toObject();
                        const QJsonObject p1 = points.at(i + 1).toObject();
                        QPointF s0 = worldToScene(p0.value("x").toDouble(), p0.value("y").toDouble());
                        QPointF s1 = worldToScene(p1.value("x").toDouble(), p1.value("y").toDouble());
                        QColor segColor = baseColor;
                        if (i < colors.size()) {
                            const QJsonObject c = colors.at(i).toObject();
                            segColor.setRgbF(
                                c.value("r").toDouble(segColor.redF()),
                                c.value("g").toDouble(segColor.greenF()),
                                c.value("b").toDouble(segColor.blueF()),
                                c.value("a").toDouble(segColor.alphaF()));
                        }
                        QPen segPen = pen; segPen.setColor(segColor);
                        auto* item = scenePtr()->addLine(QLineF(s0, s1), segPen);
                        item->setParentItem(constraintsLayer_);
                    }
                } else {
                    drawLineList(constraintsLayer_, points, pen);
                }
                break;
            case 4: // LINE_STRIP
                drawLineStrip(constraintsLayer_, points, pen);
                break;
            case 7: // SPHERE_LIST -> draw as discs
            case 8: { // POINTS
                double radiusPx = std::max(1.0, pxWidth * 0.8);
                QBrush brush(baseColor);
                if (!colors.isEmpty() && colors.size() == points.size()) {
                    for (int i = 0; i < points.size(); ++i) {
                        const QJsonObject p = points.at(i).toObject();
                        QPointF s = worldToScene(p.value("x").toDouble(), p.value("y").toDouble());
                        QColor pc = baseColor;
                        const QJsonObject c = colors.at(i).toObject();
                        pc.setRgbF(
                            c.value("r").toDouble(pc.redF()),
                            c.value("g").toDouble(pc.greenF()),
                            c.value("b").toDouble(pc.blueF()),
                            c.value("a").toDouble(pc.alphaF()));
                        auto* item = scenePtr()->addEllipse(s.x() - radiusPx, s.y() - radiusPx,
                                                            2*radiusPx, 2*radiusPx,
                                                            QPen(Qt::NoPen), QBrush(pc));
                        item->setParentItem(constraintsLayer_);
                    }
                } else {
                    drawPointList(constraintsLayer_, points, brush, radiusPx);
                }
                break;
            }
            default:
                // Unsupported types are ignored gracefully
                break;
        }
    }
}

void CartoRomMapWidget::updateTrajectoryNodeList(const QJsonObject& markerArrayMsg)
{
    ensureLayers();
    if (!trajectoryLayer_) return;
    if (mapResolution() <= 0.0 || mapWidth() <= 0 || mapHeight() <= 0) return;

    // Redraw trajectory nodes each time
    const auto children = trajectoryLayer_->childItems();
    for (QGraphicsItem* it : children) delete it;

    const QJsonArray markers = markerArrayMsg.value("markers").toArray();
    for (const auto& mv : markers) {
        const QJsonObject m = mv.toObject();
        const int action = m.value("action").toInt(0);
        if (action == 2) continue;

        int type = 0;
        if (m.value("type").isDouble()) type = m.value("type").toInt(0);
        const QJsonArray points = m.value("points").toArray();
        const QJsonArray colors = m.value("colors").toArray();
        QColor baseColor = readColor(m, QColor(255, 128, 0, 255));

        double pxFromMeters = 1.0 / mapResolution();
        double pxWidth = 1.2;
        if (m.contains("scale")) {
            const QJsonValue s = m.value("scale");
            double sx = 0.0;
            if (s.isDouble()) sx = s.toDouble(0.0);
            else if (s.isObject()) sx = s.toObject().value("x").toDouble(0.0);
            if (sx > 0.0) {
                pxWidth = std::clamp(sx * pxFromMeters, 1.0, 6.0);
            }
        }

    QPen pen(baseColor);
    pen.setCosmetic(true);
    pen.setWidthF(pxWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

        if (type == 4) { // LINE_STRIP
            drawLineStrip(trajectoryLayer_, points, pen);
        } else if (type == 5) { // LINE_LIST
            drawLineList(trajectoryLayer_, points, pen);
        } else if (type == 7 || type == 8) { // SPHERE_LIST or POINTS
            double radiusPx = std::max(1.0, pxWidth * 0.8);
            if (!colors.isEmpty() && colors.size() == points.size()) {
                for (int i = 0; i < points.size(); ++i) {
                    const QJsonObject p = points.at(i).toObject();
                    QPointF s = worldToScene(p.value("x").toDouble(), p.value("y").toDouble());
                    QColor pc = baseColor;
                    const QJsonObject c = colors.at(i).toObject();
                    pc.setRgbF(
                        c.value("r").toDouble(pc.redF()),
                        c.value("g").toDouble(pc.greenF()),
                        c.value("b").toDouble(pc.blueF()),
                        c.value("a").toDouble(pc.alphaF()));
                    auto* item = scenePtr()->addEllipse(s.x() - radiusPx, s.y() - radiusPx,
                                                        2*radiusPx, 2*radiusPx,
                                                        QPen(Qt::NoPen), QBrush(pc));
                    item->setParentItem(trajectoryLayer_);
                }
            } else {
                drawPointList(trajectoryLayer_, points, QBrush(baseColor), radiusPx);
            }
        }
    }
}

void CartoRomMapWidget::updateRobotPose(const QJsonObject& pose2dMsg)
{
    ensureLayers();
    if (!robotLayer_) return;

    // Need valid map metadata to correctly convert world coords
    if (mapResolution() <= 0.0) return;

    // Parse Pose2D: { x, y, theta }
    const double x = pose2dMsg.value("x").toDouble(0.0);
    const double y = pose2dMsg.value("y").toDouble(0.0);
    const double theta = pose2dMsg.value("theta").toDouble(0.0);

    // Represent robot as a circle centered at (x,y) with a configurable radius
    // Choose radius as half of a typical footprint (meters)
    const double robot_radius_m = 0.02; // tweak as needed
    const double radius_px = std::max(1.0, robot_radius_m / mapResolution());

    // Center in scene coordinates
    const QPointF center_scene = worldToScene(x, y);
    const QRectF rect(center_scene.x() - radius_px,
                      center_scene.y() - radius_px,
                      2.0 * radius_px,
                      2.0 * radius_px);

    if (!robotItem_) {
        robotItem_ = new QGraphicsEllipseItem(rect);
        robotItem_->setBrush(QBrush(QColor(40, 160, 240, 160)));
        QPen pen(QColor(20, 120, 200));
        pen.setWidthF(1.0);
        robotItem_->setPen(pen);
        robotItem_->setParentItem(robotLayer_);
    } else {
        robotItem_->setRect(rect);
    }

}
} // namespace rom_dynamics::ui::qt
