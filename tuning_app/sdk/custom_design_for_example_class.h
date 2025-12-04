
#ifndef CUSTOM_DESIGN_H
#define CUSTOM_DESIGN_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QScreen>
#include <QEasingCurve>
#include <QGraphicsView>
#include <QListWidget>
#include <QMouseEvent>

// Forward declarations for pointers used in MapWidget
class QGraphicsScene;
class QGraphicsPixmapItem;
class QWheelEvent;

#include "rom_structures.h"
#include <QColor>

// ----------------------------------------------------- AddRobotDialog

class RomCustomDialog : public QDialog 
{
    Q_OBJECT
public:
    explicit RomCustomDialog(QWidget* parent = nullptr, int width = 300, int height = 260) 
    : QDialog(parent), width_(width), height_(height)
    {
        // Use a frameless CHILD overlay (no top-level) so it always moves with parent on Wayland
        // Keeping it non-toplevel avoids compositor restrictions on programmatic positioning.
        setWindowFlags(Qt::FramelessWindowHint);
        setModal(true);
        setFixedSize(width_, height_);
        setStyleSheet(
		QString(
			"QDialog {"
			"    border-radius: 12px;"
			"    border: none;"
			"    background-color: #222;"
			"    font-size: 12px;"
			"    font-family: 'SF Pro Text';"
			"    color: white;"
			"}"
			"QLabel {"
			"    color: white;"
			"}"
			"QLabel#subtitleLabel {"
			"    font-size: 14px;"
			"    color: white;"
			"}"
		));

        cancelBtn = std::make_unique<QPushButton>("Cancel", this);
        // Place buttons relative to dialog height so they stay at the bottom even if height changes
        int buttonsY = std::max(0, height_ - 70); // 36px tall + ~30px bottom margin
        cancelBtn->setGeometry(45, buttonsY, 100, 36);
        cancelBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #4a4843;"
            "    color: white;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "    font-family: 'SF Pro Text';"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: #262522;"
            "}"
        );

        connectBtn = std::make_unique<QPushButton>("Connect", this);
        connectBtn->setGeometry(155, buttonsY, 100, 36);
        connectBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #007aff;"
            "    color: white;"
            "    border-radius: 8px;"
            "    padding: 10px 20px;"
            "    font-size: 14px;"
            "    font-family: 'SF Pro Text';"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: #005bb5;"
            "}"
        );
        
    }

    std::unique_ptr<QPushButton> cancelBtn;
    std::unique_ptr<QPushButton> connectBtn;
protected:
    

private:
    int width_ = 300;
    int height_ = 260;

};


class AddRobotDialog : public RomCustomDialog 
{
    Q_OBJECT
public:
    explicit AddRobotDialog(QWidget* parent = nullptr, int width = 300, int height = 260);

    //QString robotName() const;
    QString robotIP() const;
    QString getPort() const;
    void setPort(const QString& port);

    void animateShow()
    {
        // Center dialog over parent widget if available
        if (parentWidget()) {
            QRect parentRect = parentWidget()->geometry();
            int x = parentRect.center().x() - this->width() / 2;
            int y = parentRect.center().y() - this->height() / 2;
            this->move(x, y);
        }
        QPropertyAnimation *dialogAnim = new QPropertyAnimation(this, "geometry");
        dialogAnim->setDuration(500);
        QRect startRect = QRect(this->x(), parentWidget() ? parentWidget()->geometry().bottom() : QApplication::primaryScreen()->geometry().bottom(), this->width(), this->height());
        QRect endRect = this->geometry();
        dialogAnim->setStartValue(startRect);
        dialogAnim->setEndValue(endRect);
        dialogAnim->setEasingCurve(QEasingCurve::OutCubic);
        this->show();
        dialogAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QLineEdit* nameEdit_;
    QLineEdit* ipEdit_;
    QLineEdit* reserveEdit_;
};


class SaveMapDialog : public RomCustomDialog 
{
    Q_OBJECT
public:
    explicit SaveMapDialog(QWidget* parent = nullptr, int width = 300, int height = 260);

    QString getMapName() const { return mapNameLineEdit_->text(); }
    
    void animateShow()
    {
        // Center dialog over parent widget if available
        if (parentWidget()) {
            QRect parentRect = parentWidget()->geometry();
            int x = parentRect.center().x() - this->width() / 2;
            int y = parentRect.center().y() - this->height() / 2;
            this->move(x, y);
        }
        QPropertyAnimation *dialogAnim = new QPropertyAnimation(this, "geometry");
        dialogAnim->setDuration(500);
        QRect startRect = QRect(this->x(), parentWidget() ? parentWidget()->geometry().bottom() : QApplication::primaryScreen()->geometry().bottom(), this->width(), this->height());
        QRect endRect = this->geometry();
        dialogAnim->setStartValue(startRect);
        dialogAnim->setEndValue(endRect);
        dialogAnim->setEasingCurve(QEasingCurve::OutCubic);
        this->show();
        dialogAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QLineEdit* mapNameLineEdit_;
};


class OpenMapDialog : public RomCustomDialog 
{
    Q_OBJECT
public:
    explicit OpenMapDialog(QWidget* parent = nullptr, int width = 300, int height = 260);
    // Provide list population and selection retrieval
    void setMapList(const QStringList& names);
    QString getMapName() const;
    
    void animateShow()
    {
        // Center dialog over parent widget if available
        if (parentWidget()) {
            QRect parentRect = parentWidget()->geometry();
            int x = parentRect.center().x() - this->width() / 2;
            int y = parentRect.center().y() - this->height() / 2;
            this->move(x, y);
        }
        QPropertyAnimation *dialogAnim = new QPropertyAnimation(this, "geometry");
        dialogAnim->setDuration(500);
        QRect startRect = QRect(this->x(), parentWidget() ? parentWidget()->geometry().bottom() : QApplication::primaryScreen()->geometry().bottom(), this->width(), this->height());
        QRect endRect = this->geometry();
        dialogAnim->setStartValue(startRect);
        dialogAnim->setEndValue(endRect);
        dialogAnim->setEasingCurve(QEasingCurve::OutCubic);
        this->show();
        dialogAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QListWidget* mapListWidget_ {nullptr};
    QStringList mapList_;
};


class LoginDialog : public RomCustomDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr, int width = 300, int height = 260);

    QString getUserName() const;
    QString getPassword() const;
    
    void animateShow()
    {
        // Center dialog over parent widget if available
        if (parentWidget()) {
            QRect parentRect = parentWidget()->geometry();
            int x = parentRect.center().x() - this->width() / 2;
            int y = parentRect.center().y() - this->height() / 2;
            this->move(x, y);
        }
        QPropertyAnimation *dialogAnim = new QPropertyAnimation(this, "geometry");
        dialogAnim->setDuration(500);
        QRect startRect = QRect(this->x(), parentWidget() ? parentWidget()->geometry().bottom() : QApplication::primaryScreen()->geometry().bottom(), this->width(), this->height());
        QRect endRect = this->geometry();
        dialogAnim->setStartValue(startRect);
        dialogAnim->setEndValue(endRect);
        dialogAnim->setEasingCurve(QEasingCurve::OutCubic);
        this->show();
        dialogAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QLineEdit* userNameLineEdit_;
    QLineEdit* passwordLineEdit_;
};


class ConfirmDialog : public RomCustomDialog 
{
    Q_OBJECT
public:
    explicit ConfirmDialog(QWidget* parent = nullptr, 
        int width = 300, 
        int height = 260, 
        const QString& title = "add your confirmation message",
        const QString& data = "The quick brown fox jumps over the lazy dog."
    );

    QString getTitleName() const { return title_; }
    
    void animateShow()
    {
        // Center dialog over parent widget if available
        if (parentWidget()) {
            QRect parentRect = parentWidget()->geometry();
            int x = parentRect.center().x() - this->width() / 2;
            int y = parentRect.center().y() - this->height() / 2;
            this->move(x, y);
        }
        QPropertyAnimation *dialogAnim = new QPropertyAnimation(this, "geometry");
        dialogAnim->setDuration(500);
        QRect startRect = QRect(this->x(), parentWidget() ? parentWidget()->geometry().bottom() : QApplication::primaryScreen()->geometry().bottom(), this->width(), this->height());
        QRect endRect = this->geometry();
        dialogAnim->setStartValue(startRect);
        dialogAnim->setEndValue(endRect);
        dialogAnim->setEasingCurve(QEasingCurve::OutCubic);
        this->show();
        dialogAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    QString title_;
    QString data_;
};


class AddNavigationPointDialog : public RomCustomDialog
{
    Q_OBJECT
public:
    explicit AddNavigationPointDialog(QWidget* parent = nullptr, int width = 300, int height = 260, const QString& point_type_label="default");
    
    QLineEdit* addPointName;

    void animateShow()
    {
        // Center dialog over parent widget if available
        if (parentWidget()) {
            QRect parentRect = parentWidget()->geometry();
            int x = parentRect.center().x() - this->width() / 2;
            int y = parentRect.center().y() - this->height() / 2;
            this->move(x, y);
        }
        QPropertyAnimation *dialogAnim = new QPropertyAnimation(this, "geometry");
        dialogAnim->setDuration(500);
        QRect startRect = QRect(this->x(), parentWidget() ? parentWidget()->geometry().bottom() : QApplication::primaryScreen()->geometry().bottom(), this->width(), this->height());
        QRect endRect = this->geometry();
        dialogAnim->setStartValue(startRect);
        dialogAnim->setEndValue(endRect);
        dialogAnim->setEasingCurve(QEasingCurve::OutCubic);
        this->show();
        // Focus the line edit as soon as the dialog appears
        if (addPointName) {
            addPointName->setFocus();
            addPointName->setCursorPosition(addPointName->text().length());
        }
        dialogAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }

private:
    
};


class PowerOffDialog : public QDialog 
{
    Q_OBJECT
public:
    explicit PowerOffDialog(QWidget* parent = nullptr, int width = 410, int height = 260) 
    : QDialog(parent), width_(width), height_(height)
    {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Tool);
    setWindowModality(Qt::WindowModal); // keep input focus/modal relative to parent
        setFixedSize(width_, height_);
        setStyleSheet(
		QString(
			"QDialog {"
			"    border-radius: 12px;"
			"    border: none;"
			"    background-color: #222;"
			"    font-size: 12px;"
			"    font-family: 'SF Pro Text';"
			"    color: white;"
			"}"
			"QLabel {"
			"    color: white;"
			"}"
			"QLabel#subtitleLabel {"
			"    font-size: 14px;"
			"    color: white;"
			"}"
		));

        QLabel* subtitleLabel = new QLabel("Are you sure you want to shutdown robot now?", this);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setGeometry(75, 15, 260, 80);
        subtitleLabel->setAlignment(Qt::AlignCenter);
        subtitleLabel->setWordWrap(true);
        subtitleLabel->setStyleSheet("font-size: 16px;"); 
        

        cancelBtn = std::make_unique<QPushButton>("Cancel", this);
        cancelBtn->setGeometry(45, 102, 100, 36);
        cancelBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #4a4843;"
            "    color: white;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "    font-family: 'SF Pro Text';"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: #262522;"
            "}"
        );
        
        restartBtn = std::make_unique<QPushButton>("Restart", this);
        restartBtn->setGeometry(155, 102, 100, 36);
        restartBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: #007aff;"
            "    color: white;"
            "    border-radius: 8px;"
            "    padding: 10px 20px;"
            "    font-size: 14px;"
            "    font-family: 'SF Pro Text';"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: #005bb5;"
            "}"
        );
        
        shutdownBtn = std::make_unique<QPushButton>("Shutdown", this);
        shutdownBtn->setGeometry(265, 102, 100, 36);
        shutdownBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: red;"
            "    color: white;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "    font-family: 'SF Pro Text';"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: #cc4d4dff;"
            "}"
        );
    }

protected:
    std::unique_ptr<QPushButton> cancelBtn;
    std::unique_ptr<QPushButton> restartBtn;
    std::unique_ptr<QPushButton> shutdownBtn;

private:
    int width_ = 410;
    int height_ = 260;

};


// ----------------------------------------------------- MapWidget (QGraphicsView-based map renderer)

class RomMap; // forward declare
class RomLaser; // forward declare
class RomPose2D; // forward declare

class MapWidget : public QGraphicsView {
    Q_OBJECT
public:
    explicit MapWidget(QWidget* parent = nullptr);

    // Update the displayed occupancy grid; cheap if same size.
    void setMap(const RomMap& map);
    void clearMap();

    // Fit the current map pixmap to the view keeping aspect ratio.
    void fitToView();

    // Optional: control whether the view auto-fits when a new map size arrives
    void setAutoFitEnabled(bool enabled) { autoFit_ = enabled; }
    bool autoFitEnabled() const { return autoFit_; }

    // Update extra frames (odom, base_footprint) using TF and current map metadata
    void updateFrames(const class RomTF& tf, const class RomMap& map);
    void updateFrameswithLaser(const RomTF& tf, const RomMap& map, const RomLaser& scan, const RomPose2D& mbfp);
    // Laser
    void updateFrontLaser(const RomLaser& scan);
    // Clear/hide/remove the front laser overlay item
    void clearFrontLaser();

    // Toggle/show axes overlays (view, map origin, odom, base_footprint)
    void setAxesVisible(bool visible);
    bool axesVisible() const { return axesVisible_; }

    // Control robot icon visibility independently of axes toggle
    void setRobotIconVisible(bool visible);

    // Enable/disable interactive waypoint placement mode
    void setWaypointPlacementEnabled(bool enabled) { waypointModeEnabled_ = enabled; setDragMode(enabled ? QGraphicsView::NoDrag : QGraphicsView::ScrollHandDrag); }
    bool waypointPlacementEnabled() const { return waypointModeEnabled_; }

    // Add a persistent waypoint icon to the scene at image/scene coordinates
    // Optionally provide a name to show as a tooltip
    void addWaypointMarker(double image_x_px, double image_y_px, double image_theta_deg, const QString& name = QString());

    // Remove all waypoint markers that were added via addWaypointMarker
    void clearWaypointMarkers();

signals:
    // Emitted when user releases mouse after placing/rotating a waypoint
    // image coords are scene/image pixels (pixmap centered), theta in degrees; world theta in radians
    void waypointPlaced(double image_x_px, double image_y_px, double image_theta_deg,
                        double world_x_m, double world_y_m, double world_theta_rad);

protected:
    void resizeEvent(QResizeEvent* e) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* scene_{nullptr};
    QGraphicsPixmapItem* mapItem_{nullptr};
    QGraphicsItem* viewAxes_{nullptr};
    QGraphicsItem* mapOriginAxes_{nullptr};
    QGraphicsItem* odomAxes_{nullptr};
    QGraphicsItem* baseFootprintAxes_{nullptr};
    QGraphicsItem* frontLaserItem_{nullptr};
    //QGraphicsItem* rearLaserItem_{nullptr};
    //QGraphicsItem* depthCameraFuseLaserItem_{nullptr};    


    class RomRobotUi* robotUi_{nullptr};
    QSize lastMapSize_{};
    bool autoFit_{true};
    bool userZoomed_{false};
    double minScale_{0.1};
    double maxScale_{10.0};
    bool axesVisible_{false};

    // Waypoint placement state and cached map metadata
    bool waypointModeEnabled_{false};
    bool waypointDragging_{false};
    QPointF waypointPressScene_{};
    double waypointAngleDeg_{0.0};
    QGraphicsPixmapItem* waypointPreview_{nullptr};

    // Cache for conversions between image(scene) and world
    int mapWidth_{0};
    int mapHeight_{0};
    double mapResolution_{0.0};
    double mapOriginX_{0.0};
    double mapOriginY_{0.0};
};


// ----------------------------------------------------- NotificationWidget
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

class NotificationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationWidget(QWidget *parent = nullptr);
    void addNotiInstruction(const QString& title, const QString& description, const RomNotificationLevel& log_level);
    int LEFT_PANEL_WIDTH = 240;

private:
    QLabel* createHeaderLabel(const QString& text, const QString& color);
    QFrame* createInstructionItem(const QString& title, const QString& description, const QString& titleColor);

    int height_ = 100;
    int width_ = 700;
    

protected:

};



// ----------------------------------------------------- TopBarWidget
class TopBarWidget : public QWidget {
    Q_OBJECT
public:
    explicit TopBarWidget(QWidget* parent = nullptr);
    QList<QPushButton*> buttons() const { return buttons_; }
    
    void setActiveStyle();   
    void setInactiveStyle(); 
    // Per-button versions
    void setActiveStyle(QPushButton* btn);
    void setInactiveStyle(QPushButton* btn);
    // Alias spelling requested
    inline void setInActiveStyle(QPushButton* btn) { setInactiveStyle(btn); }
protected:
    void setupUi();
private:
    QList<QPushButton*> buttons_;
};

class RightBarWidget : public QWidget {
    Q_OBJECT
public:
    explicit RightBarWidget(QWidget* parent = nullptr);
    QList<QPushButton*> buttons() const { return buttons_; }

    void setActiveStyle();
    void setInactiveStyle();
    // Per-button versions
    void setActiveStyle(QPushButton* btn);
    void setInactiveStyle(QPushButton* btn);
    // Alias spelling requested
    inline void setInActiveStyle(QPushButton* btn) { setInactiveStyle(btn); }
protected:
    void setupUi();
private:
    QList<QPushButton*> buttons_;
};

// ----------------------------------------------------- Virtual Joystick Widget
class VirtualJSWidget : public QWidget {
    Q_OBJECT
public:
    enum class AxisMode { VerticalForward, HorizontalStrafe };
    explicit VirtualJSWidget(QWidget* parent = nullptr, AxisMode mode = AxisMode::VerticalForward);
    // Current forward value in [-1, 1] (forward=+1, backward=-1)
    double forwardValue() const { return lastForward_; }
    // Current strafe value in [-1, 1] (right=+1, left=-1)
    double strafeValue() const { return lastStrafe_; }
signals:
    // Emitted whenever knob moves. forward in [-1,1] (up=+1, down=-1).
    void forwardChanged(double forward);
    // Optional: both axes if needed in future (forward, strafe)
    void axesChanged(double forward, double strafe);
    // Emitted when horizontal mode is used. strafe in [-1,1] (right=+1, left=-1)
    void strafeChanged(double strafe);
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    void setupUi();
    void updateGeometryCache();
    void setKnobFromPoint(const QPoint& p);
    QPointF clampToRadius(const QPointF& v, double r) const;

private:
    QPointF center_{0,0};
    double radius_{0.0};
    QPointF knobOffset_{0,0}; // relative to center_
    bool dragging_{false};
    double lastForward_{0.0};
    double lastStrafe_{0.0};
    AxisMode orientation_{AxisMode::VerticalForward};
};
#endif // CUSTOM_DESIGN_H
