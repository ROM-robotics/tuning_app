#ifndef CARTO_ROM_SDK_MAP_WIDGET_HPP
#define CARTO_ROM_SDK_MAP_WIDGET_HPP

#pragma once

#include "rom_map_widget.hpp"
#include <QGraphicsItemGroup>
#include <QPen>
#include <QBrush>
#include <QJsonObject>
#include <QJsonArray>
#include <QGraphicsEllipseItem>

namespace rom_dynamics::ui::qt 
{

class CartoRomMapWidget : public RomMapWidget 
{
    Q_OBJECT
public:
    explicit CartoRomMapWidget(QWidget* parent = nullptr);

    // visualization_msgs/MarkerArray messages
    void updateConstraintList(const QJsonObject& markerArrayMsg);
    void updateTrajectoryNodeList(const QJsonObject& markerArrayMsg);
    void updateRobotPose(const QJsonObject& pose2dMsg);
    void clearOverlays();

private:
    void ensureLayers();
    void drawLineList(QGraphicsItemGroup* layer,
                      const QJsonArray& points,
                      const QPen& pen);
    void drawLineStrip(QGraphicsItemGroup* layer,
                       const QJsonArray& points,
                       const QPen& pen);
    void drawPointList(QGraphicsItemGroup* layer,
                       const QJsonArray& points,
                       const QBrush& brush,
                       qreal radius = 1.5);

private:
    QGraphicsItemGroup* constraintsLayer_{nullptr};
    QGraphicsItemGroup* trajectoryLayer_{nullptr};
    QGraphicsItemGroup* robotLayer_{nullptr};
    QGraphicsEllipseItem* robotItem_{nullptr};
};


}
#endif // CARTO_ROM_MAP_WIDGET_HPP

