#include "rom_map_widget.hpp"

namespace rom_dynamics::ui::qt 
{

using rom_dynamics::data_types::RomMap;


RomMapWidget::RomMapWidget(QWidget* parent) : QGraphicsView(parent)
    {
        setFrameShape(QFrame::NoFrame);
        setBackgroundBrush(QColor("#2a2f36"));
        setRenderHint(QPainter::Antialiasing, false);
        setRenderHint(QPainter::SmoothPixmapTransform, true);
        setRenderHint(QPainter::TextAntialiasing, true);
        setDragMode(QGraphicsView::ScrollHandDrag); // basic pan for now
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        scene_ = new QGraphicsScene(this);
        setScene(scene_);

        mapItem_ = scene_->addPixmap(QPixmap());
    }

// Update the displayed occupancy grid; cheap if same size.
void RomMapWidget::setMap(const RomMap& map)
    {
        QImage img = romMapToImage_local(map);
        if (img.isNull()) return;

        const QSize newSize = img.size();
        mapItem_->setPixmap(QPixmap::fromImage(img));
        mapItem_->setOffset(-img.width() / 2.0, -img.height() / 2.0); // center around (0,0)
        scene_->setSceneRect(mapItem_->boundingRect());

        // Cache metadata for conversions
        mapWidth_ = map.width();
        mapHeight_ = map.height();
        mapResolution_ = map.resolution();
        mapOriginX_ = map.originX();
        mapOriginY_ = map.originY();
        
        // if (!mapOriginAxes_) {
        //     mapOriginAxes_ = new MapOriginCoordinateFrame();
        //     scene_->addItem(mapOriginAxes_);
        //     mapOriginAxes_->setVisible(axesVisible_);
        // }
        const int w = map.width();
        const int h = map.height();
        const float res = map.resolution();
        if (w > 0 && h > 0 && res > 0.0f) 
        {
            const double ox = static_cast<double>(map.originX());
            const double oy = static_cast<double>(map.originY());
            const double pixel_x = -ox / res;
            const double pixel_y = (static_cast<double>(h) - 1.0) + oy / res;

            const double scene_x = pixel_x - static_cast<double>(w) / 2.0;
            const double scene_y = pixel_y - static_cast<double>(h) / 2.0;
            //mapOriginAxes_->setPos(scene_x, scene_y);
            //mapOriginAxes_->setVisible(axesVisible_);
        }
        // else if (mapOriginAxes_) {
        //     mapOriginAxes_->setVisible(false);
        // }

        if (autoFit_ && newSize != lastMapSize_) {
            fitInView(mapItem_, Qt::KeepAspectRatio);
            lastMapSize_ = newSize;
        }
    }

void RomMapWidget::updateMap(const QJsonObject& msg)
{
    // Expect ROS OccupancyGrid schema: { info:{width,height,resolution,origin:{position:{x,y}}}, data:[...] }
    if (msg.isEmpty()) return;

    const QJsonObject info = msg.value("info").toObject();
    const int width  = info.value("width").toInt(0);
    const int height = info.value("height").toInt(0);
    const double resolution = info.value("resolution").toDouble(0.0);

    double origin_x = 0.0;
    double origin_y = 0.0;
    if (info.contains("origin")) {
        const QJsonObject origin = info.value("origin").toObject();
        const QJsonObject pos = origin.value("position").toObject();
        origin_x = pos.value("x").toDouble(0.0);
        origin_y = pos.value("y").toDouble(0.0);
    }

    const QJsonArray dataArr = msg.value("data").toArray();
    std::vector<int8_t> data;
    data.reserve(static_cast<size_t>(width) * static_cast<size_t>(height));
    for (const auto &v : dataArr) {
        int vi = v.isDouble() ? static_cast<int>(v.toDouble()) : v.toInt(-1);
        if (vi < -1) vi = -1; // OccupancyGrid uses [-1,0..100]
        if (vi > 100) vi = 100;
        data.push_back(static_cast<int8_t>(vi));
    }
    // If missing or size mismatch, fill with unknown
    if (static_cast<int>(data.size()) < width * height) {
        data.assign(static_cast<size_t>(width * height), static_cast<int8_t>(-1));
    }

    RomMap map(width, height, static_cast<float>(resolution), static_cast<float>(origin_x), static_cast<float>(origin_y), data);
    setMap(map);
}
    
void RomMapWidget::clearMap()
{
    mapItem_->setPixmap(QPixmap());
    scene_->setSceneRect(QRectF());
    lastMapSize_ = QSize();
}

// Fit the current map pixmap to the view keeping aspect ratio.
void RomMapWidget::fitToView()
{
    if (mapItem_ && !mapItem_->pixmap().isNull()) {
        // Re-enable autoFit for this operation, and reset user zoom flag
        autoFit_ = true;
        userZoomed_ = false;
        fitInView(mapItem_, Qt::KeepAspectRatio);
    }
}


void RomMapWidget::setAutoFitEnabled(bool enabled) { autoFit_ = enabled; }

bool RomMapWidget::autoFitEnabled() const { return autoFit_; }

void RomMapWidget::resizeEvent(QResizeEvent* e)
{
    QGraphicsView::resizeEvent(e);
    if (autoFit_ && !userZoomed_ && mapItem_ && !mapItem_->pixmap().isNull()) {
        fitInView(mapItem_, Qt::KeepAspectRatio);
    }
}

void RomMapWidget::wheelEvent(QWheelEvent* event)
{
    if (!mapItem_ || mapItem_->pixmap().isNull()) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    // Typical mouse wheel step: angleDelta.y() is multiples of 120
    const int delta = event->angleDelta().y();
    if (delta == 0) { event->ignore(); return; }

    // Smooth zoom factor, clamp to min/max overall scale
    const double stepFactor = 1.15; // zoom per 120 units
    double factor = (delta > 0) ? stepFactor : (1.0 / stepFactor);

    // Current uniform scale (assumes no shear)
    const QTransform t = transform();
    double currentScale = t.m11();
    double proposed = currentScale * factor;
    if (proposed < minScale_) {
        factor = minScale_ / currentScale;
    } else if (proposed > maxScale_) {
        factor = maxScale_ / currentScale;
    }

    scale(factor, factor);
    userZoomed_ = true;
    autoFit_ = false;
    event->accept();
}

}


