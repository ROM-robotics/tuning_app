#include "covarianceDisplay.hpp" // Assumed header location
#include <QVBoxLayout>
#include <cmath>
#include <QtMath>
#include <limits> // For numeric_limits

using namespace rom_dynamics::ui::qt;
using namespace Eigen;

//================================================================================
// 1. RomPositionCovarianceGraph Implementation
//================================================================================

rom_dynamics::ui::qt::RomPositionCovarianceGraph::RomPositionCovarianceGraph(QWidget *parent)
    : QWidget(parent)
{
    initializeChart();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chart_view_);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void rom_dynamics::ui::qt::RomPositionCovarianceGraph::initializeChart()
{
    // Chart for X-Y Position Ellipse
    chart_ = new QChart();
    QColor titleColor(QColorConstants::Svg::orange);
    chart_->setTitle("Covariance of EKF Position");
    chart_->setTitleBrush(QBrush(titleColor));
    chart_->setBackgroundBrush(Qt::NoBrush);
    chart_->setPlotAreaBackgroundBrush(Qt::NoBrush);
    chart_->setBackgroundRoundness(0);
    chart_->legend()->hide();
    
    ellipse_series_ = new QScatterSeries();
    ellipse_series_->setMarkerSize(3.0);
    ellipse_series_->setColor(QColor(255, 0, 0)); // Red
    ellipse_series_->setName("95% Confidence Ellipse");
    chart_->addSeries(ellipse_series_);

    // Axis setup
    axisX_ = new QValueAxis();
    // axisX_->setTitleText("X Position (m)");
    axisY_ = new QValueAxis();
    // axisY_->setTitleText("Y Position (m)");

    // Set axis label color to white
    QColor axisColor(189, 183, 183, 80); 
    axisX_->setGridLineColor(axisColor);
    axisY_->setGridLineColor(axisColor);
    axisX_->setLabelsColor(axisColor);
    axisY_->setLabelsColor(axisColor);
    axisX_->setLinePen(QPen(axisColor));
    axisY_->setLinePen(QPen(axisColor));

    QFont axisFont;
    axisFont.setFamily("SF Pro Text");
    axisFont.setPointSize(7); // smaller font size for axis
    axisX_->setLabelsFont(axisFont);
    axisY_->setLabelsFont(axisFont);

    // Initial Range
    axisX_->setRange(-1.0, 1.0);
    axisY_->setRange(-1.0, 1.0);
    
    chart_->addAxis(axisX_, Qt::AlignBottom);
    chart_->addAxis(axisY_, Qt::AlignLeft);
    ellipse_series_->attachAxis(axisX_);
    ellipse_series_->attachAxis(axisY_);
    
    chart_view_ = new QChartView(chart_);
    chart_view_->setRenderHint(QPainter::Antialiasing);
}

void rom_dynamics::ui::qt::RomPositionCovarianceGraph::updateGraph(double ekf_pose_x, 
                                             double ekf_pose_y, 
                                             const Eigen::Matrix2d& covariance_xy)
{
    current_x_ = ekf_pose_x;
    current_y_ = ekf_pose_y;
    
    drawPositionEllipse(covariance_xy);
}

void rom_dynamics::ui::qt::RomPositionCovarianceGraph::drawPositionEllipse(const Eigen::Matrix2d& covariance_xy)
{
    // Calculate Eigenvalues and Eigenvectors for the Ellipse
    EigenSolver<Matrix2d> es(covariance_xy);
    Vector2d eigenvalues = es.eigenvalues().real();
    Matrix2d eigenvectors = es.eigenvectors().real();

    // 95% confidence chi-squared value for 2 DOF
    double k = 2.4477; 
    double major_axis = k * std::sqrt(eigenvalues(0));
    double minor_axis = k * std::sqrt(eigenvalues(1));
    double angle = std::atan2(eigenvectors(1, 0), eigenvectors(0, 0)); // Angle of major axis

    ellipse_series_->clear();
    
    // Generate Ellipse points
    const int num_points = 50;
    for (int i = 0; i <= num_points; ++i) 
    {
        double theta = 2.0 * M_PI * i / num_points;
        double x_rotated = major_axis * std::cos(theta);
        double y_rotated = minor_axis * std::sin(theta);

        // --- NORMALIZATION: Rotate and Translate to (0, 0) ---
        // Original: double x_point = current_x_ + (x_rotated * std::cos(angle) - y_rotated * std::sin(angle));
        // Original: double y_point = current_y_ + (x_rotated * std::sin(angle) + y_rotated * std::cos(angle));

        double x_point = (x_rotated * std::cos(angle) - y_rotated * std::sin(angle));
        double y_point = (x_rotated * std::sin(angle) + y_rotated * std::cos(angle));

        ellipse_series_->append(x_point, y_point);
    }
    
    // --- AXIS RANGING: Dynamically center on (0, 0) and scale to the ellipse size ---
    double range = std::max(major_axis, minor_axis) * 1.2; // 20% padding
    
    // Ensure a minimum range if covariance is near zero
    if (range < 0.1) range = 0.1;

    axisX_->setRange(-range, range);
    axisY_->setRange(-range, range);

    // OPTIONAL: Update title with uncertainty metrics
    QFont titleFont;
    QColor titleColor(QColorConstants::Svg::orange);
    titleFont.setFamily("SF Pro Text");
    titleFont.setPointSize(10);

    QString title = QString("EKF Position Covariance x,y");
    chart_->setTitle(title);

    chart_->setTitleBrush(QBrush(titleColor));
    chart_->setTitleFont(titleFont);
}

//================================================================================
// 2. RomYawCovarianceGraph Implementation
//================================================================================

rom_dynamics::ui::qt::RomYawCovarianceGraph::RomYawCovarianceGraph(QWidget *parent)
    : QWidget(parent)
{
    initializeChart();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chart_view_);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void rom_dynamics::ui::qt::RomYawCovarianceGraph::initializeChart()
{
    chart_ = new QPolarChart();

    QFont titleFont;
    QColor titleColor(QColorConstants::Svg::orange);
    titleFont.setFamily("SF Pro Text");
    titleFont.setPointSize(10);

    chart_->setTitle("EKF Yaw Covariance yaw Polar View");
    chart_->setTitleBrush(QBrush(titleColor));
    chart_->setTitleFont(titleFont);

    chart_->setBackgroundBrush(Qt::NoBrush);
    chart_->setPlotAreaBackgroundBrush(Qt::NoBrush);
    chart_->setBackgroundRoundness(0);
    chart_->legend()->hide();
    
    // Axis setup (Polar Chart uses Radial and Angular axes)
    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setRange(0, 1); // Normalized radius
    radialAxis->setLabelsVisible(false);
    radialAxis->setLinePen(Qt::NoPen);

    QFont axisFont;
    axisFont.setFamily("SF Pro Text");
    axisFont.setPointSize(7); // smaller font size for axis
    radialAxis->setLabelsFont(axisFont);

    chart_->addAxis(radialAxis, QPolarChart::PolarOrientationRadial); 

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setRange(0, 360); // Full circle in degrees
    angularAxis->setTickCount(13); // 0, 30, 60, ..., 330
    angularAxis->setLabelsFont(axisFont);

    // Set axis label color to white
    QColor axisColor(189, 183, 183, 80); 
    radialAxis->setGridLineColor(axisColor);
    angularAxis->setGridLineColor(axisColor);

    radialAxis->setLabelsColor(axisColor);
    angularAxis->setLabelsColor(axisColor);

    radialAxis->setLinePen(QPen(axisColor));
    angularAxis->setLinePen(QPen(axisColor));

    chart_->addAxis(angularAxis, QPolarChart::PolarOrientationAngular); 
    
    // Series for Yaw Arc
    yaw_arc_series_ = new QLineSeries();
    yaw_arc_series_->setColor(QColor(0, 0, 255)); // Blue
    QPen pen = yaw_arc_series_->pen();
    pen.setWidth(4); // Thicker line for sector boundary
    yaw_arc_series_->setPen(pen);
    chart_->addSeries(yaw_arc_series_);
    yaw_arc_series_->attachAxis(radialAxis);
    yaw_arc_series_->attachAxis(angularAxis);
    
    // Dummy scatter to correctly map chart axes
    QScatterSeries *dummyScatter = new QScatterSeries();
    dummyScatter->append(0, 0);
    dummyScatter->setMarkerSize(0);
    chart_->addSeries(dummyScatter);
    dummyScatter->attachAxis(radialAxis); 
    dummyScatter->attachAxis(angularAxis); 
    
    chart_view_ = new QChartView(chart_);
    chart_view_->setRenderHint(QPainter::Antialiasing);
}

void rom_dynamics::ui::qt::RomYawCovarianceGraph::updateGraph(double ekf_pose_yaw, double yaw_variance)
{
    current_yaw_ = ekf_pose_yaw;
    drawYawSector(ekf_pose_yaw, yaw_variance);
}

void rom_dynamics::ui::qt::RomYawCovarianceGraph::drawYawSector(double yaw, double yaw_variance)
{
    double sigma_yaw = std::sqrt(yaw_variance);
    // 95% confidence interval (approx k=2 for normal distribution)
    double k = 2.0; 
    
    double yaw_min = yaw - k * sigma_yaw;
    double yaw_max = yaw + k * sigma_yaw;

    // Radius for the arc (1.0 is the max radial range)
    const double radius = 0.95; 

    yaw_arc_series_->clear();

    // Start point of the arc (Center of the polar chart: radius=0, angle=0)
    yaw_arc_series_->append(0, 0); 

    const int num_segments = 30;
    double yaw_step = (yaw_max - yaw_min) / num_segments;

    // Generate points for the arc
    for (int i = 0; i <= num_segments; ++i) 
    {
        double current_angle = yaw_min + i * yaw_step;
        
        // Polar Chart coordinates are (radius, angle)
        // Convert the angle from radians to QPolarChart's degrees [0, 360]
        qreal angle_degrees = radiansToDegrees(current_angle);
        
        yaw_arc_series_->append(radius, angle_degrees);
    }
    
    // End point of the arc (back to the center to close the sector)
    yaw_arc_series_->append(0, 0);

    // Update the title to show uncertainty value
    double confidence_95_deg = k * sigma_yaw * 180.0 / M_PI; 
    QString title = QString("Covariance of EKF Yaw");
    chart_->setTitle(title);
}
