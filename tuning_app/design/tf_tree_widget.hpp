#ifndef TF_TREE_WIDGET_HPP
#define TF_TREE_WIDGET_HPP

#include <QWidget>
#include <QPainter>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <cmath>

struct TFFrameData {
    QString parent_frame;
    QString child_frame;
    double tx, ty, tz;  // translation
    double qx, qy, qz, qw;  // quaternion rotation
    qint64 last_update_time;  // timestamp in milliseconds
    int update_count;
};

class TFTreeWidget : public QWidget {
    Q_OBJECT

public:
    explicit TFTreeWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setMinimumSize(600, 400);
        setStyleSheet("background: #2e2e2e;");
        
        // Timer to calculate frequencies
        updateTimer_ = new QTimer(this);
        updateTimer_->setInterval(100);  // Update display every 100ms
        connect(updateTimer_, &QTimer::timeout, this, [this]() {
            update();  // Trigger repaint
        });
        updateTimer_->start();
    }

    void updateTransform(const QString &parent, const QString &child, 
                        double tx, double ty, double tz,
                        double qx, double qy, double qz, double qw) {
        QString key = parent + "->" + child;
        
        qint64 current_time = QDateTime::currentMSecsSinceEpoch();
        
        if (tfData_.contains(key)) {
            tfData_[key].update_count++;
        } else {
            TFFrameData data;
            data.parent_frame = parent;
            data.child_frame = child;
            data.update_count = 1;
            tfData_[key] = data;
        }
        
        tfData_[key].tx = tx;
        tfData_[key].ty = ty;
        tfData_[key].tz = tz;
        tfData_[key].qx = qx;
        tfData_[key].qy = qy;
        tfData_[key].qz = qz;
        tfData_[key].qw = qw;
        tfData_[key].last_update_time = current_time;
        
        update();  // Trigger repaint
    }

    void clearTransforms() {
        tfData_.clear();
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw background
        painter.fillRect(rect(), QColor(46, 46, 46));
        
        // Draw title
        painter.setPen(QColor(3, 252, 132));
        QFont titleFont("SF Pro", 14, QFont::Bold);
        painter.setFont(titleFont);
        painter.drawText(rect().adjusted(10, 10, -10, -10), Qt::AlignTop | Qt::AlignHCenter, "TF Tree Visualization");
        
        // Define frame positions on screen
        QMap<QString, QPointF> framePositions;
        int centerX = width() / 2;
        int startY = 80;
        
        framePositions["map"] = QPointF(centerX, startY);
        framePositions["odom"] = QPointF(centerX, startY + 100);
        framePositions["base_footprint"] = QPointF(centerX, startY + 200);
        framePositions["base_link"] = QPointF(centerX, startY + 200);  // Same as base_footprint
        framePositions["laser"] = QPointF(centerX, startY + 300);
        
        qint64 current_time = QDateTime::currentMSecsSinceEpoch();
        
        // Draw connections and info
        QFont infoFont("SF Pro", 9);
        painter.setFont(infoFont);
        
        for (auto it = tfData_.begin(); it != tfData_.end(); ++it) {
            const TFFrameData &data = it.value();
            
            if (!framePositions.contains(data.parent_frame) || 
                !framePositions.contains(data.child_frame)) {
                continue;
            }
            
            QPointF parent_pos = framePositions[data.parent_frame];
            QPointF child_pos = framePositions[data.child_frame];
            
            // Calculate distance
            double distance = std::sqrt(data.tx * data.tx + data.ty * data.ty + data.tz * data.tz);
            
            // Calculate yaw from quaternion
            double siny_cosp = 2.0 * (data.qw * data.qz + data.qx * data.qy);
            double cosy_cosp = 1.0 - 2.0 * (data.qy * data.qy + data.qz * data.qz);
            double yaw_rad = std::atan2(siny_cosp, cosy_cosp);
            double yaw_deg = yaw_rad * 180.0 / M_PI;
            
            // Calculate frequency (updates per second)
            double frequency = 0.0;
            qint64 time_diff = current_time - data.last_update_time;
            if (time_diff < 2000) {  // Only show if updated within last 2 seconds
                frequency = data.update_count;  // Approximate
            }
            
            // Draw connection line
            painter.setPen(QPen(QColor(100, 100, 100), 2));
            painter.drawLine(parent_pos, child_pos);
            
            // Draw transform info
            QPointF mid_point = (parent_pos + child_pos) / 2.0;
            mid_point.setX(mid_point.x() + 80);  // Offset to right
            
            QString info = QString("Distance: %1 m\nRotation: %2°\nFreq: %3 Hz")
                          .arg(distance, 0, 'f', 3)
                          .arg(yaw_deg, 0, 'f', 1)
                          .arg(frequency, 0, 'f', 1);
            
            painter.setPen(QColor(200, 200, 200));
            painter.drawText(QRectF(mid_point.x() - 50, mid_point.y() - 30, 100, 60), 
                           Qt::AlignLeft, info);
        }
        
        // Draw frames with coordinate axes
        for (auto it = framePositions.begin(); it != framePositions.end(); ++it) {
            QString frameName = it.key();
            QPointF pos = it.value();
            
            // Check if this frame is active
            bool isActive = false;
            for (const auto &data : tfData_) {
                if (data.child_frame == frameName || data.parent_frame == frameName) {
                    qint64 time_diff = current_time - data.last_update_time;
                    if (time_diff < 2000) {  // Active if updated within 2 seconds
                        isActive = true;
                        break;
                    }
                }
            }
            
            if (!isActive && frameName != "map") continue;  // Always show map
            
            drawFrame(painter, pos, frameName);
        }
    }
    
    void drawFrame(QPainter &painter, const QPointF &pos, const QString &name) {
        const double axisLength = 30.0;
        
        // Draw frame name
        painter.setPen(QColor(3, 252, 132));
        QFont nameFont("SF Pro", 10, QFont::Bold);
        painter.setFont(nameFont);
        painter.drawText(QRectF(pos.x() - 50, pos.y() - 50, 100, 20), 
                        Qt::AlignCenter, name);
        
        // Draw X axis (red) - pointing right
        painter.setPen(QPen(QColor(255, 0, 0), 3));
        painter.drawLine(pos, QPointF(pos.x() + axisLength, pos.y()));
        // Arrow head for X
        QPointF xArrow1(pos.x() + axisLength - 5, pos.y() - 3);
        QPointF xArrow2(pos.x() + axisLength - 5, pos.y() + 3);
        painter.drawLine(QPointF(pos.x() + axisLength, pos.y()), xArrow1);
        painter.drawLine(QPointF(pos.x() + axisLength, pos.y()), xArrow2);
        
        // Draw Y axis (green) - pointing up (right-hand rule: Z out of screen, X right, Y up)
        painter.setPen(QPen(QColor(0, 255, 0), 3));
        painter.drawLine(pos, QPointF(pos.x(), pos.y() - axisLength));
        // Arrow head for Y
        QPointF yArrow1(pos.x() - 3, pos.y() - axisLength + 5);
        QPointF yArrow2(pos.x() + 3, pos.y() - axisLength + 5);
        painter.drawLine(QPointF(pos.x(), pos.y() - axisLength), yArrow1);
        painter.drawLine(QPointF(pos.x(), pos.y() - axisLength), yArrow2);
        
        // Draw origin point
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255));
        painter.drawEllipse(pos, 4, 4);
        
        // Draw axis labels
        painter.setPen(QColor(255, 0, 0));
        QFont axisFont("SF Pro", 8);
        painter.setFont(axisFont);
        painter.drawText(QPointF(pos.x() + axisLength + 5, pos.y() + 5), "X");
        
        painter.setPen(QColor(0, 255, 0));
        painter.drawText(QPointF(pos.x() + 5, pos.y() - axisLength - 5), "Y");
    }

private:
    QMap<QString, TFFrameData> tfData_;
    QTimer *updateTimer_;
};

#endif // TF_TREE_WIDGET_HPP
