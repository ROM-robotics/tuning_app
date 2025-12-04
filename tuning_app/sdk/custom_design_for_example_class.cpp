
#include "custom_design.h"
//#include "style_sheet.h"
#include <QLabel>
#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QIcon>
#include <QSize>
#include <QToolTip>
#include <cmath>
//#include "robot/rom_map.hpp"
#include "rom_map.hpp"

// ===================================================== AddRobotDialog
AddRobotDialog::AddRobotDialog(QWidget* parent, int width, int height) 
	: RomCustomDialog(parent, width, height)
{
	QLabel* subtitleLabel = new QLabel("Add New Robot", this);
	subtitleLabel->setObjectName("subtitleLabel");
	subtitleLabel->setGeometry(20, 15, 260, 20);
	subtitleLabel->setAlignment(Qt::AlignCenter);
	subtitleLabel->setStyleSheet("font-size: 16px;"); 

	// nameEdit_ = new QLineEdit(this);
	// nameEdit_->setObjectName("robotNameEdit");
	// nameEdit_->setPlaceholderText("Robot Name");
	// nameEdit_->setGeometry(45, 55, 210, 32);
	// nameEdit_->setStyleSheet(
	// 	"QLineEdit {"
	// 	"    padding: 6px;" 
	// 	"    border-radius: 6px;"
	// 	"    font-family: 'SF Pro Text';"
	// 	"    font-size: 14px;"
	// 	"    color: white;"
	// 	"    background-color: #555;"
	// 	"    border: none;"
	// 	"}"
	// 	"QLineEdit:focus {"
	// 	"    border: 1px solid white;"
	// 	"}"
	// );

	ipEdit_ = new QLineEdit(this);
	ipEdit_->setObjectName("robotIpEdit");
	ipEdit_->setPlaceholderText("192.168.1.11");
	QRegularExpression ipRegex(
	R"((25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d))");
	QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(ipRegex, this);
	ipEdit_->setValidator(ipValidator);
	ipEdit_->setGeometry(45, 97, 210, 32);
	ipEdit_->setStyleSheet(
		"QLineEdit {"
		"    padding: 6px;" 
		"    border-radius: 6px;"
		"    font-family: 'SF Pro Text';"
		"    font-size: 14px;"
		"    color: white;"
		"    background-color: #555;"
		"    border: none;"
		"}"
		"QLineEdit:focus {"
		"    border: 1px solid white;"
		"}"
	);

	reserveEdit_ = new QLineEdit(this);
	reserveEdit_->setObjectName("robotReserveEdit");
	reserveEdit_->setPlaceholderText("9090");
	reserveEdit_->setGeometry(45, 139, 210, 32);
	reserveEdit_->setStyleSheet(
		"QLineEdit {"
		"    padding: 6px;" 
		"    border-radius: 6px;"
		"    font-family: 'SF Pro Text';"
		"    font-size: 14px;"
		"    color: white;"
		"    background-color: #555;"
		"    border: none;"
		"}"
		"QLineEdit:focus {"
		"    border: 1px solid white;"
		"}"
	);

	connect(cancelBtn.get(), &QPushButton::clicked, this, &QDialog::reject);
	connect(connectBtn.get(), &QPushButton::clicked, this, &QDialog::accept);

}


QString AddRobotDialog::robotIP() const {
	return ipEdit_->text();
}


QString AddRobotDialog::getPort() const {
	return reserveEdit_->text();
}

void AddRobotDialog::setPort(const QString& port) {
	reserveEdit_->setText(port);
}

// ----------------------------------------------------- LoginDialog
LoginDialog::LoginDialog(QWidget* parent, int width, int height) 
	: RomCustomDialog(parent, width, height)
{
	QLabel* subtitleLabel = new QLabel("Login", this);
	subtitleLabel->setObjectName("subtitleLabel");
	subtitleLabel->setGeometry(20, 25, 260, 20);
	subtitleLabel->setAlignment(Qt::AlignCenter);
	subtitleLabel->setStyleSheet("font-size: 16px;"); 

	userNameLineEdit_ = new QLineEdit(this);
	userNameLineEdit_->setObjectName("userNameEdit");
	userNameLineEdit_->setPlaceholderText("User Name");
	userNameLineEdit_->setGeometry(45, 80, 210, 32);
	userNameLineEdit_->setStyleSheet(
		"QLineEdit {"
		"    padding: 6px;" 
		"    border-radius: 6px;"
		"    font-family: 'SF Pro Text';"
		"    font-size: 14px;"
		"    color: white;"
		"    background-color: #555;"
		"    border: none;"
		"}"
		"QLineEdit:focus {"
		"    border: 1px solid white;"
		"}"
	);

	passwordLineEdit_ = new QLineEdit(this);
	passwordLineEdit_->setObjectName("passwordEdit");
	passwordLineEdit_->setPlaceholderText("Password");
	passwordLineEdit_->setEchoMode(QLineEdit::Password);
	passwordLineEdit_->setGeometry(45, 122, 210, 32);
	passwordLineEdit_->setStyleSheet(
		"QLineEdit {"
		"    padding: 6px;" 
		"    border-radius: 6px;"
		"    font-family: 'SF Pro Text';"
		"    font-size: 14px;"
		"    color: white;"
		"    background-color: #555;"
		"    border: none;"
		"}"
		"QLineEdit:focus {"
		"    border: 1px solid white;"
		"}"
	);


	connect(cancelBtn.get(), &QPushButton::clicked, this, &QDialog::reject);
	connect(connectBtn.get(), &QPushButton::clicked, this, &QDialog::accept);

}


QString LoginDialog::getUserName() const {
	return userNameLineEdit_->text();
}


QString LoginDialog::getPassword() const {
	return passwordLineEdit_->text();
}


// ----------------------------------------------------- SaveMapDialog
SaveMapDialog::SaveMapDialog(QWidget* parent, int width, int height) 
	: RomCustomDialog(parent, width, height)
{
	QLabel* subtitleLabel = new QLabel("Save Map", this);
	subtitleLabel->setGeometry(20, 25, 260, 20);
	subtitleLabel->setAlignment(Qt::AlignCenter);
	subtitleLabel->setStyleSheet("font-size: 16px;"); 

	mapNameLineEdit_ = new QLineEdit(this);
	mapNameLineEdit_->setObjectName("mapNameEdit");
	mapNameLineEdit_->setPlaceholderText("Enter Map Name");
	mapNameLineEdit_->setGeometry(45, 97, 210, 32); 
	mapNameLineEdit_->setStyleSheet(
		"QLineEdit {"
		"    padding: 6px;" 
		"    border-radius: 6px;"
		"    font-family: 'SF Pro Text';"
		"    font-size: 14px;"
		"    color: white;"
		"    background-color: #555;"
		"    border: none;"
		"}"
		"QLineEdit:focus {"
		"    border: 1px solid white;"
		"}"
	);

	this->connectBtn->setText("Save");
	connect(cancelBtn.get(), &QPushButton::clicked, this, &QDialog::reject);
	connect(connectBtn.get(), &QPushButton::clicked, this, &QDialog::accept);

}


// ----------------------------------------------------- ConfirmDialog
ConfirmDialog::ConfirmDialog(QWidget* parent, int width, int height, const QString& title, const QString& data) 
	: RomCustomDialog(parent, width, height), title_(title), data_(data)
{
	QLabel* subtitleLabel = new QLabel(this->title_, this);
	subtitleLabel->setGeometry(20, 25, 260, 20);
	subtitleLabel->setAlignment(Qt::AlignCenter);
	subtitleLabel->setStyleSheet("font-size: 16px;"); 

	QLabel* dataLabel = new QLabel(this->data_, this);
	dataLabel->setGeometry(20, 45, 260, 140);
	dataLabel->setAlignment(Qt::AlignCenter);
	dataLabel->setWordWrap(true);
	dataLabel->setStyleSheet("font-size: 13px; color: #ccc;");
	
	this->connectBtn->setText("Confirm");

	connect(cancelBtn.get(), &QPushButton::clicked, this, &QDialog::reject);
	connect(connectBtn.get(), &QPushButton::clicked, this, &QDialog::accept);

}


// ----------------------------------------------------- OpenMapDialog
OpenMapDialog::OpenMapDialog(QWidget* parent, int width, int height) 
	: RomCustomDialog(parent, width, height)
{
	QLabel* subtitleLabel = new QLabel("Open Map", this);
	subtitleLabel->setGeometry(20, 25, 260, 20);
	subtitleLabel->setAlignment(Qt::AlignCenter);
	subtitleLabel->setStyleSheet("font-size: 16px;"); 

	// List of maps
	mapListWidget_ = new QListWidget(this);
	mapListWidget_->setObjectName("mapListWidget");
	// Compute height so it never overlaps bottom buttons
	const int topMargin = 60;               // below title
	const int sideMargin = 20 + 10;         // visual breathing
	const int bottomSafe = 84;              // room for buttons + gap
	mapListWidget_->setGeometry(sideMargin, topMargin,
								this->width() - (sideMargin * 2),
								this->height() - (topMargin + bottomSafe));
	// iPhone-like light list with separators and larger row height
	mapListWidget_->setStyleSheet(
		"QListWidget {"
		"  background: #f2f2f7;"           // grouped list background
		"  color: black;"
		"  border: none;"
		"  border-radius: 10px;"
		"  font-family: 'SF Pro Text';"
		"  font-size: 14px;"               // smaller font per request
		"}"
		"QListWidget::item {"
		"  margin: 0;"
		"  padding: 4px 12px;"             // compact rows; height ~= font + 8px
		"  background: white;"
		"  border-bottom: 1px solid rgba(0,0,0,0.1);"
		"}"
		"QListWidget::item:selected {"
		"  background: rgba(0,122,255,0.22);"
		"  color: black;"
		"}"
		"QListWidget::item:hover {"
		"  background: rgba(0,0,0,0.04);"
		"}"
	);

	connectBtn->setText("Open");
	connect(cancelBtn.get(), &QPushButton::clicked, this, &QDialog::reject);
	connect(connectBtn.get(), &QPushButton::clicked, this, &QDialog::accept);

	// Accept when double-clicking an item
	connect(mapListWidget_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem*){
		this->accept();
	});

	// Align buttons with the list box edges and widen them equally
	{
		const QRect listRect = mapListWidget_->geometry();
		const int spacing = 16;           // gap between buttons
		const int btnH = 38;              // slightly taller than before
		const int marginBottom = 20;      // bottom padding
		const int y = this->height() - marginBottom - btnH;
		const int btnW = (listRect.width() - spacing) / 2;
		if (cancelBtn) cancelBtn->setGeometry(listRect.left(), y, btnW, btnH);
		if (connectBtn) connectBtn->setGeometry(listRect.left() + btnW + spacing, y, btnW, btnH);
	}
}

QString OpenMapDialog::getMapName() const
{
	if (!mapListWidget_) return QString();
	QListWidgetItem* it = mapListWidget_->currentItem();
	return it ? it->text() : QString();
}

void OpenMapDialog::setMapList(const QStringList& names)
{
	mapList_ = names;
	if (!mapListWidget_) return;
	mapListWidget_->clear();
	mapListWidget_->addItems(names);
	// With compact padding and 14px font, row height is tight to font size; no forced size.
	if (mapListWidget_->count() > 0) {
		mapListWidget_->setCurrentRow(0);
	}
}


// ----------------------------------------------------- AddNavigationPointDialog
AddNavigationPointDialog::AddNavigationPointDialog(QWidget* parent, int width, int height, const QString& point_type_label) 
	: RomCustomDialog(parent, width, height)
{
	QLabel* subtitleLabel = new QLabel(point_type_label, this);
	subtitleLabel->setGeometry(20, 25, 260, 20);
	subtitleLabel->setAlignment(Qt::AlignCenter);
	subtitleLabel->setStyleSheet("font-size: 16px;"); 

	addPointName = new QLineEdit(this);
	addPointName->setObjectName("addPointNameLineEdit");
	addPointName->setPlaceholderText("Enter Point Name");
	addPointName->setGeometry(45, 97, 210, 32);
	addPointName->setStyleSheet(
		"QLineEdit {"
		"    padding: 6px;" 
		"    border-radius: 6px;"
		"    font-family: 'SF Pro Text';"
		"    font-size: 14px;"
		"    color: white;"
		"    background-color: #555;"
		"    border: none;"
		"}"
		"QLineEdit:focus {"
		"    border: 1px solid white;"
		"}"
	);
	// Ensure the name field is focused and ready for typing
	addPointName->setFocus();
	addPointName->setCursorPosition(addPointName->text().length());
	this->connectBtn->setText("Add");

	connect(cancelBtn.get(), &QPushButton::clicked, this, &QDialog::reject);
	connect(connectBtn.get(), &QPushButton::clicked, this, &QDialog::accept);

}


// ===================================================== NotificationWidget
NotificationWidget::NotificationWidget(QWidget *parent) : QWidget(parent)
{
    
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    
    setAttribute(Qt::WA_TranslucentBackground);

    
    setFixedSize(width_, height_); 

    
    setStyleSheet(R"(
        NotificationWidget {
            background-color: gray;
            border-radius: 8px;
            padding: 10px;
            color: black;
			font-family: 'SF Pro Text';
        }
    )");
    this->setStyleSheet("background-color: rgba(255,255,255,180); border-radius: 10px; border: none;");
}


QLabel* NotificationWidget::createHeaderLabel(const QString& text, const QString& color)
{
    QLabel* label = new QLabel(text);
    
    label->setObjectName("SectionHeaderLabel");
    label->setStyleSheet(QString("color: %1; font-weight: bold; margin-top: 10px;").arg(color));
    return label;
}


QFrame* NotificationWidget::createInstructionItem(const QString& title, const QString& description, const QString& titleColor)
{
    
    QFrame* frame = new QFrame();
	frame->setStyleSheet("padding: 5px 20px 5px 20px; border-radius: 10px;");
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(0, 5, 0, 5);
    layout->setSpacing(2);

    
    QLabel* titleLabel = new QLabel(QString("<b>%1</b>").arg(title));
    titleLabel->setStyleSheet(QString("color: %1; font-family: 'SF Pro Text'; padding: 5px 20px 0px 20px; background-color: none;").arg(titleColor));
    layout->addWidget(titleLabel);

    
    QLabel* descLabel = new QLabel(description);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: black; font-family: 'SF Pro Text'; font-size: 12px; padding: 0px 20px 5px 20px; background-color: none;");
    layout->addWidget(descLabel);

    return frame;
}


void NotificationWidget::addNotiInstruction(const QString& title, const QString& description, const RomNotificationLevel& log_level)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(0);

    // example use for createHeaderLabel
    // QLabel* title = createHeaderLabel("Edit Mode Instructions", "orange");
    // mainLayout->addWidget(title);
    // mainLayout->addSpacing(5);

    QString titleColor;
	switch (log_level) {
		case RomNotificationLevel::debug: titleColor = "green"; break;
		case RomNotificationLevel::info: titleColor = "blue"; break;
		case RomNotificationLevel::warn: titleColor = "yellow"; break;
		case RomNotificationLevel::error: titleColor = "red"; break;
		case RomNotificationLevel::fatal: titleColor = "red"; break;
		default: titleColor = "cyan"; break;
	}

    QFrame* navPoint = createInstructionItem(title, description, titleColor);
    mainLayout->addWidget(navPoint);   
    
    mainLayout->addStretch(1);
}


// ----------------------------------------------------- Overlay Widgets
// Shared style used by overlay buttons
static QString romOverlayButtonStyle() {
	return QString(R"(QPushButton {
		background: #2f3840;
		border: 1px solid #3f4a55;
		border-radius: 6px;
		color: #d3d9e3;
		font-size: 12px;
		font-family: 'SF Pro Text';
		padding: 0px;
	}
	QPushButton:hover { background: #3a4550; border: 1px solid #4a5865; }
	QPushButton:pressed { background: #1f262c; border: 1px solid #566573; }
	QPushButton:disabled { background: #20262b; color: #6e767d; border: 1px solid #2d363d; })");
}

static QString romOverlayButtonClickedStyle() {
	return QString(R"(QPushButton {
		background: #1f262c;
		border: 1px solid #566573;
		border-radius: 6px;
		color: white;
		font-size: 12px;
		font-family: 'SF Pro Text';
		padding: 0px;
	}
	QPushButton:hover { background: #3a4550; border: 1px solid #4a5865; }
	QPushButton:pressed { background: #1f262c; border: 1px solid #566573; }
	QPushButton:disabled { background: #20262b; color: #6e767d; border: 1px solid #2d363d; })");
}

// ----------------------------------------------------- TopBarWidget
TopBarWidget::TopBarWidget(QWidget* parent) : QWidget(parent) {
	setObjectName("TopBarWidget");
	setAttribute(Qt::WA_TransparentForMouseEvents, false);
	setupUi();
}

void TopBarWidget::setupUi() {
	auto* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(4);

	const QVector<QString> iconFiles = {
		"go_waypoint_128.png",       // 1
		"go_service_point_128.png",          // 2
		"go_patrol_points_128.png",    // 3
		"reserve_128.png",    // 4 
		"reserve_128.png",    // 5 
		"reserve_128.png",       // 6
		"add_points_128.png",   // 7
		"add_points_128.png",  // 8
		"add_points_128.png", // 9
	};

	const QVector<QString> objNames = {
		"topBarGoWaypointsBtn", 
		"topBarGoServicePointsBtn", 
		"topBarGoPatrolPointsBtn", 
		"topBarReservedBtn1", 
		"topBarReservedBtn2", 
		"topBarReservedBtn3", 
		"topBarWaypointsModeBtn", 
		"topBarServicePointsModeBtn", 
		"topBarPatrolPointsModeBtn", 
	};

	QList<QString> buttonTexts = { "       Go\n Waypoints", "       Go\n Service Points", "       Go\n Patrol Points", "Get\n Waypoints", "Get\n Service Points", "Get\n Patrol Points", "Waypoints\n    Mode", "Service\n  Mode", "Patrol\nMode" };
	layout->addStretch();
	const int size = 40;
	const int width = 110;
	const int iconSize = 24;
	const int iconPad = 4; // leave some padding

	for (int i = 0; i < 9; ++i) {
		auto* btn = new QPushButton(buttonTexts[i], this); // စာပြသည်။
		//btn->setObjectName(QString("topBarBtn%1").arg(i));
		btn->setObjectName(QString(buttonTexts[i]));
		btn->setFixedSize(width, size);
		btn->setCursor(Qt::PointingHandCursor);
		btn->setFocusPolicy(Qt::NoFocus);
		// Use shared overlay style, but force a black background for TopBar buttons
		btn->setStyleSheet(romOverlayButtonStyle());

		const QString resPath = QString(":/icons/%1").arg(iconFiles[i]);
		btn->setIcon(QIcon(resPath));
		btn->setIconSize(QSize(iconSize - iconPad, iconSize - iconPad));

		buttons_.append(btn);
		layout->addWidget(btn);
	}
	setLayout(layout);

	// Initialize all buttons to the inactive/default style
	setInactiveStyle();
}

void TopBarWidget::setActiveStyle()
{
	// Apply the "active" look to all buttons
	for (auto* btn : buttons_) {
		if (!btn) continue;
		btn->setStyleSheet(romOverlayButtonClickedStyle());
	}
}

void TopBarWidget::setInactiveStyle()
{
	// Apply the default/inactive look to all buttons
	for (auto* btn : buttons_) {
		if (!btn) continue;
		btn->setStyleSheet(romOverlayButtonStyle());
	}
}

void TopBarWidget::setActiveStyle(QPushButton* btn)
{
	if (!btn) return;
	btn->setStyleSheet(romOverlayButtonClickedStyle());
}

void TopBarWidget::setInactiveStyle(QPushButton* btn)
{
	if (!btn) return;
	btn->setStyleSheet(romOverlayButtonStyle());
}

// ----------------------------------------------------- RightBarWidget
RightBarWidget::RightBarWidget(QWidget* parent) : QWidget(parent) {
	setObjectName("RightBarWidget");
	setupUi();
}

void RightBarWidget::setupUi() {
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(6,8,6,8);
	layout->setSpacing(10);

	// Icons from resources/icons/icons.qrc (prefix "/icons")
	const QVector<QString> iconFiles = {
		"map_edit_wall.png",       // 1
		"eraser_128.png",          // 2
		"map_edit_reserve.png",    // 3
		"map_edit_coordinate_axes.png",    // 4 (placeholder)
		"map_edit_joystick.png",    // 5 (placeholder)
		"map_edit_plug.png",       // 6
		"map_edit_relocate.png",   // 7
		"map_edit_estop_red.png",  // 8
		"map_edit_cancel_navi.png" // 9
	};

	const QVector<QString> iconTooltips = {
		"Add Walls", 
		"Erase", 
		"Reserved", 
		"Show TF Axes", 
		"Joystick", 
		"Go Charging Station", 
		"Relocate Robot", 
		"Emergency Stop", 
		"Cancel Navigation"
	};

	const QVector<QString> objNames = {
		"rightBarAddWallBtn", 
		"rightBarEraseBtn", 
		"rightBarReservedBtn1", 
		"rightBarReservedBtn2", 
		"rightBarReservedBtn3", 
		"rightBarGoChargingStationBtn", 
		"rightBarRelocateRobotBtn", 
		"rightBarEmergencyStopBtn", 
		"rightBarCancelNavigationBtn"
	};

	const int size = 40;
	const int iconPad = 8; // leave some padding
	for (int i = 0; i < iconFiles.size(); ++i) {
		auto* btn = new QPushButton(QString(), this);
		btn->setObjectName(objNames[i]);
		btn->setFixedSize(size, size);
		btn->setCursor(Qt::PointingHandCursor);
		btn->setFocusPolicy(Qt::NoFocus);
		btn->setStyleSheet(romOverlayButtonStyle());

		const QString resPath = QString(":/icons/%1").arg(iconFiles[i]);
		btn->setIcon(QIcon(resPath));
		btn->setIconSize(QSize(size - iconPad, size - iconPad));

	const QString tooltip = iconTooltips[i];
	btn->setToolTip(QString("<span style='font-size:11px;'>%1</span>").arg(tooltip));

		buttons_.append(btn);
		layout->addWidget(btn);
	}
	layout->addStretch();
	setLayout(layout);

	// Initialize all buttons to the inactive/default style
	setInactiveStyle();
}

void RightBarWidget::setActiveStyle()
{
	for (auto* btn : buttons_) {
		if (!btn) continue;
		btn->setStyleSheet(romOverlayButtonClickedStyle());
	}
}

void RightBarWidget::setInactiveStyle()
{
	for (auto* btn : buttons_) {
		if (!btn) continue;
		btn->setStyleSheet(romOverlayButtonStyle());
	}
}

void RightBarWidget::setActiveStyle(QPushButton* btn)
{
	if (!btn) return;
	btn->setStyleSheet(romOverlayButtonClickedStyle());
}

void RightBarWidget::setInactiveStyle(QPushButton* btn)
{
	if (!btn) return;
	btn->setStyleSheet(romOverlayButtonStyle());
}


// ----------------------------------------------------- VirtualJSWidget
VirtualJSWidget::VirtualJSWidget(QWidget* parent, AxisMode mode) : QWidget(parent), orientation_(mode) {
	setObjectName("VirtualJSWidget");
	setFixedSize(140,140);
	setupUi();
}

void VirtualJSWidget::setupUi() {
	setStyleSheet("background:transparent;");
	setAttribute(Qt::WA_TransparentForMouseEvents, false);
	setMouseTracking(true);
	updateGeometryCache();
}

void VirtualJSWidget::paintEvent(QPaintEvent* event) {
	QWidget::paintEvent(event);
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	QRectF outer(rect().adjusted(4,4,-4,-4));
	// Refresh geometry cache from current rect
	center_ = outer.center();
	radius_ = std::min(outer.width(), outer.height()) * 0.5;

	// Outer ring
	p.setPen(QPen(QColor(90,100,110),2));
	p.setBrush(QColor(80,90,100,40));
	p.drawEllipse(outer);

	// Draggable knob (moves relative to center_ with knobOffset_)
	const double knobR = radius_ * 0.27; // approx same look as 36px on 140px control
	QPointF kc = center_ + knobOffset_;
	p.setBrush(QColor(200,200,210,120));
	p.setPen(Qt::NoPen);
	p.drawEllipse(QRectF(kc.x()-knobR, kc.y()-knobR, knobR*2.0, knobR*2.0));
}

void VirtualJSWidget::updateGeometryCache()
{
	QRectF r = rect().adjusted(4,4,-4,-4);
	center_ = r.center();
	radius_ = std::min(r.width(), r.height()) * 0.5;
}

QPointF VirtualJSWidget::clampToRadius(const QPointF& v, double r) const
{
	const double len = std::hypot(v.x(), v.y());
	if (len <= r || len == 0.0) return v;
	const double scale = r / len;
	return QPointF(v.x()*scale, v.y()*scale);
}

void VirtualJSWidget::setKnobFromPoint(const QPoint& p)
{
	// Convert to offset from center and clamp within radius
	QPointF delta = QPointF(p) - center_;
	// Use 80% radius for knob travel to keep knob inside ring visually
	const double maxTravel = radius_ * 0.8;
	knobOffset_ = clampToRadius(delta, maxTravel);

	if (orientation_ == AxisMode::VerticalForward) {
		// Map vertical offset to forward in [-1,1]; up = +1, down = -1
		double forward = 0.0;
		if (maxTravel > 0.0) {
			forward = -knobOffset_.y() / maxTravel; // screen y-down, so negate
			if (forward > 1.0) forward = 1.0;
			if (forward < -1.0) forward = -1.0;
		}
		if (!qFuzzyCompare(1.0 + lastForward_, 1.0 + forward)) {
			lastForward_ = forward;
			emit forwardChanged(forward);
			emit axesChanged(forward, 0.0);
		}
	} else {
		// Horizontal mode: Map horizontal offset to strafe in [-1,1]; right=+1, left=-1
		double strafe = 0.0;
		if (maxTravel > 0.0) {
			strafe = knobOffset_.x() / maxTravel;
			if (strafe > 1.0) strafe = 1.0;
			if (strafe < -1.0) strafe = -1.0;
		}
		if (!qFuzzyCompare(1.0 + lastStrafe_, 1.0 + strafe)) {
			lastStrafe_ = strafe;
			emit strafeChanged(strafe*(-1.0)); // negate to match typical robot frame (left=+)
			emit axesChanged(0.0, strafe);
		}
	}
	update();
}

void VirtualJSWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		dragging_ = true;
		updateGeometryCache();
		setKnobFromPoint(event->pos());
		event->accept();
		return;
	}
	QWidget::mousePressEvent(event);
}

void VirtualJSWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging_) {
		setKnobFromPoint(event->pos());
		event->accept();
		return;
	}
	QWidget::mouseMoveEvent(event);
}

void VirtualJSWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (dragging_ && event->button() == Qt::LeftButton) {
		dragging_ = false;
		// Snap back to center and emit zero
		knobOffset_ = QPointF(0,0);
		if (orientation_ == AxisMode::VerticalForward) {
			if (!qFuzzyIsNull(lastForward_)) {
				lastForward_ = 0.0;
				emit forwardChanged(0.0);
				emit axesChanged(0.0, 0.0);
			}
		} else {
			if (!qFuzzyIsNull(lastStrafe_)) {
				lastStrafe_ = 0.0;
				emit strafeChanged(0.0);
				emit axesChanged(0.0, 0.0);
			}
		}
		update();
		event->accept();
		return;
	}
	QWidget::mouseReleaseEvent(event);
}


// ===================================================== Map QGraphicsView
static QImage romMapToImage_local(const RomMap& map)
{// Convert RomMap occupancy grid to grayscale QImage
	const int w = map.width();
	const int h = map.height();
	QImage img(w, h, QImage::Format_Grayscale8);
	if (w <= 0 || h <= 0) return img;

	const auto& data = map.data();
	const int expected = w * h;
	if ((int)data.size() < expected) {
		img.fill(127);
		return img;
	}

	for (int y = 0; y < h; ++y) {
		uchar* line = img.scanLine(h - 1 - y); // flip Y
		const int rowOff = y * w;
		for (int x = 0; x < w; ++x) {
			const int8_t occ = data[rowOff + x];
			uchar v = 127; // unknown
			if (occ >= 0) {
				int clamped = occ;
				if (clamped > 100) clamped = 100;
				v = static_cast<uchar>(255 - (clamped * 255) / 100);
			}
			line[x] = v;
		}
	}
	return img;
}

#include "rom_nav_design.hpp"
#include "rom_tf.hpp"

MapWidget::MapWidget(QWidget* parent)
	: QGraphicsView(parent)
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

	// Create the robot UI icon (above axes by default)
	if (!robotUi_) 
	{
		robotUi_ = new RomRobotUi();
		// If your robot.png points upwards, uncomment the next line to align with +X
		// robotUi_->setHeadingOffsetDeg(-90.0);
		robotUi_->setVisible(false); // follow axesVisible_ initially
		scene_->addItem(robotUi_);
	}

	// Add a small coordinate frame at the GraphicsView origin
	if (!viewAxes_) {
		auto* garphic_axes = new GraphicViewCoordinateFrame();
		garphic_axes->setPos(0, 0); // scene origin (current map center)
		scene_->addItem(garphic_axes);
		viewAxes_ = garphic_axes; // keep handle if we want to move/hide later
		viewAxes_->setVisible(axesVisible_);
	}
}

// Helper: scene(image) -> world(map meters) conversion (inverse of worldToScene_mapFrame)
static inline QPointF sceneToWorld_mapFrame(double scene_x, double scene_y,
											int w, int h, double res, double origin_x, double origin_y)
{
	if (w <= 0 || h <= 0 || res <= 0.0) return QPointF(0,0);
	// Convert scene (centered) to image top-left basis
	const double px_from_left = scene_x + static_cast<double>(w) / 2.0;
	const double py_from_top  = scene_y + static_cast<double>(h) / 2.0;
	// Invert mapping used in setMap/worldToScene_mapFrame
	const double wx = (px_from_left * res) + origin_x;
	const double wy = -((py_from_top * res) - ((static_cast<double>(h) - 1.0) * res + origin_y));
	return QPointF(wx, wy);
}

// Convert world (meters, map frame) to scene coordinates considering our centered pixmap and image Y-flip
static inline QPointF worldToScene_mapFrame(const RomMap& map, double wx, double wy)
{
	const int w = map.width();
	const int h = map.height();
	const double res = static_cast<double>(map.resolution());
	if (w <= 0 || h <= 0 || res <= 0.0) return QPointF(0,0);

	// pixel_x from left grows with +x; pixel_y from top grows downward
	const double pixel_x = wx / res; // world x to pixels relative to origin pixel (origin at (origin_x, origin_y))
	const double pixel_y_down = -wy / res; // because up in world should go to smaller image y (we use down-positive image y)

	// Convert from map's origin (at world origin) to full image with origin at (originX, originY)
	const double px_from_left = (-static_cast<double>(map.originX()) / res) + pixel_x;
	const double py_from_top  = (static_cast<double>(h) - 1.0) + (static_cast<double>(map.originY()) / res) + pixel_y_down;

	// Centered pixmap moves (0,0) to image center
	const double scene_x = px_from_left - static_cast<double>(w) / 2.0;
	const double scene_y = py_from_top  - static_cast<double>(h) / 2.0;
	return QPointF(scene_x, scene_y);
}

void MapWidget::updateFrames(const RomTF& tf, const RomMap& map)
{
	// Ensure items exist
	if (!odomAxes_) 
	{
		odomAxes_ = new LabeledLocalCoordinateFrame(QStringLiteral("odom"));
		scene_->addItem(odomAxes_);
		odomAxes_->setVisible(axesVisible_);
	}
	if (!baseFootprintAxes_) 
	{
		baseFootprintAxes_ = new LabeledLocalCoordinateFrame(QStringLiteral("base_footprint"));
		scene_->addItem(baseFootprintAxes_);
		baseFootprintAxes_->setVisible(axesVisible_);
	}

	// map->odom translation in meters
	const QVector3D& mo_t = tf.map_odom.translation;
	const QQuaternion& mo_q = tf.map_odom.rotation;
	// odom->base_footprint translation (compose to get map->base_footprint)
	const QVector3D& ob_t = tf.odom_base_footprint.translation;
	const QQuaternion& ob_q = tf.odom_base_footprint.rotation;

	// Positions in map frame
	const QPointF odomScene = worldToScene_mapFrame(map, mo_t.x(), mo_t.y());
	// Compose position: map->odom then odom->base_footprint
	const double bfx = mo_t.x() + ob_t.x();
	const double bfy = mo_t.y() + ob_t.y();
	const QPointF baseScene = worldToScene_mapFrame(map, bfx, bfy);

	// Extract yaw (2D) from quaternions for rotation (degrees for QGraphicsItem::setRotation)
	// auto yawFromQ = [](const QQuaternion& q){
	// 	// yaw around Z assuming quaternion (w,x,y,z)
	// 	const double siny_cosp = 2.0 * (static_cast<double>(q.scalar()) * q.z() + q.x() * q.y());
	// 	const double cosy_cosp = 1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());
	// 	return std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;
	// };
	auto yawFromQ = [](const QQuaternion& q){
		// yaw around Z assuming quaternion (w,x,y,z)
		const double siny_cosp = 2.0 * (static_cast<double>(q.scalar()) * q.z() + q.x() * q.y());
		const double cosy_cosp = 1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());
		return std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;
	};
	const double odomYawDeg = yawFromQ(mo_q);
	const QQuaternion mb_q = mo_q * ob_q; // compose rotations
	const double baseYawDeg = yawFromQ(mb_q);

	// Apply to items
	odomAxes_->setPos(odomScene);
	odomAxes_->setRotation(odomYawDeg);
	odomAxes_->setZValue(960.0);
	odomAxes_->setVisible(axesVisible_);

	baseFootprintAxes_->setPos(baseScene);
	baseFootprintAxes_->setRotation(baseYawDeg);
	baseFootprintAxes_->setZValue(970.0);
	baseFootprintAxes_->setVisible(axesVisible_);

	// Update robot UI icon using Option A converter (map meters -> scene/image px)
	if (robotUi_) {
		const int w = map.width();
		const int h = map.height();
		const double res = static_cast<double>(map.resolution());
		const double ox = static_cast<double>(map.originX());
		const double oy = static_cast<double>(map.originY());
		const auto pose = RomRobotUi::tfPoseToImagePose(bfx, bfy, baseYawDeg, w, h, res, ox, oy);

		robotUi_->setImagePosition(pose.x, pose.y, pose.theta_deg);
		// visibility controlled independently via setRobotIconVisible()
	}

	

	// // Keep the front laser item aligned with the robot base frame (position + yaw)
	// if (frontLaserItem_) {
	// 	frontLaserItem_->setPos(baseScene);
	// 	frontLaserItem_->setRotation(baseYawDeg);
	// 	frontLaserItem_->setZValue(965.0); // between odom (960) and base footprint (970)
	// 	frontLaserItem_->setVisible(axesVisible_);
	// }

}

void MapWidget::updateFrameswithLaser(const RomTF& tf, const RomMap& map, const RomLaser& scan, const RomPose2D& mbfp)
{
	// Ensure items exist
	if (!odomAxes_) 
	{
		odomAxes_ = new LabeledLocalCoordinateFrame(QStringLiteral("odom"));
		scene_->addItem(odomAxes_);
		odomAxes_->setVisible(axesVisible_);
	}
	if (!baseFootprintAxes_) 
	{
		baseFootprintAxes_ = new LabeledLocalCoordinateFrame(QStringLiteral("base_footprint"));
		scene_->addItem(baseFootprintAxes_);
		baseFootprintAxes_->setVisible(axesVisible_);
	}

	// map->odom translation in meters
	const QVector3D& mo_t = tf.map_odom.translation;
	const QQuaternion& mo_q = tf.map_odom.rotation;

	const double x = mbfp.x*(-1.0);
	const double y = mbfp.y*(-1.0);
	const double theta = mbfp.theta_deg;

	const QPointF odomScene = worldToScene_mapFrame(map, mo_t.x(), mo_t.y());
	const QPointF baseScene = worldToScene_mapFrame(map, x, y);

	// need to check yaw degrees from quaternion
	auto yawFromQ = [](const QQuaternion& q) 
	{
		// yaw around Z assuming quaternion (w,x,y,z)
		const double siny_cosp = 2.0 * (static_cast<double>(q.scalar()) * q.z() + q.x() * q.y());
		const double cosy_cosp = 1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());
		return std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;
	};
	const double odomYawDeg = yawFromQ(mo_q);

	// ----------------- const QQuaternion mb_q = mo_q * ob_q; // compose rotations
	// ----------------- const double baseYawDeg = yawFromQ(mb_q);

	const double baseYawDeg = theta;

	// Apply to items
	odomAxes_->setPos(odomScene);
	//odomAxes_->setRotation(odomYawDeg);
	odomAxes_->setZValue(960.0);
	odomAxes_->setVisible(axesVisible_);

	 baseFootprintAxes_->setPos(baseScene);
	 baseFootprintAxes_->setRotation(baseYawDeg);
	 baseFootprintAxes_->setZValue(970.0);
	 baseFootprintAxes_->setVisible(axesVisible_);

	// Update robot UI icon using Option A converter (map meters -> scene/image px)
    if (robotUi_) {
    	const int w = map.width();
    	const int h = map.height();
    	const double res = static_cast<double>(map.resolution());
    	const double ox = static_cast<double>(map.originX());
    	const double oy = static_cast<double>(map.originY());
    	const auto pose = RomRobotUi::tfPoseToImagePose(x, y, baseYawDeg, w, h, res, ox, oy);

    	robotUi_->setImagePosition(pose.x, pose.y, pose.theta_deg);
    	// visibility controlled independently via setRobotIconVisible()
    }

	if (!frontLaserItem_) 
	{
        frontLaserItem_ = new RomLaserGraphicsItem();
        scene_->addItem(frontLaserItem_);
        frontLaserItem_->setVisible(axesVisible_); // or true if you want independent control
    }

    if (auto gli = static_cast<RomLaserGraphicsItem*>(frontLaserItem_)) {
        // scale: scene pixels per meter (use 1/resolution to match map pixels)
        if (mapResolution_ > 0.0) {
            gli->setMetersToPixels(1.0 / mapResolution_);
        }
        gli->updateFrom(scan);
		// gli->setColor(Qt::red); // optional override
		gli->setPointRadius(0.3);
    }

	// // Keep the front laser item aligned with the robot base frame (position + yaw)
	if (frontLaserItem_) {
        frontLaserItem_->setPos(baseScene);
        frontLaserItem_->setRotation(baseYawDeg);
		frontLaserItem_->setZValue(965.0); // between odom (960) and base footprint (970)
		frontLaserItem_->setVisible(axesVisible_);
	}

}

void MapWidget::updateFrontLaser(const RomLaser& scan)
{
	if (!frontLaserItem_) 
	{
        frontLaserItem_ = new RomLaserGraphicsItem();
        scene_->addItem(frontLaserItem_);
        frontLaserItem_->setVisible(axesVisible_); // or true if you want independent control
    }

    if (auto gli = static_cast<RomLaserGraphicsItem*>(frontLaserItem_)) {
        // scale: scene pixels per meter (use 1/resolution to match map pixels)
        if (mapResolution_ > 0.0) {
            gli->setMetersToPixels(1.0 / mapResolution_);
        }
        gli->updateFrom(scan);
		//gli->setColor(Qt::green); // optional override
    }

	// if (frontLaserItem_) 
	// {
    // frontLaserItem_->setPos(baseScene);
    // frontLaserItem_->setRotation(baseYawDeg);
    // frontLaserItem_->setZValue(965.0);     
    // frontLaserItem_->setVisible(axesVisible_);
	// }
}
void MapWidget::setAxesVisible(bool visible)
{
	axesVisible_ = visible;
	if (viewAxes_) viewAxes_->setVisible(visible);
	if (mapOriginAxes_) mapOriginAxes_->setVisible(visible);
	if (odomAxes_) odomAxes_->setVisible(visible);
	if (baseFootprintAxes_) baseFootprintAxes_->setVisible(visible);
}

void MapWidget::setRobotIconVisible(bool visible)
{
	if (robotUi_) robotUi_->setVisible(visible);
}

void MapWidget::clearFrontLaser()
{
	if (frontLaserItem_) {
		if (scene_) scene_->removeItem(frontLaserItem_);
		delete frontLaserItem_;
		frontLaserItem_ = nullptr;
	}
}

void MapWidget::setMap(const RomMap& map)
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

	// Ensure a map-origin axes item exists and position it at the map-frame origin (0,0)
	// Compute scene coordinates from OccupancyGrid metadata:
	//   - origin_x/origin_y are world meters of pixel (0,0) (bottom-left in map coords)
	//   - resolution is meters/pixel
	//   - We flipped rows when creating the image, so pixel_y_top = h-1 - j (j is map row)
	//   - The pixmap is centered at (0,0) via setOffset(-w/2, -h/2)
	// Therefore for world (0,0):
	//   pixel_x_from_left = -origin_x / resolution
	//   pixel_y_from_top  = (h - 1) + origin_y / resolution
	//   scene_x = pixel_x_from_left - w/2
	//   scene_y = pixel_y_from_top  - h/2
	if (!mapOriginAxes_) {
		mapOriginAxes_ = new MapOriginCoordinateFrame();
		scene_->addItem(mapOriginAxes_);
		mapOriginAxes_->setVisible(axesVisible_);
	}
	const int w = map.width();
	const int h = map.height();
	const float res = map.resolution();
	if (w > 0 && h > 0 && res > 0.0f) {
		const double ox = static_cast<double>(map.originX());
		const double oy = static_cast<double>(map.originY());
		const double pixel_x = -ox / res;
		const double pixel_y = (static_cast<double>(h) - 1.0) + oy / res;
		const double scene_x = pixel_x - static_cast<double>(w) / 2.0;
		const double scene_y = pixel_y - static_cast<double>(h) / 2.0;
		mapOriginAxes_->setPos(scene_x, scene_y);
		mapOriginAxes_->setVisible(axesVisible_);
	} else if (mapOriginAxes_) {
		mapOriginAxes_->setVisible(false);
	}

	if (autoFit_ && newSize != lastMapSize_) {
		fitInView(mapItem_, Qt::KeepAspectRatio);
		lastMapSize_ = newSize;
	}
}

void MapWidget::clearMap()
{
	mapItem_->setPixmap(QPixmap());
	scene_->setSceneRect(QRectF());
	lastMapSize_ = QSize();
}

void MapWidget::resizeEvent(QResizeEvent* e)
{
	QGraphicsView::resizeEvent(e);
	if (autoFit_ && mapItem_ && !mapItem_->pixmap().isNull()) {
		fitInView(mapItem_, Qt::KeepAspectRatio);
	}
}

void MapWidget::wheelEvent(QWheelEvent* event)
{
	if (!mapItem_ || mapItem_->pixmap().isNull()) {
		QGraphicsView::wheelEvent(event);
		return;
	}

	// Disable auto-fit after the user starts zooming manually
	autoFit_ = false;
	userZoomed_ = true;

	constexpr double zoomInFactor = 1.15;
	constexpr double zoomOutFactor = 1.0 / zoomInFactor;

	const double factor = (event->angleDelta().y() > 0) ? zoomInFactor : zoomOutFactor;

	// Compute new scale while clamping to min/max
	QTransform t = transform();
	// current uniform scale assuming no shear
	const double currentScale = std::sqrt(t.m11() * t.m11() + t.m12() * t.m12());
	double newScale = currentScale * factor;
	if (newScale < minScale_) newScale = minScale_;
	if (newScale > maxScale_) newScale = maxScale_;

	// Apply by resetting scale to target factor relative to current
	const double applyFactor = newScale / currentScale;
	scale(applyFactor, applyFactor);

	event->accept();
}

void MapWidget::fitToView()
{
	if (mapItem_ && !mapItem_->pixmap().isNull()) {
		// Re-enable autoFit for this operation, and reset user zoom flag
		autoFit_ = true;
		userZoomed_ = false;
		fitInView(mapItem_, Qt::KeepAspectRatio);
	}
}

void MapWidget::addWaypointMarker(double image_x_px, double image_y_px, double image_theta_deg, const QString& name)
{
	if (!scene_) return;
	QPixmap pix(":/icons/wp_waypoint.png");
	if (pix.isNull()) return;
	const int W = 25, H = 25;
	QPixmap scaled = pix.scaled(W, H, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	auto* item = scene_->addPixmap(scaled);
	item->setTransformationMode(Qt::SmoothTransformation);
	item->setOffset(-W/2.0, -H/2.0);
	item->setPos(image_x_px, image_y_px);
	item->setRotation(image_theta_deg);
	item->setZValue(985.0);
	item->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
	// Tag as waypoint marker and store its name for later lookup/removal
	item->setData(1001, true);           // waypoint marker flag
	item->setData(1002, name);           // waypoint name
	if (!name.isEmpty()) 
	{
		// Use HTML tooltip to control font a bit; SF Pro Text must be installed
		// If not installed, system will fallback.
		item->setToolTip(QString("<div style=\"font-family:'SF Pro Text'; font-size:10px;\">%1</div>").arg(name.toHtmlEscaped()));
		auto *label = scene_->addText(name, QFont("SF Pro Text", 10));
		label->setDefaultTextColor(Qt::black);
		label->setPos(image_x_px + 0.5 , image_y_px ); // right of icon
		label->setZValue(985.0);
		label->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		// Tag label as waypoint marker too for easy removal
		label->setData(1001, true);           // waypoint marker flag
		label->setData(1002, name);           // waypoint name	
	}
	
	
}

void MapWidget::clearWaypointMarkers()
{
	if (!scene_) return;
	// Collect then delete to avoid mutating while iterating
	QList<QGraphicsItem*> toRemove;
	const auto itemsList = scene_->items();
	for (QGraphicsItem* it : itemsList) {
		if (!it) continue;
		const QVariant flag = it->data(1001);
		if (flag.isValid() && flag.toBool()) {
			toRemove.append(it);
		}
	}
	for (QGraphicsItem* it : toRemove) {
		scene_->removeItem(it);
		delete it;
	}
}

void MapWidget::mousePressEvent(QMouseEvent* event)
{
	if (!waypointModeEnabled_ || !mapItem_ || mapItem_->pixmap().isNull()) {
		QGraphicsView::mousePressEvent(event);
		return;
	}
	if (event->button() != Qt::LeftButton) {
		QGraphicsView::mousePressEvent(event);
		return;
	}

	// Begin waypoint placement/rotation
	const QPointF scenePos = mapToScene(event->pos());
	waypointPressScene_ = scenePos;
	waypointDragging_ = true;
	waypointAngleDeg_ = 0.0;

	// Create preview icon if needed
	if (!waypointPreview_) {
		QPixmap pix(":/icons/wp_waypoint.png");
		if (!pix.isNull()) {
			const int W = 25, H = 25;
			QPixmap scaled = pix.scaled(W, H, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			waypointPreview_ = scene_->addPixmap(scaled);
			waypointPreview_->setTransformationMode(Qt::SmoothTransformation);
			waypointPreview_->setOffset(-W/2.0, -H/2.0);
			waypointPreview_->setZValue(990.0);
			waypointPreview_->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		}
	}
	if (waypointPreview_) {
		waypointPreview_->setPos(scenePos);
		waypointPreview_->setRotation(0.0);
		waypointPreview_->setVisible(true);
	}
	event->accept();
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!waypointModeEnabled_ || !waypointDragging_) {
		QGraphicsView::mouseMoveEvent(event);
		return;
	}
	const QPointF scenePos = mapToScene(event->pos());
	const QPointF d = scenePos - waypointPressScene_;
	double angleRad = std::atan2(d.y(), d.x()); // scene coords: +x right, +y down
	double angleDeg = angleRad * 180.0 / M_PI;
	waypointAngleDeg_ = angleDeg; // image angle convention
	if (waypointPreview_) {
		waypointPreview_->setPos(waypointPressScene_);
		waypointPreview_->setRotation(waypointAngleDeg_);
	}
	event->accept();
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (!waypointModeEnabled_ || !waypointDragging_) {
		QGraphicsView::mouseReleaseEvent(event);
		return;
	}
	
	if (event->button() != Qt::LeftButton) {
		QGraphicsView::mouseReleaseEvent(event);
		return;
	}

	// Finalize waypoint placement
	const QPointF scenePos = waypointPressScene_;
	const double image_x = scenePos.x();
	const double image_y = scenePos.y();
	const double image_theta_deg = waypointAngleDeg_;

	// Convert to world (map meters). Convert image deg to world yaw radians.
	const QPointF w = sceneToWorld_mapFrame(image_x, image_y, mapWidth_, mapHeight_, mapResolution_, mapOriginX_, mapOriginY_);
	
	// Inverse of Option A from RomRobotUi::imagePoseToTfPose: robot_theta_deg = -image_theta_deg
	const double world_theta_rad = (-image_theta_deg) * M_PI / 180.0;

	emit waypointPlaced(image_x, image_y, image_theta_deg, w.x(), w.y(), world_theta_rad);

	// Keep the preview visible at the placed location until next interaction (optional)
	// Or hide it after placement; here we hide to reduce clutter
	if (waypointPreview_) {
		waypointPreview_->setVisible(false);
	}
	waypointDragging_ = false;
	event->accept();
}

// ////////////////////////////////////////////// END /////////////////////////////////////////////
